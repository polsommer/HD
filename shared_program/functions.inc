// ======================================================================
// functions_dx9_ultimate.inc
// Ultimate DX9 Shader Utility Pack (SM3.0-Optimized)
// Max Quality: Full lighting, material decoding, normal handling
// Compatible with SWG and high-end DX9 visuals
// ======================================================================

// -------------------- BASIC MATH --------------------

float lengthSquared(float3 v)
{
    return dot(v, v);
}

float3 signAndBias(float3 v)
{
    return (v - 0.5f) * 2.0f;
}

float3 reverseSignAndBias(float3 v)
{
    return (v * 0.5f) + 0.5f;
}

float3 fastNormalize(float3 v)
{
    return v * rsqrt(dot(v, v) + 1e-6f);
}

// -------------------- NORMAL DECODING --------------------

float3 decodeNormalRG(float2 enc)
{
    float2 xy = enc * 2.0f - 1.0f;
    float z = sqrt(saturate(1.0f - dot(xy, xy)));
    return float3(xy, z);
}

float3 decodeNormalDXT5(sampler2D map, float2 uv)
{
    float4 px = tex2D(map, uv);
    float2 xy = (float2(px.a, px.g) - 0.5f) * 2.0f;
    float z = sqrt(saturate(1.0f - dot(xy, xy)));
    return float3(xy, z);
}

float3 decodeNormalDXT5Fast(sampler2D map, float2 uv)
{
    float4 px = tex2D(map, uv);
    float2 xy = (float2(px.a, px.g) - 0.5f) * 2.0f;
    float d = dot(xy, xy);
    float z = lerp(d * 1.6667f + 0.15f, d * 0.6667f + 0.375f, step(0.24f, d));
    return fastNormalize(float3(xy, z));
}

float3 computeNormalFromHeight(sampler2D heightMap, float2 uv, float scale)
{
    float hL = tex2D(heightMap, uv + float2(-scale, 0)).r;
    float hR = tex2D(heightMap, uv + float2(scale, 0)).r;
    float hD = tex2D(heightMap, uv + float2(0, -scale)).r;
    float hU = tex2D(heightMap, uv + float2(0, scale)).r;
    float3 n = normalize(float3(hL - hR, hD - hU, 2.0f * scale));
    return n;
}

// -------------------- LIGHTING --------------------

// Full Blinn-Phong lighting with Fresnel and ambient occlusion
float3 computeLighting(
    float3 normal,
    float3 lightDir,
    float3 viewDir,
    float3 lightColor,
    float gloss,
    float metalness,
    float ao
)
{
    float3 halfDir = normalize(lightDir + viewDir);
    float diff = saturate(dot(normal, lightDir));
    float spec = pow(saturate(dot(normal, halfDir)), gloss * 128.0f);
    float fresnel = pow(1.0f - saturate(dot(viewDir, normal)), 5.0f);
    float3 specColor = lerp(1.0f.xxx, lightColor, metalness);
    float3 final = (diff * lightColor + spec * specColor * fresnel) * ao;
    return final;
}

// -------------------- UTILITIES --------------------

float intensity(float3 rgb)
{
    return dot(rgb, float3(0.3f, 0.59f, 0.11f));
}

void unpackMaterialParams(float4 packed, out float gloss, out float metal, out float ao, out float detail)
{
    gloss = packed.r;
    metal = packed.g;
    ao    = packed.b;
    detail = packed.a;
}

float3 decodeGlossNormal(float4 px, out float gloss)
{
    float2 xy = (float2(px.a, px.g) - 0.5f) * 2.0f;
    float z = sqrt(saturate(1.0f - dot(xy, xy)));
    gloss = px.r;
    return float3(xy, z);
}

// -------------------- OPTIONAL FX --------------------

// Tone mapping using Reinhard approximation
float3 toneMapReinhard(float3 color)
{
    return color / (color + 1.0f);
}

// Detail normal map blending
float3 blendNormals(float3 base, float3 detail)
{
    base = normalize(base);
    detail = normalize(detail);
    float3 up = float3(0.0f, 0.0f, 1.0f);
    float3 t = normalize(cross(up, base));
    float3 b = cross(base, t);
    float3x3 TBN = float3x3(t, b, base);
    return normalize(mul(detail, TBN));
}

// ======================================================================
// END OF ULTIMATE DX9 SHADER UTILITY PACK (MAX QUALITY)
// ======================================================================