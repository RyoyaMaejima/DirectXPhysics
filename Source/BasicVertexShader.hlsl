#include "BasicType.hlsli"

//変換をまとめた構造体
cbuffer SceneData : register(b0)
{
    matrix world;//ワールド変換行列
    matrix viewproj; //ビュープロジェクション行列
};

//ボーン用
cbuffer BoneData : register(b1)
{
    matrix bones[256]; //ボーン行列
};

BasicType BasicVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD0, float3 color : COLOR, unsigned int id : TEXCOORD1, min16uint2 boneno : BONE_NO, min16uint weight : WEIGHT)
{
    BasicType output;//ピクセルシェーダーへ渡す値
    if (id == 3)
    {
        float w = (float) weight / 100.0f;
        matrix bm = bones[boneno[0]] * w + bones[boneno[1]] * (1.0f - w);
        pos = mul(bm, pos);
    }
    output.svpos = mul(mul(viewproj, world), pos);
    normal.w = 0;//平行移動成分を無効にする
    output.normal = mul(world, normal);//法線にもワールド変換を行う
    output.uv = uv;
    output.color = color;
    output.id = id;
    return output;
}
