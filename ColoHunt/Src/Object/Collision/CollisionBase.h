#pragma once
#include <DxLib.h>
#include <map>


class Transform;
class ColliderBase;
class ColliderCapsule;

class CollisionBase {

public:

	CollisionBase();
	~CollisionBase();

protected:

	const MV1_COLL_RESULT_POLY Collision(ColliderBase* colA, ColliderBase* colB);
	const MV1_COLL_RESULT_POLY_DIM CollisionDim(ColliderBase* colA, ColliderBase* colB);

	//押し戻し
	const VECTOR GetPosPushBackAlongNormal(const MV1_COLL_RESULT_POLY& hitColPoly, ColliderBase* capsule, int maxTryCnt, float pushDistance) const;

private:

	//モデルと線(当たった最初の場所のみ)
	const MV1_COLL_RESULT_POLY CollisionLine(ColliderBase* model, ColliderBase* line);
	//モデルと線(当たったすべての場所)
	const MV1_COLL_RESULT_POLY_DIM CollisionDimLine(ColliderBase* model, ColliderBase* line);
	//モデルとカプセル(コライダーのあるもの{相対位置})
	const MV1_COLL_RESULT_POLY_DIM CollisionCapsule(ColliderBase* model, ColliderBase* capsule);
	//モデルと球
	const MV1_COLL_RESULT_POLY_DIM CollisionSphere(ColliderBase* model, ColliderBase* sphere);
};