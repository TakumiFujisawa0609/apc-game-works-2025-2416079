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
		WAIT,
		MOVE,
		ATTACK,
		COMBO,
		DOGDE,
		END,
	};

	// アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE,
		WALK,
		RUN,
		ATTACK,
		COMBO_1,
		COMBO_2,
		COMBO_3,
		DODGE,
		MAX,
	};

	static constexpr VECTOR DEFAULT_POS = { 0.0f, 0.0f, -500.0f };
	static constexpr VECTOR DIFF_ANGLES = { 0.0f, DX_PI_F, 0.0f };
	static constexpr VECTOR SWORD_POS = { 75.0f, 30.0f, -10.0f };
	static constexpr int MAX_HP = 10;

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

	//モデルの取得
	int GetModelId(void) { return modelId_; }

	// 座標取得
	VECTOR GetPos(void) { return pos_; }
	//攻撃座標の取得
	VECTOR GetAttackStartPos(void) { return attackPos1_; }
	VECTOR GetAttackEndPos(void) { return attackPos2_; }

	// ダメージを与える
	void Damage(int damage);

	int GetPower(void) { return power_; }
	void SetPower(int pow) { power_ = pow; }

	// 衝突判定が有効な状態
	bool IsCollisionState(void);
	bool IsAttackMotion(void);
	bool IsAttack(void);
	bool IsDodge(void) { return dodgeFlg_; }

	int DodgeCount(void) { return dodgeCnt_; }

	bool OverFlg(void) { return overFlg_; }

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
	//攻撃判定の始点と終点
	VECTOR attackPos1_;
	VECTOR attackPos2_;

	// HP
	int hp_;
	bool overFlg_;

	bool isAttack_;
	int power_;

	int dodgeCnt_;
	bool dodgeFlg_;

	// 状態遷移
	void ChangeWait(void);
	void ChangeMove(void);
	void ChangeAttack(void);
	void ChangeCombo(void);
	void ChangeDodge(void);

	// 状態別更新
	void UpdateWait(void);
	void UpdateMove(void);
	void UpdateAttack(void);
	void UpdateCombo(void);
	void UpdateDodge(void);

	// 状態別描画
	void DrawStandby(void);
	void DrawDeadReact(void);
	void DrawHitReact(void);
	void DrawEnd(void);
};