#include "BasicType.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v��

float4 BasicPS(BasicType input) : SV_TARGET
{
    if (input.id == 0)
    {
        return float4(1, 0, 0, 1);
    }
    else if (input.id == 1)
    {
        return float4(0, 0, 1, 1);
    }
    else
    {
        return float4(tex.Sample(smp, input.uv));
    }
}