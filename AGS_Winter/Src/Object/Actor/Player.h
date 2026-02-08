#pragma once
#include <vector>
#include <DxLib.h>
#include "ActorBase.h"


class AnimationController;
class Item;

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
		DRINK,
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
		KO,
		DRINK,
		FINISH,
		MAX,
	};

	// エフェクト
	enum class EFFECT
	{
		NON = -1,
		GREAT_DODGE,
		GOOD_DODGE,
		HEAL,
		STAMINA,
		MAX,
	};

	static constexpr VECTOR DEFAULT_POS = { 0.0f, 38.0f, -500.0f };
	static constexpr VECTOR DIFF_ANGLES = { 0.0f, DX_PI_F, 0.0f };

	static constexpr int MAX_HP = 85;
	
	static constexpr int HEAL_COUNT = 15;
	static constexpr float MAX_STAMINA = 1000.0f;
	static constexpr float DODGE_STAMINA = 200.0f;
	static constexpr int STAMINA_MAX_TIME = 30 * 60;
	
	static constexpr int BASIC_DAMAGE = 10;
	static constexpr int MAX_POWER = 6;
	
	static constexpr int EFFECT_NUM = 9;
	static constexpr float EFFECT_MAX_SIZE = 150.0f;

	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 100.0f, 0.0f };
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 45.0f, 0.0f };
	static constexpr float COL_CAPSULE_RADIUS = 30.0f;

	static constexpr VECTOR SWORD_POS = { 77.5f, 27.5f, -10.0f };
	static constexpr float SWORD_RADIUS = 10.0f;

	// コンストラクタ
	Player(Item* itm);
	// デストラクタ
	~Player(void);

	// ロード
	void InitLoad() override;
	//アニメーションの初期化
	void InitAnim() override;
	//モデルの初期化
	void InitTransform() override;
	// コライダの初期化
	void InitCollider() override;
	// 更新処理
	void Update(void) override;
	// 描画処理
	void Draw(void) const  override;
	// 解放処理
	void Release(void) const  override;

	// 状態遷移
	void DoChangeState(STATE state);

	// ダメージを与える
	void Damage(int damage, float dir);

	int GetPower(void) const { return damage_; }
	void NoSe(void) { se_ = false; }

	// 衝突判定が有効な状態
	bool IsAttackMotion(void) const;
	
	bool IsHit(void) const;
	bool IsAttack(void) const {return isAttack_;}

	// 自身の衝突情報取得
	const std::map<int, ColliderBase*>& GetSwordColliders(void) const
	{
		return swordColliders_;
	}

	bool IsDodge(void) const { return dodgeFlg_; }
	int DodgeCount(void) const { return dodgeCnt_; }

	void GreatDodge(void);
	void GoodDodge(void);
	void Dodge(void) { dodge_ = true; }

	bool SuccessDodge(void) const { return greatDodge_ || goodDodge_ || dodge_; }

	bool OverFlg(void) const { return overFlg_; }

	AnimationController* GetPlayerAnim(void) const { return animationCtrl_; }

	double GetBuff(void) const { return buff_; }
	void ResetBuff(void) { buff_ = 1.0; }

protected:

private:

	Item* item_;

	// 状態
	STATE state_;

	Transform swordTransform_;

	// 剣の衝突情報
	std::map<int, ColliderBase*> swordColliders_;

	// 移動速度
	float speed_;

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
	bool dodge_;
	int dodgeStamina_;
	
	VECTOR dodgeTopPos_[EFFECT_NUM];
	VECTOR dodgeBottomPos_[EFFECT_NUM];
	VECTOR effectDir_[EFFECT_NUM];

	int effectCnt_;
	float effectSize_;
	EFFECT effectType_;

	bool se_;

	void Status(void);
	void Heal(void);
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
	void ChangeDrink(void) const;

	// 状態別更新
	void UpdateWait(void);
	void UpdateMove(void);
	void UpdateAttack(void);
	void UpdateCombo(void);
	void UpdateDodge(void);
	void UpdateDamagedLight(void);
	void UpdateDamagedHeavy(void);
	void UpdateKO(void);
	void UpdateDrink(void);

	// 状態遷移の判断
	void BoolChangeMove(void);
	void BoolChangeAttack(void);
	void BoolChangeCombo(void);
	void BoolChangeDodge(void);
	void BoolChangeDrink(void);

	// 更新処理
	void StopSE(void);
	void EffectCreate(void);
	void EffectUpdate(void);
};