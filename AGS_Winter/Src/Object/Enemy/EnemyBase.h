#pragma once
#include <vector>
#include <DxLib.h>


class AnimationController;
class Player;

class EnemyBase
{
public:

	// 状態
	enum class STATE
	{
		WAIT,
		MOVE,
		ATTACK,
		DEAD_REACT,
		END,
	};

	// アニメーション種別
	enum class ANIM_TYPE
	{
		T,
		WAIT,
		WALK,
		MAX,
	};

	static constexpr VECTOR DEFAULT_POS = { 0.0f, 0.0f, 1000.0f };
	static constexpr VECTOR DIFF_ANGLES = { 0.0f, DX_PI_F, 0.0f };
	static constexpr VECTOR ATTACK_POS = { 0.0f, 450.0f, 450.0f };

	// コンストラクタ
	EnemyBase(Player* pl);
	// デストラクタ
	~EnemyBase(void);

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
	// 座標設定
	void SetPos(VECTOR pos) { pos_ = pos; }
	
	// 角度取得
	VECTOR GetAngle(void) { return angles_; }
	// 角度設定
	void SetAngle(VECTOR angles) { angles_ = angles; }
	
	//攻撃座標の取得
	VECTOR GetAttackPos(void) { return attackPos_; }
	VECTOR GetAttackStartPos(void) { return attackPos1_; }
	VECTOR GetAttackEndPos(void) { return attackPos2_; }

	bool IsAttackA(void) { return attackAFlg_; }
	bool IsAttackB(void) { return attackBFlg_; }

	// ダメージを与える
	void Damage(int damage) { hp_ -= damage; }

	bool ClearFlg(void) { return clearFlg_; }
	//// 衝突判定が有効な状態
	//bool IsCollisionState(void);
	//bool IsAttack(void);

protected:

	// アニメーションコントローラ
	AnimationController* animationController_;
	//ゲームシーン
	Player* player_;
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
	//攻撃判定の中心
	VECTOR attackPos_;
	VECTOR attackPos1_;
	VECTOR attackPos2_;
	VECTOR attackDir_;
	float attackSpeed_;

	// HP
	int hp_;

	bool clearFlg_;

	float cnt_;
	bool attackAFlg_;
	bool attackBFlg_;

	void LookPlayer(void);

	//// 状態遷移
	//void ChangeWait(void);
	//void ChangeMove(void);
	//void ChangeAttack(void);
	//void ChangeDodge(void);

	//// 状態別更新
	//void UpdateWait(void);
	//void UpdateMove(void);
	//void UpdateAttack(void);
	//void UpdateDodge(void);

	//// 状態別描画
	//void DrawStandby(void);
	//void DrawDeadReact(void);
	//void DrawHitReact(void);
	//void DrawEnd(void);
};