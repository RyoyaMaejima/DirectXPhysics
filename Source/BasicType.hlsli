//���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct BasicType
{
    float4 svpos : SV_POSITION; //�V�X�e���p���_���W
    float4 normal : NORMAL; //�@���x�N�g��
    float2 uv : TEXCOORD0; //UV�l
    float3 color : COLOR; //�F
    uint id : TEXCOORD1; //ID
    min16uint2 boneno : BONE_NO;//�{�[���ԍ�
    min16uint weight : WEIGHT;//�{�[���e���x
};