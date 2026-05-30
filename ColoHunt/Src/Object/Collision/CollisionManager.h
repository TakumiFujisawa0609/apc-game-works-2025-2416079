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
	void PushBack(ColliderBase* model, ColliderBase* capsule, ColliderBase* line, Transform* transform, const int tryCnt, const float dist);
	void PushBack(ColliderBase* model, ColliderBase* capsule, Transform* transform, const int tryCnt, const float dist);

	//当たったかどうかだけの処理
	bool IsHit(ColliderBase* colA, ColliderBase* colB);

	//情報が欲しい人用に
	MV1_COLL_RESULT_POLY_DIM Hit(ColliderBase* colA, ColliderBase* colB);

	static CollisionManager& GetInstance(void) { return *inst_; }
	static void CreateInstance(void);

private:

	//シングルトン用のインスタンス
	static CollisionManager* inst_;
};