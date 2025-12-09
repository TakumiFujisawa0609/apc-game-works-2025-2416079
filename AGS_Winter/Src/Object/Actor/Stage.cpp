#include <DxLib.h>
#include "../../Application.h"
#include "../../Utility/Utility.h"
#include "Stage.h"


Stage::Stage(void)
{
}

Stage::~Stage(void)
{
}

void Stage::InitLoad(void)
{
	// 外部ファイルの３Ｄモデルをロード
	modelId_ = MV1LoadModel((Application::PATH_MODEL + "Stage.mv1").c_str());
}

void Stage::InitOwn(void)
{
	pos_ = { 0.0f, -180.0f, 0.0f };
	angles_ = localAngles_ = Utility::VECTOR_ZERO;
	scales_ = VScale(Utility::VECTOR_ONE, 2.0f);
}
