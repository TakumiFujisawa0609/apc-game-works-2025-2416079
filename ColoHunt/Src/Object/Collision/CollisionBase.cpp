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

const MV1_COLL_RESULT_POLY CollisionBase::CollisionLine(const std::map<int, ColliderBase*> model, const std::map<int, ColliderBase*> line)
{
	ColliderModel* collModel = SearchType<ColliderModel>(model, ActorBase::COLLIDER_TYPE::MODEL);
	ColliderLine* collLine = SearchType<ColliderLine>(line, ActorBase::COLLIDER_TYPE::LINE);
	
	if (collModel == nullptr || collLine == nullptr) return MV1_COLL_RESULT_POLY();

	return MV1CollCheck_Line(collModel->GetFollow()->modelId, -1, collLine->GetPosStart(), collLine->GetPosEnd());
}

const MV1_COLL_RESULT_POLY_DIM CollisionBase::CollisionPolyLine(const std::map<int, ColliderBase*> model, const std::map<int, ColliderBase*> line)
{
	ColliderModel* collModel = SearchType<ColliderModel>(model, ActorBase::COLLIDER_TYPE::MODEL);
	ColliderLine* collLine = SearchType<ColliderLine>(line, ActorBase::COLLIDER_TYPE::LINE);

	if (collModel == nullptr || collLine == nullptr) return MV1_COLL_RESULT_POLY_DIM();

	return MV1CollCheck_LineDim(collModel->GetFollow()->modelId, -1, collLine->GetPosStart(), collLine->GetPosEnd());
}

const MV1_COLL_RESULT_POLY_DIM CollisionBase::CollisionCapsule(const std::map<int, ColliderBase*> model, const std::map<int, ColliderBase*> capsule)
{
	ColliderModel* collModel = SearchType<ColliderModel>(model, ActorBase::COLLIDER_TYPE::MODEL);
	ColliderCapsule* collCapsule = SearchType<ColliderCapsule>(capsule, ActorBase::COLLIDER_TYPE::CAPSULE);

	if (collModel == nullptr || collCapsule == nullptr) return MV1_COLL_RESULT_POLY_DIM();

	return MV1CollCheck_Capsule(collModel->GetFollow()->modelId, -1, collCapsule->GetPosTop(), collCapsule->GetPosDown(), collCapsule->GetRadius());
}

const MV1_COLL_RESULT_POLY_DIM CollisionBase::CollisionCapsule(const std::map<int, ColliderBase*> model, const VECTOR start, const VECTOR end, const float radius)
{
	ColliderModel* collModel = SearchType<ColliderModel>(model, ActorBase::COLLIDER_TYPE::MODEL);
	
	if (collModel == nullptr) return MV1_COLL_RESULT_POLY_DIM();

	return MV1CollCheck_Capsule(collModel->GetFollow()->modelId, -1, start, end, radius);
}

const MV1_COLL_RESULT_POLY_DIM CollisionBase::CollisionSphere(const std::map<int, ColliderBase*> model, const std::map<int, ColliderBase*> sphere)
{
	ColliderModel* collModel = SearchType<ColliderModel>(model, ActorBase::COLLIDER_TYPE::MODEL);
	ColliderSphere* collSphere = SearchType<ColliderSphere>(sphere, ActorBase::COLLIDER_TYPE::SPHERE);

	if (collModel == nullptr || collSphere == nullptr) return MV1_COLL_RESULT_POLY_DIM();

	return MV1CollCheck_Sphere(collModel->GetFollow()->modelId, -1, collSphere->GetPos(), collSphere->GetRadius());
}

const VECTOR CollisionBase::GetPosPushBackAlongNormal(const MV1_COLL_RESULT_POLY& hitColPoly, std::map<int, ColliderBase*> capsule,  int maxTryCnt, float pushDistance) const
{
	// コピー生成
	ColliderCapsule tmpCapsule = *SearchType<ColliderCapsule>(capsule, ActorBase::COLLIDER_TYPE::CAPSULE);
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
