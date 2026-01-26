#pragma once
#include <vector>
#include <DxLib.h>
#include "ActorBase.h"


class Player;

class Enemy : public ActorBase
{
public:

	// 状態
	enum class STATE
	{
		WAIT,
		MOVE,
		ATTACK,
		KO,
		DOWN,
		END,
	};

	// アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE,
		WALK,
		RUN,
		ATTACK_A,
		ATTACK_B,
		ATTACK_C,
		DOWN,
		STRUGGLE,
		UP,
		MAX,
	};

	// 攻撃種類
	enum class ATTACK 
	{
		SHOT,
		ARM,
		HEAD
	};

	//初期値
	static constexpr VECTOR DEFAULT_POS = { 0.0f, 0.0f, 1000.0f };
	static constexpr VECTOR DIFF_ANGLES = { 0.0f, DX_PI_F, 0.0f };
	static constexpr VECTOR SCALE = { 6.0f, 6.0f, 6.0f };
	static constexpr float SPEED = 8.5f;

	//遠距離攻撃の頭の位置
	static constexpr VECTOR ATTACK_POS_A = { 0.0f, 250.0f, 325.0f };
	//遠距離攻撃の速さ
	static constexpr float ATTACK_SPEED = 28.0f;
	//攻撃範囲
	static constexpr float ATTACK_RADIUS = 35.0f;

	//体力
	static constexpr int MAX_HP = 400;

	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 250.0f, 300.0f };
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 250.0f, -250.0f };
	static constexpr float COL_CAPSULE_RADIUS = 225.0f;

	// コンストラクタ
	Enemy(Player* pl);
	// デストラクタ
	~Enemy(void);

	// ロード
	void InitLoad() override;
	//アニメーションの初期化
	void InitAnim() override;
	//その他の初期化
	void InitTransform() override;
	//コライダの初期化
	void InitCollider() override;

	// 更新処理
	void Update(void) override;
	// 状態遷移
	void ChangeState(STATE state);
	// 描画処理
	void Draw(void) const override;

	// 弾の衝突情報取得
	const std::map<int, ColliderBase*>& GetShotColliders(void) const
	{
		return shotColliders_;
	}

	// 腕の衝突情報取得
	const std::map<int, ColliderBase*>& GetArmColliders(void) const
	{
		return armColliders_;
	}

	// 頭の衝突情報取得
	const std::map<int, ColliderBase*>& GetHeadColliders(void) const
	{
		return headColliders_;
	}

	//攻撃座標の取得
	VECTOR GetAttackStartPos(void) const { return attackPos1_; }
	VECTOR GetAttackEndPos(void) const { return attackPos2_; }
	VECTOR GetAttackPrevPos(void) const { return attackPrevPos_; }

	bool IsAttackA(void) const { return attackAFlg_; }
	bool IsAttackB(void) const { return attackBFlg_; }
	bool IsAttackC(void) const { return attackCFlg_; }
	bool IsMove(void) const { return state_ == STATE::MOVE; }

	void DeleteAttackA(void) { attackAFlg_ = attackShowFlg_ = false; }

	bool IsAttack(void) const;

	// ダメージを与える
	void Damage(int damage);

	bool ClearFlg(void) const { return clearFlg_; }

	AnimationController* GetEnemyAnim(void) { return animationCtrl_; }
	//// 衝突判定が有効な状態
	//bool IsCollisionState(void);
	//bool IsAttackA(void);

private:

	//ゲームシーン
	Player* player_;
	// 状態
	STATE state_;
	ATTACK attack_;

	Transform shotTransform_;
	Transform armTransform_;
	Transform headTransform_;

	VECTOR armPos_;
	VECTOR headPos_;

	// 弾の衝突情報
	std::map<int, ColliderBase*> shotColliders_;

	// 腕の衝突情報
	std::map<int, ColliderBase*> armColliders_;

	// 頭の衝突情報
	std::map<int, ColliderBase*> headColliders_;

	// 角度
	VECTOR targetAngles_;

	//攻撃判定の中心
	VECTOR attackPrevPos_;
	VECTOR attackPos1_;
	VECTOR attackPos2_;
	VECTOR attackDir_;

	bool clearFlg_;

	int attackDiff_;
	float cnt_;
	int downCnt_;

	int coolDown_;
	bool isCoolDown_;

	bool attackAFlg_;
	bool attackBFlg_;
	bool attackCFlg_;

	bool attackShowFlg_;

	void DirectionPlayer(void);
	bool Turn(void);

	// 状態遷移
	void ChangeWait(void);
	void ChangeMove(void);
	void ChangeAttack(void);
	void ChangeDown(void);
	void ChangeKO(void);

	// 状態別更新
	void UpdateWait(void);
	void UpdateMove(void);
	void UpdateAttack(void);
	void UpdateAttackA(void);
	void UpdateAttackB(void);
	void UpdateAttackC(void);
	void UpdateDown(void);
	void UpdateKO(void);
};