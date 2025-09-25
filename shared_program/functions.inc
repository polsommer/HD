// ======================================================================
// functions_dx9_ultimate.inc
// Utility helpers tuned for Shader Model 3.0 (DirectX 9)
// Focused on predictable ALU usage and compatibility with SWG's renderer
// ======================================================================

static const float SMALL_EPSILON = 1.0e-6f;

// ----------------------------------------------------------------------
// texture access helpers ------------------------------------------------
// Provide a single entry point for SM3.0 vertex and pixel shaders. When
// compiling as a vertex shader we fall back to tex2Dlod which is available
// in vs_3_0, while pixel shaders keep the hardware filtered tex2D path.
// ----------------------------------------------------------------------

float4 sampleTextureDX9(sampler2D map, float2 uv)
{
#if defined(VERTEX_SHADER_VERSION) && (VERTEX_SHADER_VERSION >= 30)
    return tex2Dlod(map, float4(uv, 0.0f, 0.0f));
#else
    return tex2D(map, uv);
#endif
}

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
    float lenSq = max(dot(v, v), SMALL_EPSILON);
    return v * rsqrt(lenSq);
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
    float4 px = sampleTextureDX9(map, uv);
    float2 xy = (float2(px.a, px.g) - 0.5f) * 2.0f;
    float z = sqrt(saturate(1.0f - dot(xy, xy)));
    return float3(xy, z);
}

float3 decodeNormalDXT5Fast(sampler2D map, float2 uv)
{
    float4 px = sampleTextureDX9(map, uv);
    float2 xy = (float2(px.a, px.g) - 0.5f) * 2.0f;
    float d = dot(xy, xy);
    float useHighCurve = (d >= 0.24f) ? 1.0f : 0.0f;
    float zHigh = d * 1.6667f + 0.15f;
    float zLow  = d * 0.6667f + 0.375f;
    float z = lerp(zLow, zHigh, useHighCurve);
    return fastNormalize(float3(xy, z));
}

float3 computeNormalFromHeight(sampler2D heightMap, float2 uv, float scale)
{
    float hL = sampleTextureDX9(heightMap, uv + float2(-scale, 0.0f)).r;
    float hR = sampleTextureDX9(heightMap, uv + float2( scale, 0.0f)).r;
    float hD = sampleTextureDX9(heightMap, uv + float2(0.0f, -scale)).r;
    float hU = sampleTextureDX9(heightMap, uv + float2(0.0f,  scale)).r;
    float3 n = float3(hL - hR, hD - hU, 2.0f * scale);
    return fastNormalize(n);
}

// -------------------- LIGHTING --------------------

float safePow(float base, float exponent)
{
    return pow(saturate(base), exponent);
}

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
    float3 n = fastNormalize(normal);
    float3 l = fastNormalize(lightDir);
    float3 v = fastNormalize(viewDir);
    float3 halfDir = fastNormalize(l + v);
    float diff = saturate(dot(n, l));
    float spec = safePow(dot(n, halfDir), max(gloss * 128.0f, 1.0f));
    float fresnel = safePow(1.0f - saturate(dot(v, n)), 5.0f);
    float3 specColor = lerp(float3(1.0f, 1.0f, 1.0f), lightColor, saturate(metalness));
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
    base = fastNormalize(base);
    detail = fastNormalize(detail);
    float3 up = float3(0.0f, 0.0f, 1.0f);
    float3 t = fastNormalize(cross(up, base));
    float3 b = cross(base, t);
    float3x3 TBN = float3x3(t, b, base);
    return fastNormalize(mul(detail, TBN));
}

// ======================================================================
// END OF ULTIMATE DX9 SHADER UTILITY PACK (MAX QUALITY)
// ======================================================================
