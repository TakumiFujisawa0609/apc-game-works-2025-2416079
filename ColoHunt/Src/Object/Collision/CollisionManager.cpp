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

void CollisionManager::PushBack(ColliderBase* model, ColliderBase* capsule, ColliderBase* line, Transform* transform, const int tryCnt, const float dist)
{
	auto hit = CollisionBase::Collision(model, line);

	if (hit.HitFlag) {

		transform->pos.y = hit.HitPosition.y;
	}
	PushBack(model, capsule, transform, tryCnt, dist);
}

void CollisionManager::PushBack(ColliderBase* model, ColliderBase* capsule, Transform* transform, const int tryCnt, const float dist)
{
	// モデルとカプセルの衝突判定
	auto hits = CollisionBase::CollisionDim(model, capsule);

	// 衝突した複数のポリゴンと衝突回避するまで、位置を移動させる
	for (int i = 0; i < hits.HitNum; i++) {

		auto hitPoly = hits.Dim[i];

		// 指定された回数と距離で三角形の法線方向に押し戻す
		transform->pos = GetPosPushBackAlongNormal(hitPoly, capsule, tryCnt, dist);
	}
	// 検出した地面ポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(hits);
}

bool CollisionManager::IsHit(ColliderBase* colA, ColliderBase* colB)
{
	MV1_COLL_RESULT_POLY_DIM res = CollisionBase::CollisionDim(colA, colB);
	return res.HitNum > 0 ? true : false;
}

MV1_COLL_RESULT_POLY_DIM CollisionManager::Hit(ColliderBase* colA, ColliderBase* colB)
{
	return MV1_COLL_RESULT_POLY_DIM();	return CollisionBase::CollisionDim(colA, colB);
}

void CollisionManager::Release(void)
{
	inst_ = nullptr;
}
