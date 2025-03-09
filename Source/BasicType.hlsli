//頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct BasicType
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float4 normal : NORMAL; //法線ベクトル
    float2 uv : TEXCOORD0; //UV値
    float3 color : COLOR; //色
    uint id : TEXCOORD1; //ID
    min16uint2 boneno : BONE_NO;//ボーン番号
    min16uint weight : WEIGHT;//ボーン影響度
};