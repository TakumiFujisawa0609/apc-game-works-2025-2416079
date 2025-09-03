#pragma once
#include <vector>
#include <DxLib.h>


class AnimationController;

class EnemyBase
{
public:

	// エネミー種別
	enum class TYPE
	{
		BIRB,
		BEE,
		MAX,
	};

	// 状態
	enum class STATE
	{
		NONE,
		STANDBY,
		HIT_REACT,
		DEAD_REACT,
		END,
	};

	// アニメーション種別
	enum class ANIM_TYPE_A
	{
		DEATH,
		DUCK,
		HIT_REACT,
		IDLE,
		JUMP,
		JUMP_IDLE,
		JUMP_LAND,
		NO,
		PUNCH,
		RUN,
		WALK,
		WAVE,
		WEAPON,
		YES,
		MAX,
	};

	enum class ANIM_TYPE_B
	{
		DEATH,
		FLY_MOVE,
		FLY_IDLE,
		HEADBUTT,
		HIT_REACT,
		NO,
		PUNCH,
		YES,
		MAX,
	};

	// コンストラクタ
	EnemyBase(void);
	// デストラクタ
	virtual ~EnemyBase(void);

	// 初期処理
	void Init(TYPE type, int baseModelId);
	// 更新処理
	void Update(void);
	// 状態遷移
	void ChangeState(STATE state);
	// 描画処理
	void Draw(void);
	// 解放処理
	void Release(void);

	int GetModelId(void) { return modelId_; }

	// 座標取得
	VECTOR GetPos(void) { return pos_; }

	// 衝突判定用半径
	float GetCollisionRadius(void) { return collisionRadius_; }
	// 衝突判定用調整後座標取得
	VECTOR GetCollisionPos(void);

	// ダメージを与える
	void Damage(int damage);
	// 衝突判定が有効な状態
	bool IsCollisionState(void);
	bool IsDead(void);

protected:

	// アニメーションコントローラ
	AnimationController* animationController_;
	// 種別
	TYPE type_;
	// 状態
	STATE state_;

	// モデルのハンドルID
	int modelId_;

	// 座標
	VECTOR pos_;
	// 角度
	VECTOR angles_;
	// 大きさ
	VECTOR scales_;

	// 移動方向
	VECTOR moveDir_;
	// 移動速度
	float speed_;

	// HP
	int hp_;

	int deadCnt_;

	// パラメータ設定(純粋仮想関数)
	virtual void SetParam(void) = 0;

	// 状態遷移
	virtual void ChangeStandby(void) = 0;
	virtual void ChangeDeadReact(void);
	virtual void ChangeHitReact(void) = 0;
	virtual void ChangeEnd(void);

	// 状態別更新
	virtual void UpdateStandby(void);
	virtual void UpdateHitReact(void);
	virtual void UpdateDeadReact(void);
	virtual void UpdateEnd(void);

	// 状態別描画
	virtual void DrawStandby(void);
	virtual void DrawDeadReact(void);
	virtual void DrawHitReact(void);
	virtual void DrawEnd(void);

	// 衝突判定用半径
	float collisionRadius_;
	// 衝突判定用の球体中心座標の調整
	VECTOR collisionLocalPos_;

	// 移動
	virtual void Move(void) = 0;
};