#pragma once
#include <vector>
#include <DxLib.h>


class AnimationController;

class Player
{
public:

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
	enum class ANIM_TYPE
	{
		T,
		WALK,
		ATTACK,
		MAX,
	};

	static constexpr VECTOR DEFAULT_POS = { 0.0f, 0.0f, 0.0f };
	static constexpr VECTOR DIFF_ANGLES = { 0.0f, DX_PI_F, 0.0f };

	// コンストラクタ
	Player(void);
	// デストラクタ
	~Player(void);

	// 初期処理
	void Init();
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

	// 状態遷移
	void ChangeStandby(void);
	void ChangeDeadReact(void);
	void ChangeHitReact(void);
	void ChangeEnd(void);

	// 状態別更新
	void UpdateStandby(void);
	void UpdateHitReact(void);
	void UpdateDeadReact(void);
	void UpdateEnd(void);

	// 状態別描画
	void DrawStandby(void);
	void DrawDeadReact(void);
	void DrawHitReact(void);
	void DrawEnd(void);

	// 衝突判定用半径
	float collisionRadius_;
	// 衝突判定用の球体中心座標の調整
	VECTOR collisionLocalPos_;

	// 移動
	void Move(void);
	//攻撃
	void Attack(void);
};