#include <DxLib.h>
#include "../Application.h"
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
	modelId_ = MV1LoadModel((Application::PATH_MODEL + "Stage/Stage.mv1").c_str());
	backModelId_ = MV1LoadModel((Application::PATH_MODEL + "Stage/BackStage.mv1").c_str());
	backImg_ = LoadGraph("Data/Image/Sky.jpg");

	// ３Ｄモデルの大きさを設定(引数は、x, y, zの倍率)
	MV1SetScale(modelId_, { 1.0f, 1.0f, 1.0f });
	MV1SetScale(backModelId_, { 1.0f, 1.0f, 1.0f });

	// ３Ｄモデルの位置(引数は、３Ｄ座標)
	MV1SetPosition(modelId_, { 0.0f, 0.0f, 0.0f });
	MV1SetPosition(backModelId_, { 0.0f, 0.0f, 0.0f });

	// ３Ｄモデルの向き(引数は、x, y, zの回転量。単位はラジアン。)
	MV1SetRotationXYZ(modelId_, { 0.0f, DX_PI_F, 0.0f });
	MV1SetRotationXYZ(backModelId_, { 0.0f, 0.0f, 0.0f });

	MV1SetupCollInfo(modelId_);
}

void Stage::Update(void)
{
}

void Stage::Draw(void)
{
	// ロードされた３Ｄモデルを画面に描画
	DrawGraph(0, 0, backImg_, true);
	MV1DrawModel(modelId_);
	MV1DrawModel(backModelId_);
}

void Stage::Release(void)
{
	// ロードされた３Ｄモデルをメモリから解放
	MV1DeleteModel(modelId_);
	MV1DeleteModel(backModelId_);
}