#include "BasicType.hlsli"

//�ϊ����܂Ƃ߂��\����
cbuffer cbuff0 : register(b0)
{
    matrix mat; //�ϊ��s��
};

BasicType BasicVS(float4 pos : POSITION, float2 uv : TEXCOORD0, unsigned int id : TEXCOORD1)
{
    BasicType output; //�s�N�Z���V�F�[�_�[�֓n���l
    output.svpos = mul(mat, pos);
    output.uv = uv;
    output.id = id;
    return output;
}
