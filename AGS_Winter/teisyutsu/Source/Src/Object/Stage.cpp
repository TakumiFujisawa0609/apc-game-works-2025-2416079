#include <DxLib.h>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "Stage.h"


Stage::Stage(void)
{
}

Stage::~Stage(void)
{
}

void Stage::Init(void)
{
	// 外部ファイルの３Ｄモデルをロード
	modelId_ = MV1LoadModel((Application::PATH_MODEL + "Stage.mv1").c_str());

	// ３Ｄモデルの大きさを設定(引数は、x, y, zの倍率)
	MV1SetScale(modelId_, VScale(AsoUtility::VECTOR_ONE, 2.0f));

	// ３Ｄモデルの位置(引数は、３Ｄ座標)
	MV1SetPosition(modelId_, { 0.0f, -180.0f, 0.0f });

	// ３Ｄモデルの向き(引数は、x, y, zの回転量。単位はラジアン。)
	MV1SetRotationXYZ(modelId_, { 0.0f, DX_PI_F, 0.0f });

	MV1SetupCollInfo(modelId_);
}

void Stage::Update(void)
{
}

void Stage::Draw(void)
{
	// ロードされた３Ｄモデルを画面に描画
	MV1DrawModel(modelId_);
}

void Stage::Release(void)
{
	// ロードされた３Ｄモデルをメモリから解放
	MV1DeleteModel(modelId_);
}