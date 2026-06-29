#include "Player.h"
#include "../../Utility/Utility.h"
#include "../../Utility/VectorUtility.h"
#include "../../Utility/AngleUtility.h"
#include "../../Manager/Audio/AudioManager.h"
#include "../../Manager/Audio/SoundTable.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Camera.h"
#include "../../Manager/Input/InputManager.h"
#include "../Common/AnimationController.h"
#include "../Collider/ColliderCapsule.h"
#include "../Collider/ColliderLine.h"
#include "../Collider/ColliderModel.h"
#include "../Item.h"


Player::Player(Item* itm): ActorBase(), item_(itm), autoHealCnt_(0), autoHealHp_(0), speed_(),
effectSize_(), effectCnt_(), dodge_(), damaged_(), swordPosEnd_(),
	dodgeCnt_(), dodgeFlg_(), healCount_(0), isAttack_(false), isHealMax_(), isHeal_(false), isStaminaMax_(false),
	knockBackDir_(0.0f), swordPosStast_(), overFlg_(false), power_(0), staminaMaxCnt_(), stamina_(MAX_STAMINA), state_(STATE::WAIT),
	effectDir_(),se_(true), dodgeStamina_(DODGE_STAMINA),
	barEX_(), barHpEY_(), barHpSY_(), barSize_(), barSX_(), barStaminaEY_(), barStaminaSY_(),
	damage_(BASIC_DAMAGE), goodDodge_(), greatDodge_(), guageEX_(), guageSize_(), guageSX_(), guageSY_(),hpBar_(),
	powerGauge_(), powerUp_(), powerUpCnt_(), effectBottomPos_(), effectTopPos_(), buff_(1.0), effectType_(EFFECT::NON),
	StateChange(), StateUpdate()
{
}

Player::~Player(void)
{
}

void Player::InitLoad()
{
	//モデルのロード
	transform_.SetModel(MV1LoadModel((Application::PATH_MODEL + PLAYER_NAME).c_str()));
	powerGauge_ = LoadSoftImage((Application::PATH_IMAGE + POWER_GAUGE_NAME).c_str());
	hpBar_ = LoadSoftImage((Application::PATH_IMAGE + HP_BAR_NAME).c_str());
}

void Player::InitAnim()
{
	//アニメーションのロード
	animationCtrl_ = new AnimationController(transform_.modelId);

	for (const ANIMATION_FBX& init : ANIM_FXB) {

		animationCtrl_->AddInFbx(init.num, init.speed, init.fbx);
	}

	for (const ANIMATION_INIT& init : ANIM_INIT) {

		animationCtrl_->Add(init.num, init.speed, Application::PATH_ANIMATION + init.path);
	}
}

void Player::InitTransform()
{
	FindHpAndPower();

	transform_.pos = DEFAULT_POS;
	transform_.prevPos = transform_.pos;
	transform_.rot = Utility::VECTOR_ZERO;
	transform_.localRot = VScale(Utility::AXIS_Y, DX_PI_F);
	transform_.scl = Utility::VECTOR_ONE;

	speed_ = SPEED;
	hp_ = MAX_HP;
	moveDir_ = Utility::DIR_F;

#pragma region 関数ポインタのセットアップ
	StateUpdate[static_cast<int>(STATE::WAIT)] = &Player::UpdateWait;
	StateUpdate[static_cast<int>(STATE::MOVE)] = &Player::UpdateMove;
	StateUpdate[static_cast<int>(STATE::ATTACK)] = &Player::UpdateAttack;
	StateUpdate[static_cast<int>(STATE::COMBO)] = &Player::UpdateCombo;
	StateUpdate[static_cast<int>(STATE::DOGDE)] = &Player::UpdateDodge;
	StateUpdate[static_cast<int>(STATE::DAMAGED_LIGHT)] = &Player::UpdateDamagedLight;
	StateUpdate[static_cast<int>(STATE::DAMAGED_HEAVY)] = &Player::UpdateDamagedHeavy;
	StateUpdate[static_cast<int>(STATE::KO)] = &Player::UpdateKO;
	StateUpdate[static_cast<int>(STATE::DRINK)] = &Player::UpdateDrink;

	StateChange[static_cast<int>(STATE::WAIT)] = &Player::ChangeWait;
	StateChange[static_cast<int>(STATE::MOVE)] = &Player::ChangeMove;
	StateChange[static_cast<int>(STATE::ATTACK)] = &Player::ChangeAttack;
	StateChange[static_cast<int>(STATE::COMBO)] = &Player::ChangeCombo;
	StateChange[static_cast<int>(STATE::DOGDE)] = &Player::ChangeDodge;
	StateChange[static_cast<int>(STATE::DAMAGED_LIGHT)] = &Player::ChangeDamagedLight;
	StateChange[static_cast<int>(STATE::DAMAGED_HEAVY)] = &Player::ChangeDamagedHeavy;
	StateChange[static_cast<int>(STATE::KO)] = &Player::ChangeKO;
	StateChange[static_cast<int>(STATE::DRINK)] = &Player::ChangeDrink;
#pragma endregion

	ChangeState(STATE::WAIT);
}

void Player::InitCollider()
{
	// モデルコライダ
	ColliderModel* colModel = new ColliderModel(&transform_);
	ownColliders_.emplace(COLLIDER_TAG::MODEL, colModel);
	
	// 主に地面との衝突で仕様する線分コライダ
	ColliderLine* colLine = new ColliderLine(&transform_, COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
	ownColliders_.emplace(COLLIDER_TAG::LINE, colLine);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule = new ColliderCapsule(&transform_, COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS, COL_CAPSULE_RADIUS);
	ownColliders_.emplace(COLLIDER_TAG::CAPSULE, colCapsule);
	
	// 剣用のフレームセット
	SetFrame();

	// 剣用のカプセルコライダー
	colCapsule = new ColliderCapsule(swordPosStast_, swordPosEnd_, SWORD_RADIUS);
	ownColliders_.emplace(COLLIDER_TAG::SWORD, colCapsule);
}

void Player::Update(void)
{
	transform_.prevPos = transform_.pos;
	// 剣用のカプセルコライダー
	SetFrame();

	//状態別更新処理
	(this->*StateUpdate[static_cast<int>(state_)])();

	Status();
	EffectUpdate();

	//モデルの設定
	transform_.Update();
	//アニメーションの移動
	animationCtrl_->Update();
	//剣の移動
	swordPosStast_ = MV1GetFramePosition(transform_.modelId, 58);
	swordPosEnd_ = VTransform(SWORD_POS, MV1GetFrameLocalWorldMatrix(transform_.modelId, 37));
	//当たり判定の更新
	MV1RefreshCollInfo(transform_.modelId);
}

void Player::ChangeState(STATE state)
{
	state_ = state;

	//歩く走るの音を止める
	StopSE();

	// 状態別変化処理
	(this->*StateChange[static_cast<int>(state_)])();
}

void Player::Draw(void) const
{
	//エフェクトの描画
	if (effectCnt_ >= 0) {
		
		SetUseLighting(false);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 250);

		for (int i = 0; i < EFFECT_NUM; i++) {
			switch (effectType_)
			{
			case Player::EFFECT::GREAT_DODGE:

				DrawCone3D(effectTopPos_[i], effectBottomPos_[i], EFFECT_RADIUS, EFFECT_DIV_NUM, GREAT_EFFECT_COLOR, GREAT_EFFECT_COLOR, true);
				break;

			case Player::EFFECT::GOOD_DODGE:

				DrawCone3D(effectTopPos_[i], effectBottomPos_[i], EFFECT_RADIUS, EFFECT_DIV_NUM, GOOD_EFFECT_COLOR, GOOD_EFFECT_COLOR, true);
				break;

			case Player::EFFECT::HEAL:

				DrawCone3D(effectTopPos_[i], effectBottomPos_[i], EFFECT_RADIUS, EFFECT_DIV_NUM, HEAL_EFFECT_DIF_COLOR, HEAL_EFFECT_SPC_COLOR, true);
				break;

			case Player::EFFECT::STAMINA:

				DrawCone3D(effectTopPos_[i], effectBottomPos_[i], EFFECT_RADIUS, EFFECT_DIV_NUM, STAMINA_EFFECT_DIF_COLOR, STAMINA_EFFECT_SPC_COLOR, true);
				break;
			}
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		SetUseLighting(true);
	}
	//ステータスの描画
	DrawHpAndPower();
}

void Player::Release(void) const
{
	//モデルの開放
	ActorBase::Release();

	//画像の開放
	DeleteSoftImage(hpBar_);
	DeleteSoftImage(powerGauge_);
}

int Player::Damage(int damage, float dir)
{
	//状態の更新
	damaged_ = damage;
	autoHealHp_ = damage / 3;
	knockBackDir_ = dir;
	greatDodge_ = goodDodge_ = dodge_ = false;
	isAttack_ = false;
	buff_ = 1.0;

	AudioManager::GetInstance()->PlaySE(SoundID::SE_DAMAGE);

	//回復を止める
	if (isHealMax_) {

		isHealMax_ = false;
	}
	if (isHeal_) {

		isHeal_ = false;
	}

	// HPがゼロなら死ぬ
	if (hp_ - damaged_ <= 0) {

		autoHealHp_ = 0;
		// 瞬間の画面のイメージ保存
		SceneManager::GetInstance().SetScreenImage();
		ChangeState(STATE::KO);
		return 0;
	}

	// パワーアップしていないならモーションをとる
	if (!powerUp_) {
		// 被ダメ量によってアクションを変える
		if (damage > LIGHT_ACTION_DAMAGE) {

			ChangeState(STATE::DAMAGED_HEAVY);
			return SHAKE_TIME_HEAVY;
		}
		else {

			ChangeState(STATE::DAMAGED_LIGHT);
			return SHAKE_TIME_LIGHT;
		}
	}
	return 0;
}

bool Player::IsAttackMotion(void) const
{
	if (state_ == STATE::ATTACK || state_ == STATE::COMBO) {

		return true;
	}
	else {

		return false;
	}
}

bool Player::IsHit(void) const
{
	if (state_ != STATE::DAMAGED_LIGHT && state_ != STATE::DAMAGED_HEAVY) {

		return true;
	}
	else {

		return false;
	}
}

void Player::GreatDodge(void)
{
	greatDodge_ = true;

	//エフェクトの生成
	EffectCreate();
	effectType_ = EFFECT::GREAT_DODGE;

	if (!powerUp_) {

		power_ += POWER_UP;
	}
	AudioManager::GetInstance()->PlaySE(SoundID::SE_DODGE);
}

void Player::GoodDodge(void)
{
	goodDodge_ = true;

	//エフェクトの生成
	EffectCreate();
	effectType_ = EFFECT::GOOD_DODGE;

	if (!powerUp_) {

		power_++;
	}
	AudioManager::GetInstance()->PlaySE(SoundID::SE_DODGE);
}

void Player::Status(void)
{
	//スタミナ回復の条件
	if ((state_ != STATE::DOGDE && (!InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::RUN).keyNew || state_ != STATE::MOVE)) || isStaminaMax_) {
		if (stamina_ < MAX_STAMINA) {

			stamina_++;
		}
	}
	//マイナスにならないように
	if (stamina_ < 0.0f) {

		stamina_ = 0.0f;
	}
	//一定数回復する
	if (isHeal_) {
		
		Heal();

		if (healCount_ >= HEAL_COUNT) {

				isHeal_ = false;
				healCount_ = 0;
		}
	}
	//全回復
	if (isHealMax_) {

		Heal();
	}
	//スタミナ無消費
	if (isStaminaMax_) {
		if (staminaMaxCnt_ > STAMINA_MAX_TIME) {

			staminaMaxCnt_ = 0;
			dodgeStamina_ = DODGE_STAMINA;
			isStaminaMax_ = false;
		}
		else {

			staminaMaxCnt_++;
		}
	}
	//自動回復
	if (autoHealHp_ > 0) {

		autoHealCnt_++;

		if (autoHealCnt_ >= AUTO_HEAL) {

			autoHealCnt_ = 0;
			autoHealHp_--;
			hp_++;
		}
	}
	//ゲージマックス時
	if (powerUp_) {

		powerUpCnt_++;

		//ゲージが減る速度
		if (powerUpCnt_ >= Application::FPS * 3) {
		
			powerUpCnt_ = 0;
			power_--;
		}
		if (power_ <= 0) {

			power_ = 0;
			damage_ = BASIC_DAMAGE;
			powerUp_ = false;
		}
	}
	//ゲージマックスでない時
	else {
		// ゲージマックスになったとき
		if (power_ >= MAX_POWER) {

			// ゲージを固定してバフ
			power_ = MAX_POWER;
			damage_ = (int)(BASIC_DAMAGE * 1.5);
			dodgeStamina_ = DODGE_STAMINA / 2.0f;
			powerUp_ = true;
		}
	}
	//シームレスにHPが減る
	if (damaged_ > 0) {

		hp_--;
		damaged_--;

		if (hp_ <= 0) {

			hp_ = 0;
		}
	}
}

void Player::Heal(void)
{
	// 赤いHPまで自動回復
	if (hp_ < MAX_HP) {
		if (autoHealHp_ > 0) {

			autoHealHp_--;
		}
		hp_++;
		healCount_++;
	}
	else {

		hp_ = MAX_HP;
		isHeal_ = false;
		isHealMax_ = false;
		healCount_ = 0;
	}
}

void Player::KnockBack()
{
	transform_.pos.x += sinf(knockBackDir_) * 4.0f;
	transform_.pos.z += cosf(knockBackDir_) * 4.0f;

	transform_.rot.y = knockBackDir_ - DX_PI_F;
}

void Player::FindHpAndPower(void)
{
	int dx, dy;
	int r, g, b, a;
	GetSoftImageSize(hpBar_, &dx, &dy);
	bool first = false;

	for (float x = 0; x <= static_cast<float>(dx); x++) {
		for (float y = 0; y < static_cast<float>(dy / 2); y++) {

			GetPixelSoftImage(hpBar_, static_cast<int>(x), static_cast<int>(y), &r, &g, &b, &a);

			//元画像の真っ青のサイズを探す
			if (r ==0 && g == 0 && b == 255 && a > 0) {
				if (!first) {
					
					//バーの最小XとYを出す
					first = true;
					barSX_ = x;
					barHpSY_ = y;
				}
				//サイズ更新したら書き換える
				if (barEX_ < x) {

					barEX_ = x;
				}
				if (barHpEY_ < y) {

					barHpEY_ = y;
				}
			}
		}
	}
	first = false;

	for (float x = 0; x <= static_cast<float>(dx); x++) {
		for (float y = static_cast<float>(dy / 2); y <= static_cast<float>(dy); y++) {

			GetPixelSoftImage(hpBar_, static_cast<int>(x), static_cast<int>(y), &r, &g, &b, &a);

			//元画像の真っ青のサイズを探す
			if (r == 0 && g == 0 && b == 255 && a > 0) {
				if (!first) {

					//HPとXは同じところなので
					//スタミナはYのみ
					first = true;
					barStaminaSY_ = y;
				}
				if (barStaminaEY_ < y) {

					barStaminaEY_ = y;
				}
			}
		}
	}
	barSize_ = barEX_ - barSX_;

	//真っ青の部分を黒に染める
	for (int y = static_cast<int>(barHpSY_); y <= static_cast<int>(barHpEY_); y++) {

		DrawLineSoftImage(hpBar_, static_cast<int>(barSX_), y, static_cast<int>(barEX_) + 1, y, 0, 0, 0, 255);
	}
	for (int y = static_cast<int>(barStaminaSY_); y <= static_cast<int>(barStaminaEY_); y++) {

		DrawLineSoftImage(hpBar_, static_cast<int>(barSX_), y, static_cast<int>(barEX_) + 1, y, 0, 0, 0, 255);
	}
	first = false;

	GetSoftImageSize(powerGauge_, &dx, &dy);

	int gx = 0;

	for (int y = 0; y < dy; y++) {
		for (int x = 0; x <= dx; x++) {

			GetPixelSoftImage(powerGauge_, x, y, &r, &g, &b, &a);

			//真っ青な部分を探す
			if (r == 0 && g == 0 && b == 255 && a > 0) {
				if (!first) {

					first = true;
					guageSX_ = x;
					guageSY_ = y;
				}
				gx = x;
			}
		}
		//斜めの部分をそれぞれ出す
		if (gx > 0) {
			guageEX_.push_back(gx + 1);
			gx = 0;
		}
	}
}

void Player::DrawHpAndPower(void) const
{
	//バー全体をHPの最大値分する
	float barRate = static_cast<float>(barSize_ / MAX_HP);
	//現在のHP分かける
	float barNorm = barRate * hp_;
	//赤いゲージを出す分かける
	float barRed = barRate * autoHealHp_;

	DrawSoftImage(static_cast<int>(BAR_POS), 0, hpBar_);

	//死んでないなら赤ゲージをだす
	if (hp_ - damaged_ > 0) {
	
		DrawBoxAA(BAR_POS + barSX_, barHpSY_, BAR_POS + barSX_ + barRate * (hp_ - damaged_) + barRed, barHpEY_, 0xff0000, true);
	}
	DrawBoxAA(BAR_POS + barSX_, barHpSY_, BAR_POS + barSX_ + barNorm, barHpEY_, 0x00ff00, true);

	//バー全体をスタミナの最大値分する
	barRate = barSize_ / static_cast<int>(MAX_STAMINA);
	//現在のスタミナ分かける
	barNorm = barRate * stamina_;

	// スタミナがマックス固定状態なら点滅させる
	if (isStaminaMax_) {

		DrawBoxAA(BAR_POS + barSX_, barStaminaSY_, BAR_POS + barSX_ + barNorm, barStaminaEY_, GetColor(255, 255, (int)(std::abs(staminaMaxCnt_ % 101 - 50) * 5.1f)), true);
	}
	else {
		if (stamina_ <= DODGE_STAMINA) {

			DrawBoxAA(BAR_POS + barSX_, barStaminaSY_, BAR_POS + barSX_ + barNorm, barStaminaEY_, 0xff0000, true);
		}
		else {
			DrawBoxAA(BAR_POS + barSX_, barStaminaSY_, BAR_POS + barSX_ + barNorm, barStaminaEY_, 0xffff00, true);
		}
	}

	//一番長いサイズをゲージのマックス値分する
	float powerRate = (static_cast<float>(guageEX_.front()) - static_cast<float>(guageSX_)) / static_cast<float>(MAX_POWER);
	float power = static_cast<float>(guageSX_) + powerRate * static_cast<float>(power_);

	// 横軸で埋める
	for (int y = 0; y < guageEX_.size(); y++) {

		int dy = y + guageSY_;
		DrawLineSoftImage(powerGauge_, guageSX_, dy, guageEX_.at(y), dy, 100, 100, 100, 255);

		if (!powerUp_) {
			if (power <= static_cast<float>(guageEX_.at(y))) {

				DrawLineSoftImage(powerGauge_, guageSX_, dy, static_cast<int>(power), dy, 255, 0, 0, 255);
			}
			else {

				DrawLineSoftImage(powerGauge_, guageSX_, dy, guageEX_.at(y), dy, 255, 0, 0, 255);
			}
		}
		else {
			int color = abs(powerUpCnt_ % 30 - 15) * 17;
			if (power <= static_cast<float>(guageEX_.at(y))) {

				DrawLineSoftImage(powerGauge_, guageSX_, dy, static_cast<int>(power), dy, 255, color, color, 255);
			}
			else {

				DrawLineSoftImage(powerGauge_, guageSX_, dy, guageEX_.at(y), dy, 255, color, color, 255);
			}
		}
	}
	DrawSoftImage(240, 100, powerGauge_);
}

void Player::ChangeWait(void)
{
	//待機モーション
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
}

void Player::ChangeMove(void)
{
}

void Player::ChangeAttack(void)
{
	if (!powerUp_) {
		//攻撃モーション
		animationCtrl_->Play(static_cast<int>(ANIM_TYPE::ATTACK), false);
	}
	else {
		//攻撃モーション
		animationCtrl_->Play(static_cast<int>(ANIM_TYPE::FINISH), false);
	}
}

void Player::ChangeCombo(void)
{
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::COMBO_1), false);
}

void Player::ChangeDodge(void)
{
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::DODGE), false);
	dodgeCnt_ = 0;
	dodgeFlg_ = true;
	if (!isStaminaMax_) {
	
		stamina_ -= DODGE_STAMINA;
	}
}

void Player::ChangeDamagedLight(void)
{
	// アニメーションを再生
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::DAMAGED_LIGHT), false);
}

void Player::ChangeDamagedHeavy(void)
{
	// アニメーションを再生
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::DAMAGED_HEAVY), false);
}

void Player::ChangeKO(void)
{
	// アニメーションを再生
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::KO), false);
}

void Player::ChangeDrink(void)
{
	// アニメーションを再生
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::DRINK), false);
	// 選択中のアイテムを使う
	item_->Use();
}

void Player::UpdateWait(void)
{	
	//状態遷移の判断
	BoolChangeAttack();
	BoolChangeCombo();
	BoolChangeMove();
	BoolChangeDodge();
	BoolChangeDrink();
}

void Player::UpdateMove(void)
{
	//行列と方向の設定
	MATRIX mat = MGetIdent();
	VECTOR dir = Utility::VECTOR_ZERO;

	//前後左右の移動処理
	//方向の取得
	dir = InputManager::GetInstance().GetDirectionXZAKeyL().at(InputManager::GetInstance().GetMostPriority());
	
	if (!VectorUtility::EqualsVZero(dir)) {

		//カメラの角度を得る
		VECTOR angle = Camera::GetInstance()->GetCameraAngles();

		//回転行列を取る
		mat = MMult(mat, MGetRotY(angle.y));

		//進む向きに回転させる
		dir = VNorm(dir);
		moveDir_ = VTransform(dir, mat);

		if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::RUN).keyNew) {
			if (stamina_ > 0.0f) {

				//移動モーション
				animationCtrl_->Play(static_cast<int>(ANIM_TYPE::RUN), true);
				
				// SEを出してもよいなら
				if (se_) {
					// 歩いている音を止め
					if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_WALK)) {

						AudioManager::GetInstance()->StopSE(SoundID::SE_WALK);
					}
					// 走っている音を出す
					if (!AudioManager::GetInstance()->IsPlaySE(SoundID::SE_RUN)) {

						AudioManager::GetInstance()->PlaySE(SoundID::SE_RUN);
					}
				}

				//移動させる
				transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_ * 1.75f));

				if (!isStaminaMax_) {
					if (!powerUp_) {
					
						// 通常時は二倍早くスタミナを減らす
						stamina_ -= 2;
					}
					else {

						// 強化時はスタミナの減少を緩やかに
						stamina_--;
					}
				}
			}
			else {

				//移動モーション
				animationCtrl_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
				
				//移動させる
				transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_));
			}
		}
		else {

			//移動モーション
			animationCtrl_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
			
			// SEを鳴らしてよいなら
			if (se_) {
				// 走っている音を止め
				if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_RUN)) {

					AudioManager::GetInstance()->StopSE(SoundID::SE_RUN);
				}
				// 歩いている音を出す
				if (!AudioManager::GetInstance()->IsPlaySE(SoundID::SE_WALK)) {

					AudioManager::GetInstance()->PlaySE(SoundID::SE_WALK);
				}
			}
			//移動させる
			transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_));
		}

		// 状態遷移の判断
		BoolChangeAttack();
		BoolChangeCombo();
		BoolChangeDodge();
		BoolChangeDrink();

		transform_.rot.y = atan2f(moveDir_.x, moveDir_.z);
	}
	else {

		ChangeState(STATE::WAIT);
	}
}

void Player::UpdateAttack(void)
{
	//パワーアップしていないときの連続攻撃
	if (!powerUp_) {
		//モーション時間に合わせて攻撃を発生させる
		if (animationCtrl_->GetTime() >= ATTACK_START_TIMING_1_FRONT && animationCtrl_->GetTime() <= ATTACK_START_TIMING_1_BACK) {

			isAttack_ = true;
		}
		if (animationCtrl_->GetTime() >= ATTACK_END_TIMING_1_FRONT && animationCtrl_->GetTime() <= ATTACK_END_TIMING_1_BACK) {

			isAttack_ = false;
		}
		if (animationCtrl_->GetTime() >= ATTACK_START_TIMING_2_FRONT && animationCtrl_->GetTime() <= ATTACK_START_TIMING_2_BACK) {

			isAttack_ = true;
		}
		if (animationCtrl_->GetTime() >= ATTACK_END_TIMING_2_FRONT && animationCtrl_->GetTime() <= ATTACK_END_TIMING_2_BACK) {

			isAttack_ = false;
		}
		if (animationCtrl_->GetTime() >= ATTACK_START_TIMING_3_FRONT && animationCtrl_->GetTime() <= ATTACK_START_TIMING_3_BACK) {

			// 最終段だけ火力をあげる
			buff_ = 1.2;
			isAttack_ = true;
		}
		if (animationCtrl_->GetTime() >= ATTACK_END_TIMING_3) {

			isAttack_ = false;
		}
		if (animationCtrl_->GetTime() <= ATTACK_START_TIMING_3_FRONT) {

			//移動させる
			transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_ * 0.2f));
		}
	}
	//パワーアップ時のゲージ全消費攻撃
	else {
		if (animationCtrl_->GetTime() >= FULL_START_TIMING_FRONT && animationCtrl_->GetTime() <= FULL_START_TIMING_BACK) {

			transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_ * 0.8f));
			// 攻撃力二倍で渡す
			buff_ = 2.0;
			isAttack_ = true;
		}
		if (animationCtrl_->GetTime() >= FULL_END_TIMING) {

			isAttack_ = false;
		}
		if (animationCtrl_->IsEnd()) {
		
			power_ = 0;
		}
	}

	if (animationCtrl_->IsEnd()) {

		// バフを戻す
		buff_ = 1.0;
		isAttack_ = false;
		//待機モーションに移行
		ChangeState(STATE::WAIT);
	}
}

void Player::UpdateCombo(void)
{
	if (animationCtrl_->GetPlayType() == static_cast<int>(ANIM_TYPE::COMBO_1)) {
		//モーション時間に合わせて攻撃を出す
		if (animationCtrl_->GetTime() >= COMMBO_1_START_TIMING_FRONT && animationCtrl_->GetTime() <= COMMBO_1_START_TIMING_BACK) {

			isAttack_ = true;
		}
		//攻撃判定の消失
		if (animationCtrl_->GetTime() >= COMMBO_1_END_TIMING) {
			
			isAttack_ = false;
			//回避キャンセル
			BoolChangeDodge();

			//同じボタンを押したら二段階目
			if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::COMBO).keyTrgDown) {

				animationCtrl_->Play(static_cast<int>(ANIM_TYPE::COMBO_2), false);
			}
		}
		//攻撃判定がなくなるまで前進させる
		else {

			transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_ * 0.3f));
		}
	}
	if (animationCtrl_->GetPlayType() == static_cast<int>(ANIM_TYPE::COMBO_2)) {
		//モーション時間に合わせて攻撃を出す
		if (animationCtrl_->GetTime() >= COMMBO_2_START_TIMING_FRONT && animationCtrl_->GetTime() <= COMMBO_2_START_TIMING_BACK) {

			isAttack_ = true;

		}
		//攻撃判定の消失
		if (animationCtrl_->GetTime() >= COMMBO_2_END_TIMING) {
			
			isAttack_ = false;
			//回避キャンセル
			BoolChangeDodge();
			
			//同じボタンを押したら三段階目
			if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::COMBO).keyTrgDown) {

				animationCtrl_->Play(static_cast<int>(ANIM_TYPE::COMBO_3), false);
			}
		}
		//攻撃判定がなくなるまで前進させる
		else {

			transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_ * 0.2f));
		}
	}
	if (animationCtrl_->GetPlayType() == static_cast<int>(ANIM_TYPE::COMBO_3)) {
		//モーション時間に合わせて攻撃を出す
		//最終段なので少しだけ攻撃力上昇
		if (animationCtrl_->GetTime() >= COMMBO_3_START_TIMING_FRONT && animationCtrl_->GetTime() <= COMMBO_3_START_TIMING_BACK) {

			// 最終段だけ火力をあげる
			buff_ = 1.1;
			isAttack_ = true;
		}
		//攻撃判定の消失
		if (animationCtrl_->GetTime() >= COMMBO_3_END_TIMING) {
			
			// バフを戻す
			buff_ = 1.0;
			isAttack_ = false;
			//回避キャンセル
			BoolChangeDodge();
		}
		//攻撃判定がなくなるまで前進させる
		else {

			transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_ * 0.4f));
		}
	}
	if (animationCtrl_->IsEnd()) {

		isAttack_ = false;
		//待機モーションに移行
		ChangeState(STATE::WAIT);
	}
}

void Player::UpdateDodge(void)
{
	//移動させる
	transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_ * 1.25f));

	if (dodgeFlg_) {
		// 回避判定の持続
		if (dodgeCnt_ <= DODGE_MOVE) {

			dodgeCnt_++;
		}
		else {

			dodgeFlg_ = false;
			dodgeCnt_ = 0;
		}
	}

	if (animationCtrl_->IsEnd()) {

		//待機モーションに移行
		ChangeState(STATE::WAIT);

		greatDodge_ = goodDodge_ = dodge_ = false;
	}
}

void Player::UpdateDamagedLight(void)
{
	if (animationCtrl_->GetTime() >= CHANGE_STATE_TIMING_LIGHT) {

		ChangeState(STATE::WAIT);
	}
}

void Player::UpdateDamagedHeavy(void)
{
	static int steps = 0;
	static bool prevPause = false;

	if (animationCtrl_->GetPlayType() == static_cast<int>(ANIM_TYPE::DAMAGED_HEAVY)) {
		if (animationCtrl_->GetTime() <= NOCKBACK_COUNT) {

			KnockBack();
		}
		if (!prevPause){
			//吹っ飛んだあとの起き上がりで
			//アニメーションを止める
			if (animationCtrl_->GetTime() >= NOCKBACK_STOP_TIMING) {
				
				prevPause = true;
				animationCtrl_->ChangePause(true);
			}
		}
		if (animationCtrl_->IsPause()) {

			steps++;

			//何かキーが押されるか一定時間経つまで起き上がらない
			if (InputManager::GetInstance().GetPriorityAnyoneTrg() || steps >= NOCKBACK_UP_TIMING) {

				steps = 0;
				animationCtrl_->ChangePause(false);
			}
		}
		if (animationCtrl_-> GetTime() >= WAKE_UP_TIMING) {

			prevPause = false;
			animationCtrl_->Play(static_cast<int>(ANIM_TYPE::STAND_UP), false);
		}
	}
	if (animationCtrl_->GetPlayType() == static_cast<int>(ANIM_TYPE::STAND_UP)) {
		//起き上がりきるまで無敵
		if (animationCtrl_->GetTime() >= CHANGE_STATE_TIMING_HEAVY) {

			ChangeState(STATE::WAIT);
		}
	}
}

void Player::UpdateKO(void)
{
	overFlg_ = true;
}

void Player::UpdateDrink(void)
{
	//飲むモーションが終わるまで回復せず数も減らない
	if (!animationCtrl_->IsEnd()) return;

	AudioManager::GetInstance()->PlaySE(SoundID::SE_HEAL);
	switch (item_->GetUseType())
	{
	case Item::TYPE::HP:

		isHeal_ = true;
		effectType_ = EFFECT::HEAL;
		
		break;

	case Item::TYPE::HP_MAX:
	
		isHealMax_ = true;
		effectType_ = EFFECT::HEAL;
		break;

	case Item::TYPE::STAMINA:

		isStaminaMax_ = true;
		effectType_ = EFFECT::STAMINA;
		break;
	}
	//エフェクト生成
	EffectCreate();
	ChangeState(STATE::WAIT);
}

void Player::BoolChangeMove(void)
{
	if (!VectorUtility::EqualsVZero(InputManager::GetInstance().GetDirectionXZAKeyL().at(InputManager::GetInstance().GetMostPriority()))) {

		//移動モーションに移行
		ChangeState(STATE::MOVE);
	}
}

void Player::BoolChangeAttack(void)
{
	if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::ATTACK).keyTrgDown) {

		//攻撃モーションに移行
		ChangeState(STATE::ATTACK);
	}
}

void Player::BoolChangeCombo(void)
{
	if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::COMBO).keyTrgDown) {

		//攻撃モーションに移行
		ChangeState(STATE::COMBO);
	}
}

void Player::BoolChangeDodge(void)
{
	if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::DODGE).keyTrgDown) {
		if (stamina_ >= dodgeStamina_ || isStaminaMax_) {

			//回避モーションに移行
			ChangeState(STATE::DOGDE);
		}
	}
}

void Player::BoolChangeDrink(void)
{
	if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::USE).keyTrgDown) {
		if (!isHeal_ && !isHealMax_ && damaged_ <= 0) {

			//飲むモーションに移行
			ChangeState(STATE::DRINK);
		}
	}
}

void Player::StopSE(void)
{
	// 走る、歩くのSEを消す
	if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_RUN)) {

		AudioManager::GetInstance()->StopSE(SoundID::SE_RUN);
	}
	if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_WALK)) {
				
		AudioManager::GetInstance()->StopSE(SoundID::SE_WALK);
	}
}

void Player::EffectCreate(void)
{
	effectSize_ = EFFECT_START_SIZE;

	//エフェクトの数まで四方八方に生成する
	//(頂点シェーダを使いたかった)
	for (int i = 0; i < EFFECT_NUM; i++) {

		effectDir_[i].x = sinf(AngleUtility::Deg2RadF((float)GetRand(360)));
		effectDir_[i].y = sinf(AngleUtility::Deg2RadF((float)GetRand(360)));
		effectDir_[i].z = sinf(AngleUtility::Deg2RadF((float)GetRand(360)));

		effectTopPos_[i] = VAdd(VAdd(transform_.pos, EFFECT_POS), VScale(effectDir_[i], effectSize_));
		effectBottomPos_[i] = VAdd(transform_.pos, EFFECT_POS);
	}
	effectCnt_ = 30;
}

void Player::EffectUpdate(void)
{
	if (effectCnt_ < 0) return;

	//サイズの更新
	effectSize_ += (EFFECT_MAX_SIZE - effectSize_) / 8;

	//場所の更新
	for (int i = 0; i < EFFECT_NUM; i++) {

		effectTopPos_[i] = VAdd(VAdd(transform_.pos, EFFECT_POS), VScale(effectDir_[i], effectSize_));
		effectBottomPos_[i] = VAdd(transform_.pos, EFFECT_POS);
	}
	effectCnt_--;
}

void Player::SetFrame(void)
{
	swordPosStast_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "mixamorig:Sword_joint"));
	swordPosEnd_ = VTransform(SWORD_POS, MV1GetFrameLocalWorldMatrix(transform_.modelId, MV1SearchFrame(transform_.modelId, "mixamorig:RightHand")));
}
