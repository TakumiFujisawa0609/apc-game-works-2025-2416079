// ピクセルシェーダーの入力
struct PS_INPUT
{
    float4 Position : SV_POSITION; // 座標( プロジェクション空間 )
    float4 Diffuse : COLOR0; // ディフューズカラー
    float4 Specular : COLOR1; // スペキュラカラー
    float2 TexCoords0 : TEXCOORD0; // テクスチャ座標
};

// 定数バッファ：スロット0番目(b0と書く)
cbuffer cbParam : register(b0)
{
    float g_sizeX;
    float g_sizeY;
    float g_time;
}

// 描画するテクスチャ
Texture2D g_SrcTexture : register(t0);

// サンプラー：適切な色を決める処理
SamplerState g_SrcSampler : register(s0);

float4 main(PS_INPUT PSInput) : SV_TARGET
{
    //UV座標
    float2 uv = PSInput.TexCoords0 - 0.5f;
    
    if (abs(uv.x) <= 0.38f)
    {
        return g_SrcTexture.Sample(g_SrcSampler, uv + 0.5f);
    }
    
    //ジャギーのサイズ割り出し
    float scaleX = g_sizeX / g_time * 1.5f;
    float scaleY = g_sizeY / g_time * 1.5f;
    
    //サイズ分にテクスチャ座標を縮める
    uv.x = floor(uv.x * scaleX) / scaleX;
    uv.y = floor(uv.y * scaleY) / scaleY;
    
    uv += 0.5f;

    float4 srcCol = g_SrcTexture.Sample(g_SrcSampler, uv);
    
    return srcCol;
}