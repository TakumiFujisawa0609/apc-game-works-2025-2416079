#pragma once
#include <DxLib.h>
#include <vector>
#include <map>
#include "../Collider/ColliderBase.h"
#include "../Common/Transform.h"


class AnimationController;

class ActorBase {
public:

	// 衝突判定種別
	enum class COLLIDER_TYPE
	{
		MODEL,
		LINE,
		CAPSULE,
		SPHERE,
		MAX,
	};

	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 10.0f, 0.0f };
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -50.0f, 0.0f };

	ActorBase();
	~ActorBase();

	//全初期化
	virtual void InitAll();
	//ロード以外
	virtual void Init();
	//ロード
	virtual void InitLoad() = 0;
	//アニメーションの初期化
	virtual void InitAnim() = 0;
	//モデルの初期化
	virtual void InitTransform() = 0;
	//コライダの初期化
	virtual void InitCollider() = 0;

	//更新
	virtual void Update() = 0;

	//モデルの描画
	virtual void DrawModel() const;
	//他の描画
	virtual void Draw() const = 0;

	//解放
	virtual void Release() const;

	// トランスフォームの取得
	Transform& GetTransform(void) { return transform_; }

	// 自身の衝突情報取得
	const std::map<int, ColliderBase*>& GetOwnColliders(void) const
	{
		return ownColliders_;
	}

protected:

	//アニメーションコントローラー
	AnimationController* animationCtrl_;

	//モデルの情報
	Transform transform_;

	// 自身の衝突情報
	std::map<int, ColliderBase*> ownColliders_;

	//移動方向
	VECTOR moveDir_;

	//体力
	int hp_;
};