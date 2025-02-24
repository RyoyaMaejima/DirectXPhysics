#include "BasicType.hlsli"

//変換をまとめた構造体
cbuffer cbuff0 : register(b0)
{
    matrix world;//ワールド変換行列
    matrix viewproj; //ビュープロジェクション行列
};

BasicType BasicVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD0, unsigned int id : TEXCOORD1)
{
    BasicType output;//ピクセルシェーダーへ渡す値
    output.svpos = mul(mul(viewproj, world), pos);
    normal.w = 0;//平行移動成分を無効にする
    output.normal = mul(world, normal);//法線にもワールド変換を行う
    output.uv = uv;
    output.id = id;
    return output;
}
