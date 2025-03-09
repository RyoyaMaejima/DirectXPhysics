#include "BasicType.hlsli"

//�ϊ����܂Ƃ߂��\����
cbuffer SceneData : register(b0)
{
    matrix world;//���[���h�ϊ��s��
    matrix viewproj; //�r���[�v���W�F�N�V�����s��
};

//�{�[���p
cbuffer BoneData : register(b1)
{
    matrix bones[256]; //�{�[���s��
};

BasicType BasicVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD0, float3 color : COLOR, unsigned int id : TEXCOORD1, min16uint2 boneno : BONE_NO, min16uint weight : WEIGHT)
{
    BasicType output;//�s�N�Z���V�F�[�_�[�֓n���l
    if (id == 3)
    {
        float w = (float) weight / 100.0f;
        matrix bm = bones[boneno[0]] * w + bones[boneno[1]] * (1.0f - w);
        pos = mul(bm, pos);
    }
    output.svpos = mul(mul(viewproj, world), pos);
    normal.w = 0;//���s�ړ������𖳌��ɂ���
    output.normal = mul(world, normal);//�@���ɂ����[���h�ϊ����s��
    output.uv = uv;
    output.color = color;
    output.id = id;
    return output;
}
