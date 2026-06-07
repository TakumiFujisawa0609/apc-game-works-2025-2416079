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

	// エフェクトのサイズ
	static constexpr float FIRE_SIZE = 25.0f;
	static constexpr float EFFECT_SIZE = 10.0f;

	// ストップディフ
	static constexpr float FAR_STOP_DIF = 275.0f;
	static constexpr float NEAR_STOP_DIF = 0.0002f;

	// 移動に変わる距離
	static constexpr float CHANGE_MOVE_DIF = 650.0f;

	// 回頭の補完サイズ
	static constexpr float LERP = 0.1f;

	//遠距離攻撃の頭の位置
	static constexpr VECTOR ATTACK_POS_A = { 0.0f, 250.0f, 325.0f };
	//遠距離攻撃の速さ
	static constexpr float ATTACK_SPEED = 28.0f;
	// 遠距離攻撃で狙うプレイヤーの位置
	static constexpr VECTOR SHOT_AIM = { 0.0f, 80.0f, 0.0f };

	// 近距離攻撃の停止タイミング
	static constexpr int STOP_TIMING = 50;
	// 近距離攻撃の開始タイミング
	static constexpr float START_TIMING_A = 33.1f;
	static constexpr float START_TIMING_B = 26;
	static constexpr float START_TIMING_C = 30;

	// もがく回数
	static constexpr float DOWN_NUM = 5;

	//体力
	static constexpr float MAX_HP = 500;

	//体のカプセルコライダーのサイズ
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 200.0f, 65.0f };
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 200.0f, -125.0f };
	static constexpr float COL_CAPSULE_RADIUS = 135.0f;
	static constexpr float COL_BODY_HEAD_RADIUS = 60.0f;
	static constexpr float COL_PARTS_RADIUS = 30.0f;

	//ゲームシーン
	Player* player_;
	// 状態
	STATE state_;
	ATTACK attack_;

	Transform shotTransform_;

	VECTOR headPosStart_;
	VECTOR headPosEnd_;

	VECTOR armPosRStart_;
	VECTOR armPosREnd_;

	VECTOR armPosLStart_;
	VECTOR armPosLEnd_;

	VECTOR legPosRStart_;
	VECTOR legPosREnd_;

	VECTOR legPosLStart_;
	VECTOR legPosLEnd_;

	VECTOR bodyPosStart_;
	VECTOR bodyPosEnd_;

	// 角度
	VECTOR targetAngles_;

	//攻撃の向き
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