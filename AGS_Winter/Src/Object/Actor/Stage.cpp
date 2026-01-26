#include <DxLib.h>
#include "../../Application.h"
#include "../../Utility/Utility.h"
#include "../../Utility/AngleUtility.h"
#include "../Collider/ColliderModel.h"
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
	transform_.SetModel(MV1LoadModel((Application::PATH_MODEL + "Stage.mv1").c_str()));
	opacityModelId_ = MV1DuplicateModel(transform_.modelId);
}

void Stage::InitTransform(void)
{
	transform_.pos = { 0.0f, -180.0f, 0.0f };
	transform_.rot = transform_.localRot = Utility::VECTOR_ZERO;
	transform_.scl = VScale(Utility::VECTOR_ONE, 2.0f);

	MATRIX mat = MGetIdent();
	mat = MMult(mat, AngleUtility::Multiplication(transform_.localRot, transform_.rot));
	mat = MMult(mat, MGetScale(transform_.scl));
	mat = MMult(mat, MGetTranslate(transform_.pos));

	MV1SetMatrix(opacityModelId_, mat);
	MV1SetOpacityRate(opacityModelId_, 0.5f);
}

void Stage::InitCollider(void)
{
	// モデルコライダ
	ColliderModel* colModel = new ColliderModel(&transform_);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::MODEL), colModel);
}

void Stage::Draw(void) const
{
	MV1DrawModel(opacityModelId_);
}

void Stage::SetOpacityIndex(std::vector<int> index)
{
	int num = opacityIndex.size();

	if (num > 0) {

		MV1SetFrameVisible(transform_.modelId, opacityIndex.at(num - 1), true);
		num--;
	}

	opacityIndex = index;
	num = opacityIndex.size();
	
	if (num > 0) {

		MV1SetFrameVisible(transform_.modelId, opacityIndex.at(num - 1), false);
		num--;
	}
}
