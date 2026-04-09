#include "CollisionManager.h"
#include "../Common/Transform.h"
#include "../Collider/ColliderBase.h"


CollisionManager* CollisionManager::inst_ = nullptr;

CollisionManager::CollisionManager() : CollisionBase()
{
}

CollisionManager::~CollisionManager()
{
}

void CollisionManager::CreateInstance() {

	if (inst_ == nullptr) {

		inst_ = new CollisionManager();
	}
}

void CollisionManager::PushBack(std::map<int, ColliderBase*> model, std::map<int, ColliderBase*> capsule, Transform* transform, const int tryCnt, const float dist)
{
	auto hit = CollisionBase::CollisionLine(model, capsule);

	if (hit.HitFlag) {

		transform->pos.y = hit.HitPosition.y;
	}

	// モデルとカプセルの衝突判定
	auto hits = CollisionBase::CollisionCapsule(model, capsule);

	// 衝突した複数のポリゴンと衝突回避するまで、位置を移動させる
	for (int i = 0; i < hits.HitNum; i++) {

		auto hitPoly = hits.Dim[i];

		// 指定された回数と距離で三角形の法線方向に押し戻す
		transform->pos = GetPosPushBackAlongNormal(hitPoly, capsule, tryCnt, dist);
	}
	// 検出した地面ポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(hits);
}

bool CollisionManager::IsHitCapsule(std::map<int, ColliderBase*> model, std::map<int, ColliderBase*> capsule)
{
	MV1_COLL_RESULT_POLY_DIM res = CollisionBase::CollisionCapsule(model, capsule);
	return res.HitNum > 0 ? true : false;
}

bool CollisionManager::IsHitCapsule(std::map<int, ColliderBase*> model, VECTOR start, VECTOR end, float radius)
{
	MV1_COLL_RESULT_POLY_DIM res = CollisionBase::CollisionCapsule(model, start, end, radius);
	return res.HitNum > 0 ? true : false;
}

bool CollisionManager::IsHitSphere(std::map<int, ColliderBase*> model, std::map<int, ColliderBase*> sphere)
{
	MV1_COLL_RESULT_POLY_DIM res = CollisionBase::CollisionSphere(model, sphere);
	return res.HitNum > 0 ? true : false;
}

MV1_COLL_RESULT_POLY_DIM CollisionManager::HitCapsule(std::map<int, ColliderBase*> model, std::map<int, ColliderBase*> capsule)
{
	return CollisionBase::CollisionCapsule(model, capsule);
}

MV1_COLL_RESULT_POLY_DIM CollisionManager::HitCapsule(std::map<int, ColliderBase*> model, VECTOR start, VECTOR end, float radius)
{
	return 	CollisionBase::CollisionCapsule(model, start, end, radius);
}

void CollisionManager::Release(void)
{
	inst_ = nullptr;
}
