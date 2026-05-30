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
}

void Stage::InitTransform(void)
{
	transform_.pos = { 0.0f, -180.0f, 0.0f };
	transform_.rot = transform_.localRot = Utility::VECTOR_ZERO;
	transform_.scl = VScale(Utility::VECTOR_ONE, 2.0f);
}

void Stage::InitCollider(void)
{
	// モデルコライダ
	ColliderModel* colModel = new ColliderModel(&transform_);
	ownColliders_.emplace(COLLIDER_TAG::MODEL, colModel);
}

void Stage::Draw(void) const
{
	//裏側が見えるためバックカリングを有効に
	SetUseBackCulling(false);
	//透明モデルをフレームごとに描画
	for (int i = 0; i < MV1GetFrameNum(transform_.modelId); i++) {
		if (opacityIndex.size() == 0) {
		
			break;
		}
		else {
			for (int j : opacityIndex) {
				if (i == j) {

					MV1DrawFrame(transform_.modelId, i);
					break;
				}
			}
		}
	}
	SetUseBackCulling(true);
}

void Stage::DrawModel(void) const
{
	//不透明モデルをフレームごとに描画
	for (int i = 0; i < MV1GetFrameNum(transform_.modelId); i++) {
		if (opacityIndex.size() == 0) {

			MV1DrawFrame(transform_.modelId, i);
		}
		else {
			bool opaFlg = false;
			for (int j : opacityIndex) {
				if (i == j) {

					opaFlg = true;
					break;
				}
			}
			if (opaFlg == false) {

				MV1DrawFrame(transform_.modelId, i);
			}
		}
	}
}

void Stage::SetOpacityIndex(std::vector<int> index)
{
	int num = (int)opacityIndex.size();

	//透明フレームを不透明に
	if (num > 0) {

		MV1SetFrameOpacityRate(transform_.modelId, opacityIndex.at(num - 1), 1.0f);
		num--;
	}

	opacityIndex = index;
	num = (int)opacityIndex.size();

	//不透明フレームを透明に
	if (num > 0) {
		
		MV1SetFrameOpacityRate(transform_.modelId, opacityIndex.at(num - 1), 0.5f);
		num--;
	}
}
