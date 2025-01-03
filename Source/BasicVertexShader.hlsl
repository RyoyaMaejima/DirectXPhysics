#include "BasicType.hlsli"

//変換をまとめた構造体
cbuffer cbuff0 : register(b0)
{
    matrix mat; //変換行列
};

BasicType BasicVS(float4 pos : POSITION, float2 uv : TEXCOORD0, unsigned int id : TEXCOORD1)
{
    BasicType output; //ピクセルシェーダーへ渡す値
    output.svpos = mul(mat, pos);
    output.uv = uv;
    output.id = id;
    return output;
}
