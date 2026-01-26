#pragma once
#include <DxLib.h>
#include <map>
#include "CollisionBase.h"


class CollisionManager : public CollisionBase {
public:

	CollisionManager();		//コンストラクタ
	~CollisionManager();		//デストラクタ

	void Release(void);

	//押し戻し処理
	void PushBack(std::map<int, ColliderBase*> model, std::map<int, ColliderBase*> capsule, Transform* transform, const float tryCnt, const float dist);

	//当たったかどうかだけの処理
	bool IsHitCapsule(std::map<int, ColliderBase*> model, std::map<int, ColliderBase*> capsule);
	bool IsHitSphere(std::map<int, ColliderBase*> model, std::map<int, ColliderBase*> sphere);

	//情報が欲しい人用に
	MV1_COLL_RESULT_POLY_DIM HitCapsule(std::map<int, ColliderBase*> model, std::map<int, ColliderBase*> capsule);

	static CollisionManager& GetInstance(void) { return *inst_; }
	static void CreateInstance(void);

private:

	//シングルトン用のインスタンス
	static CollisionManager* inst_;
};