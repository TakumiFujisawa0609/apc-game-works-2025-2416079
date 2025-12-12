#pragma once
#include <vector>
#include <DxLib.h>
#include "ActorBase.h"


class AnimationController;

class Player : public ActorBase
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
		DAMAGED_LIGHT,
		DAMAGED_HEAVY,
		KO,
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
		DAMAGED_LIGHT,
		DAMAGED_HEAVY,
		STAND_UP,
		KO_MOVE,
		KO,
		MAX,
	};

	static constexpr VECTOR DEFAULT_POS = { 0.0f, 0.0f, -500.0f };
	static constexpr VECTOR DIFF_ANGLES = { 0.0f, DX_PI_F, 0.0f };
	static constexpr VECTOR SWORD_POS = { 77.5f, 27.5f, -10.0f };
	static constexpr int MAX_HP = 1;
	static constexpr int HEAL_COUNT = 15;
	static constexpr float MAX_STAMINA = 1000.0f;
	static constexpr float DOGDE_STAMINA = 100.0f;
	static constexpr int STAMINA_MAX_TIME = 180 * 60;
	static constexpr int BASIC_DAMAGE = 10;
	static constexpr int MAX_POWER = 10;
	static constexpr int EFFECT_NUM = 9;

	// コンストラクタ
	Player(void);
	// デストラクタ
	~Player(void);

	// ロード
	void InitLoad() override;
	//アニメーションの初期化
	void InitAnim() override;
	//その他の初期化
	void InitOwn() override;
	// 更新処理
	void Update(void) override;
	// 状態遷移
	void ChangeState(STATE state);
	// 描画処理
	void Draw(void) const  override;
	// 解放処理
	void Release(void) const  override;

	VECTOR GetAngle(void) const { return angles_; }

	//攻撃座標の取得
	VECTOR GetAttackStartPos(void) const { return attackPos1_; }
	VECTOR GetAttackEndPos(void) const { return attackPos2_; }

	// ダメージを与える
	void Damage(int damage, float dir);
	void Heal(void) { isHeal_ = true; }
	void HealMax(void) { isHealMax_ = true; }
	void StaminaMax(void) { isStaminaMax_ = true; staminaMaxCnt_ = 0; }

	bool HealUsed(void) const { return isHeal_ || isHealMax_; }
	bool Healable(void) const;

	int GetPower(void) const { return damage_; }

	// 衝突判定が有効な状態
	bool IsAttackMotion(void) const;
	
	bool IsHit(void) const;
	bool IsAttack(void) const {return isAttack_;}
	
	bool IsDodge(void) const { return dodgeFlg_; }
	int DodgeCount(void) const { return dodgeCnt_; }

	void GreatDodge(void);
	void GoodDodge(void);

	bool SuccessDodge(void) const { return greatDodge_ || goodDodge_; }

	bool OverFlg(void) const { return overFlg_; }

	AnimationController* GetPlayerAnim(void) const { return animationCtrl_; }

	double GetBuff(void) const { return buff_; }
	void ResetBuff(void) { buff_ = 1.0; }

protected:

	// 状態
	STATE state_;

	// 移動速度
	float speed_;
	//攻撃判定の始点と終点
	VECTOR attackPos1_;
	VECTOR attackPos2_;

	float knockBackDir_;

	// HP
	bool isHeal_;
	bool isHealMax_;
	int healCount_;
	int autoHealHp_;
	int autoHealCnt_;

	//stamina
	bool isStaminaMax_;
	int staminaMaxCnt_;
	float stamina_;
	
	int hpBar_;
	float barSX_, barEX_;
	float barHpSY_, barHpEY_;
	float barStaSY_, barStaEY_;
	float barSize_;

	bool overFlg_;

	bool isAttack_;
	int power_;
	bool powerUp_;
	int powerUpCnt_;
	int damage_;
	double buff_;

	int powerGauge_;
	float guageSX_, guageSY_;
	std::vector<float> guageEX_;
	std::vector<float> guageSize_;

	int dodgeCnt_;
	bool dodgeFlg_;
	bool greatDodge_;
	bool goodDodge_;
	
	VECTOR dodgeTopPos_[EFFECT_NUM];
	VECTOR dodgeBottomPos_[EFFECT_NUM];
	VECTOR effectDir_[EFFECT_NUM];

	void Status(void);
	void KnockBack(void);

	void FindHpAndPower(void);
	void DrawHpAndPower(void) const;

	// 状態遷移
	void ChangeWait(void) const;
	void ChangeMove(void) const;
	void ChangeAttack(void);
	void ChangeCombo(void);
	void ChangeDodge(void);
	void ChangeDamagedLight(void) const;
	void ChangeDamagedHeavy(void) const;
	void ChangeKO(void) const;

	// 状態別更新
	void UpdateWait(void);
	void UpdateMove(void);
	void UpdateAttack(void);
	void UpdateCombo(void);
	void UpdateDodge(void);
	void UpdateDamagedLight(void);
	void UpdateDamagedHeavy(void);
	void UpdateKO(void);
};