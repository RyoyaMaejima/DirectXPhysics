#include "BasicType.hlsli"

//�ϊ����܂Ƃ߂��\����
cbuffer cbuff0 : register(b0)
{
    matrix world;//���[���h�ϊ��s��
    matrix viewproj; //�r���[�v���W�F�N�V�����s��
};

BasicType BasicVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD0, unsigned int id : TEXCOORD1)
{
    BasicType output;//�s�N�Z���V�F�[�_�[�֓n���l
    output.svpos = mul(mul(viewproj, world), pos);
    normal.w = 0;//���s�ړ������𖳌��ɂ���
    output.normal = mul(world, normal);//�@���ɂ����[���h�ϊ����s��
    output.uv = uv;
    output.id = id;
    return output;
}
