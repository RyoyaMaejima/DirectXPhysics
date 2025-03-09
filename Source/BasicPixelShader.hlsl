#include "BasicType.hlsli"

Texture2D<float4> tex : register(t0);//0番スロットに設定されたテクスチャ
Texture2D<float4> tex1 : register(t1); //1番スロットに設定されたテクスチャ
SamplerState smp : register(s0);//0番スロットに設定されたサンプラ

//マテリアル用
cbuffer Material : register(b2)
{
    float4 diffuse; //ディフューズ色
    float4 specular; //スペキュラ
    float3 ambient; //アンビエント
};

float4 BasicPS(BasicType input) : SV_TARGET
{
    if (input.id == 0)
    {
        float3 light = normalize(float3(1, -1, 1));
        float brightness = dot(-light, input.normal);
        return float4(brightness * input.color, 1);
    }
    else if (input.id == 1)
    {
        return float4(0, 0, 0, 1);
    }
    else if (input.id == 2)
    {
        return float4(tex.Sample(smp, input.uv));
    }
    else if (input.id == 3)
    {
        float3 light = normalize(float3(1, -1, 1));
        float brightness = dot(-light, input.normal);
        return float4(brightness , brightness, brightness, 1) * diffuse * tex1.Sample(smp, input.uv);
    }
    else
    {
        return float4(0, 0, 0, 1);
    }
}