#include <DxLib.h>
#include "../../Application.h"
#include "../../Utility/Utility.h"
#include "../../Utility/AngleUtility.h"
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
	opacityModelId_ = MV1DuplicateModel(modelId_);
}

void Stage::InitOwn(void)
{
	pos_ = { 0.0f, -180.0f, 0.0f };
	angles_ = localAngles_ = Utility::VECTOR_ZERO;
	scales_ = VScale(Utility::VECTOR_ONE, 2.0f);
}

void Stage::InitModel(void) const
{
	ActorBase::InitModel();

	//場所諸々の初期化
	MV1SetPosition(opacityModelId_, pos_);
	MV1SetRotationMatrix(opacityModelId_, AngleUtility::Multiplication(localAngles_, angles_));
	MV1SetScale(opacityModelId_, scales_);
}

void Stage::Draw(void) const
{
	int num = opacityIndex.size();

	while (num > 0) {

		MV1SetFrameOpacityRate(opacityModelId_, num - 1, 0.5f);

		num--;

		if (num == 0) {

			MV1DrawModel(opacityModelId_);
		}
	}
}

void Stage::SetOpacityIndex(std::vector<int> index)
{
	int num = opacityIndex.size();

	if (num > 0) {

		MV1SetFrameVisible(modelId_, opacityIndex.at(num - 1), true);
		num--;
	}

	opacityIndex = index;
	num = opacityIndex.size();
	
	if (num > 0) {

		MV1SetFrameVisible(modelId_, opacityIndex.at(num - 1), false);
		num--;
	}
}
