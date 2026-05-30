#include "CollisionBase.h"
#include "../Actor/ActorBase.h"
#include "../Common/Transform.h"
#include "../Collider/ColliderBase.h"
#include "../Collider/ColliderModel.h"
#include "../Collider/ColliderLine.h"
#include "../Collider/ColliderCapsule.h"
#include "../Collider/ColliderSphere.h"


CollisionBase::CollisionBase()
{
}

CollisionBase::~CollisionBase()
{
}

const MV1_COLL_RESULT_POLY CollisionBase::Collision(ColliderBase* colA, ColliderBase* colB)
{
	if (colA->GetShape() == ColliderBase::SHAPE::MODEL && colB->GetShape() == ColliderBase::SHAPE::LINE){
		return CollisionLine(colA, colB);
	}
	if (colA->GetShape() == ColliderBase::SHAPE::LINE && colB->GetShape() == ColliderBase::SHAPE::MODEL) {
		return CollisionLine(colB, colA);
	}
	return MV1_COLL_RESULT_POLY();
}

const MV1_COLL_RESULT_POLY_DIM CollisionBase::CollisionDim(ColliderBase* colA, ColliderBase* colB)
{
	if (colA->GetShape() == ColliderBase::SHAPE::MODEL && colB->GetShape() == ColliderBase::SHAPE::LINE) {
		return CollisionDimLine(colA, colB);
	}
	if (colA->GetShape() == ColliderBase::SHAPE::LINE && colB->GetShape() == ColliderBase::SHAPE::MODEL) {
		return CollisionDimLine(colB, colA);
	}
	if (colA->GetShape() == ColliderBase::SHAPE::MODEL && colB->GetShape() == ColliderBase::SHAPE::CAPSULE) {
		return CollisionCapsule(colA, colB);
	}
	if (colA->GetShape() == ColliderBase::SHAPE::CAPSULE && colB->GetShape() == ColliderBase::SHAPE::MODEL) {
		return CollisionCapsule (colB, colA);
	}
	if (colA->GetShape() == ColliderBase::SHAPE::MODEL && colB->GetShape() == ColliderBase::SHAPE::SPHERE) {
		return CollisionSphere(colA, colB);
	}
	if (colA->GetShape() == ColliderBase::SHAPE::SPHERE && colB->GetShape() == ColliderBase::SHAPE::MODEL) {
		return CollisionSphere(colB, colA);
	}
	return MV1_COLL_RESULT_POLY_DIM();
}

const MV1_COLL_RESULT_POLY CollisionBase::CollisionLine(ColliderBase* model, ColliderBase* line)
{
	ColliderModel* collModel = dynamic_cast<ColliderModel*>(model);
	ColliderLine* collLine = dynamic_cast<ColliderLine*>(line);

	if (collModel == nullptr || collLine == nullptr) return MV1_COLL_RESULT_POLY();

	return MV1CollCheck_Line(collModel->GetFollow()->modelId, -1, collLine->GetPosStart(), collLine->GetPosEnd());
}

const MV1_COLL_RESULT_POLY_DIM CollisionBase::CollisionDimLine(ColliderBase* model, ColliderBase* line)
{
	ColliderModel* collModel = dynamic_cast<ColliderModel*>(model);
	ColliderLine* collLine = dynamic_cast<ColliderLine*>(line);

	if (collModel == nullptr || collLine == nullptr) return MV1_COLL_RESULT_POLY_DIM();

	return MV1CollCheck_LineDim(collModel->GetFollow()->modelId, -1, collLine->GetPosStart(), collLine->GetPosEnd());
}

const MV1_COLL_RESULT_POLY_DIM CollisionBase::CollisionCapsule(ColliderBase* model, ColliderBase* capsule)
{
	ColliderModel* collModel = dynamic_cast<ColliderModel*>(model);
	ColliderCapsule* collCapsule = dynamic_cast<ColliderCapsule*>(capsule);

	if (collModel == nullptr || collCapsule == nullptr) return MV1_COLL_RESULT_POLY_DIM();

	return MV1CollCheck_Capsule(collModel->GetFollow()->modelId, -1, collCapsule->GetPosTop(), collCapsule->GetPosDown(), collCapsule->GetRadius());
}

const MV1_COLL_RESULT_POLY_DIM CollisionBase::CollisionSphere(ColliderBase* model, ColliderBase* sphere)
{
	ColliderModel* collModel = dynamic_cast<ColliderModel*>(model);
	ColliderSphere* collSphere = dynamic_cast<ColliderSphere*>(sphere);

	if (collModel == nullptr || collSphere == nullptr) return MV1_COLL_RESULT_POLY_DIM();

	return MV1CollCheck_Sphere(collModel->GetFollow()->modelId, -1, collSphere->GetPos(), collSphere->GetRadius());
}

const VECTOR CollisionBase::GetPosPushBackAlongNormal(const MV1_COLL_RESULT_POLY& hitColPoly, ColliderBase* capsule, int maxTryCnt, float pushDistance) const
{
	// コピー生成
	ColliderCapsule tmpCapsule = dynamic_cast<ColliderCapsule&>(*capsule);
	Transform tmpTransform = *tmpCapsule.GetFollow();

	// 衝突補正処理
	int tryCnt = 0;

	while (tryCnt < maxTryCnt) {

		// カプセルと三角形の当たり判定
		if (!HitCheck_Capsule_Triangle(tmpCapsule.GetPosTop(), tmpCapsule.GetPosDown(), tmpCapsule.GetRadius(),
			hitColPoly.Position[0], hitColPoly.Position[1], hitColPoly.Position[2])) {

			break;
		}

		// 衝突していたら法線方向に押し戻し
		tmpTransform.pos = VAdd(tmpTransform.pos, VScale(hitColPoly.Normal, pushDistance));
		tryCnt++;
	}
	return tmpTransform.pos;
}
