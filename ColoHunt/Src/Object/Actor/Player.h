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

	//剣のサイズ
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

	//攻撃力を取得
	int GetPower(void) const { return damage_; }
	//SEを鳴らさないようにする
	void NoSe(void) { se_ = false; }

	// 衝突判定が有効な状態
	bool IsAttackMotion(void) const;

	//当たり判定があるかどうか
	bool IsHit(void) const;
	//攻撃しているかどうか
	bool IsAttack(void) const { return isAttack_; }

	//剣の座標の取得
	VECTOR GetSwordPosSta(void) const { return swordPosSta_; }
	VECTOR GetSwordPosEnd(void) const { return swordPosEnd_; }

	//回避判定があるかどうか
	bool IsDodge(void) const { return dodgeFlg_; }
	//回避フレーム数
	int DodgeCount(void) const { return dodgeCnt_; }

	//ドンピタ回避
	void GreatDodge(void);
	//いい回避
	void GoodDodge(void);
	//一応回避
	void Dodge(void) { dodge_ = true; }

	//回避成功しているかどうか
	bool SuccessDodge(void) const { return greatDodge_ || goodDodge_ || dodge_; }

	//死んだかどうか
	bool OverFlg(void) const { return overFlg_; }

	//プレイヤーのアニメーション
	AnimationController* GetPlayerAnim(void) const { return animationCtrl_; }

	//攻撃のバフ
	double GetBuff(void) const { return buff_; }
	//バフのリセット
	void ResetBuff(void) { buff_ = 1.0; }

protected:

private:

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

	//初期位置
	static constexpr VECTOR DEFAULT_POS = { 0.0f, 38.0f, -500.0f };
	static constexpr VECTOR DIFF_ANGLES = { 0.0f, DX_PI_F, 0.0f };

	//マックスHP
	static constexpr int MAX_HP = 85;
	//回復するフレーム数
	static constexpr int HEAL_COUNT = 20;

	//マックススタミナ
	static constexpr float MAX_STAMINA = 1000.0f;
	//回避で消費するスタミナ
	static constexpr float DODGE_STAMINA = 200.0f;
	//スタミナ無消費状態の秒数
	static constexpr int STAMINA_MAX_TIME = 30 * 60;

	//基礎速度
	static constexpr int SPEED = 7;
	//基礎攻撃力
	static constexpr int BASIC_DAMAGE = 10;
	//パワーアップするまでに必要なゲージ量
	static constexpr int MAX_POWER = 6;

	//エフェクトの数
	static constexpr int EFFECT_NUM = 9;
	//エフェクトのマックスサイズ
	static constexpr float EFFECT_MAX_SIZE = 150.0f;

	//カプセルコライダーのサイズ
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 100.0f, 0.0f };
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 45.0f, 0.0f };
	static constexpr float COL_CAPSULE_RADIUS = 30.0f;

	//剣の相対位置
	static constexpr VECTOR SWORD_POS = { 80.5f, 30.5f, -13.5f };

	//アイテムのポインター
	Item* item_;

	// 状態
	STATE state_;

	//剣の座標
	VECTOR swordPosSta_;
	VECTOR swordPosEnd_;

	// 移動速度
	float speed_;

	//ノックバックする向き
	float knockBackDir_;

	//回復
	bool isHeal_;
	//全回復
	bool isHealMax_;
	//回復しているフレーム数
	int healCount_;
	
	//自動回復するHP量
	int autoHealHp_;
	//自動回復するフレーム
	int autoHealCnt_;
	//くらったダメージ量
	int damaged_;

	//スタミナ無消費かどうか
	bool isStaminaMax_;
	//スタミナ無消費のフレーム
	int staminaMaxCnt_;
	//スタミナ量
	float stamina_;
	
	//HPバーの画像ハンドル
	int hpBar_;
	//バーの最初・最後の位置
	float barSX_, barEX_;

	//HPバーの最初・最後の高さ
	float barHpSY_, barHpEY_;
	//スタミナバーの最初・最後の高さ
	float barStaSY_, barStaEY_;
	//バーの長さ
	float barSize_;

	//死んだかどうか
	bool overFlg_;

	//攻撃状態
	bool isAttack_;

	//現在のパワーゲージ
	int power_;
	//パワーアップしているかどうか
	bool powerUp_;
	//パワーアップしているフレーム数
	int powerUpCnt_;

	//攻撃力
	int damage_;
	//攻撃のバフ(倍率)
	double buff_;

	//パワーアップゲージの画像ハンドル
	int powerGauge_;
	//ゲージの最初の位置高さ
	int guageSX_, guageSY_;
	//ゲージの曲線の最後の位置(数が高さ)
	std::vector<int> guageEX_;
	//ゲージの長さ
	std::vector<int> guageSize_;

	//回避フレーム数
	int dodgeCnt_;

	//回避できるかどうか
	bool dodgeFlg_;

	//回避しているかどうか
	bool greatDodge_;
	bool goodDodge_;
	bool dodge_;

	//回避時消費するスタミナ量
	float dodgeStamina_;
	
	//エフェクトの位置情報
	VECTOR effectTopPos_[EFFECT_NUM];
	VECTOR effectBottomPos_[EFFECT_NUM];
	VECTOR effectDir_[EFFECT_NUM];

	//エフェクトがでているフレーム数
	int effectCnt_;
	//エフェクトの現在のサイズ
	float effectSize_;
	//エフェクトの種類
	EFFECT effectType_;

	//SEを鳴らしても良いか
	bool se_;

	//状態管理
	void Status(void);
	//回復
	void Heal(void);
	//ノックバック
	void KnockBack(void);

	//バーのサイズ判定
	void FindHpAndPower(void);
	//バーの描画
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