#include <algorithm>
#include "CollisionBase.h"
#include "../../Utility/VectorUtility.h"
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
	if (colA->GetShape() == ColliderBase::SHAPE::CAPSULE && colB->GetShape() == ColliderBase::SHAPE::CAPSULE) {
		return CollisionCapsuleCapsule(colA, colB);
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

const MV1_COLL_RESULT_POLY_DIM CollisionBase::CollisionCapsuleCapsule(ColliderBase* capA, ColliderBase* capB)
{
	// 返り値用の初期化
	MV1_COLL_RESULT_POLY_DIM res{};
	res.HitNum = 1;

	// 一旦キャスト
	ColliderCapsule* collCapsuleA = dynamic_cast<ColliderCapsule*>(capA);
	ColliderCapsule* collCapsuleB = dynamic_cast<ColliderCapsule*>(capB);

	// 線分の 始点/終点 
	const VECTOR aStartPos = collCapsuleA->GetPosTop(), aEndPos = collCapsuleA->GetPosDown();
	const VECTOR bStartPos = collCapsuleB->GetPosTop(), bEndPos = collCapsuleB->GetPosDown();

	// 半径
	const float aRadius = collCapsuleA->GetRadius(), bRadius = collCapsuleB->GetRadius();

	// Aの方向ベクトル
	VECTOR u = VSub(aEndPos, aStartPos);
	// Bの方向ベクトル
	VECTOR v = VSub(bEndPos, bStartPos);

	// Bの始点からAの始点までのベクトル
	VECTOR w = VSub(aStartPos, bStartPos);

	float aLen = VectorUtility::SqrMagnitudeF(u);
	float bLen = VectorUtility::SqrMagnitudeF(v);
	float ab = VDot(u, v);
	float aw = VDot(u, w);
	float bw = VDot(v, w);

	float denom = aLen * bLen - ab * ab;
	float s, t;

	if (denom < 1e-6f) {
		// 線分がほぼ平行 → 片方に合わせて計算
		s = 0.0f;
		t = bw / bLen;
	}
	else {
		s = (ab * bw - bLen * aw) / denom;
		t = (aLen * bw - ab * aw) / denom;
	}

	// 線分内に clamp
	s = std::clamp(s, 0.0f, 1.0f);
	t = std::clamp(t, 0.0f, 1.0f);
	
	VECTOR us = VScale(u, s);
	VECTOR vt = VScale(v, t);

	VECTOR pa = VectorUtility::Add(aStartPos, us);  // A線分上の最近点
	VECTOR pb = VectorUtility::Add(bStartPos, vt);  // B線分上の最近点

	// 距離計算
	float distSq = VectorUtility::SqrDistance(pa, pb);
	float radSum = aRadius + bRadius;

	// 当たってない
	if (distSq >= radSum * radSum) { res.HitNum = 0; }

	// 当たった
	return res;
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
