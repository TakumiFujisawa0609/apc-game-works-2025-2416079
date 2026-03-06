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

	//攻撃範囲
	static constexpr float ATTACK_RADIUS = 35.0f;

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

	//攻撃座標の取得
	VECTOR GetAttackStartPos(void) const;
	VECTOR GetAttackEndPos(void) const;

	//攻撃状態かどうか
	bool IsAttack(void) const;

	// どの状態か
	bool IsAttackA(void) const { return attackAFlg_; }
	bool IsAttackB(void) const { return attackBFlg_; }
	bool IsAttackC(void) const { return attackCFlg_; }
	bool IsMove(void) const { return state_ == STATE::MOVE; }

	//弾を消させる
	void DeleteShot(void);

	// ダメージを与える
	void Damage(int damage);

	// HPがゼロになったか
	bool ClearFlg(void) const { return clearFlg_; }

	// 死んだときのモーションをとるため
	AnimationController* GetEnemyAnim(void) { return animationCtrl_; }

private:

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
	static constexpr int BASE_ATTACK_DIFF = 65;

	//遠距離攻撃の頭の位置
	static constexpr VECTOR ATTACK_POS_A = { 0.0f, 250.0f, 325.0f };
	//遠距離攻撃の速さ
	static constexpr float ATTACK_SPEED = 28.0f;

	//体力
	static constexpr int MAX_HP = 500;

	//体のカプセルコライダーのサイズ
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 200.0f, 65.0f };
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 200.0f, -125.0f };
	static constexpr float COL_CAPSULE_RADIUS = 135.0f;

	//ゲームシーン
	Player* player_;
	// 状態
	STATE state_;
	ATTACK attack_;

	Transform shotTransform_;

	VECTOR armStartPos_;
	VECTOR headStartPos_;

	VECTOR armEndPos_;
	VECTOR headEndPos_;

	// 弾の衝突情報
	std::map<int, ColliderBase*> shotColliders_;

	// 角度
	VECTOR targetAngles_;

	//攻撃判定の中心
	VECTOR attackPrevPos_;
	VECTOR attackPos1_;
	VECTOR attackPos2_;
	VECTOR attackDir_;

	// HPがゼロになったらTrue
	bool clearFlg_;

	// 攻撃間隔
	int baseAttackDiff_;
	int attackDiff_;

	// 攻撃カウント
	float cnt_;
	// スタンカウント
	int downCnt_;

	// 攻撃状態の種類フラグ
	bool attackAFlg_;
	bool attackBFlg_;
	bool attackCFlg_;

	// 怒ったかどうか
	bool angryFlg_;

	// 足の速さ
	float speed_;
	// 弾の速さ
	float attackSpeed_;

	// エフェクト画像のハンドル
	int effectHandle_;

	// エフェクトを一回だけ出すためのフラグ
	bool first_;

	// プレイヤーの向きを出す
	void DirectionPlayer(void);
	// プレイヤーの向きを向く
	bool Turn(void);

	// 状態遷移
	void ChangeWait(void);
	void ChangeMove(void);
	void ChangeAttack(void);
	void ChangeDown(void);
	void ChangeKO(void);
	
	// 怒りによる変数変化
	void Anger(void);

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