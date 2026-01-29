#include "Player.h"
#include "../../Application.h"
#include "../../Utility/Utility.h"
#include "../../Utility/VectorUtility.h"
#include "../../Utility/AngleUtility.h"
#include "../../Manager/Audio/AudioManager.h"
#include "../../Manager/Audio/SoundTable.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Camera.h"
#include "../../Manager/Input/Controller.h"
#include "../Common/AnimationController.h"
#include "../Collider/ColliderCapsule.h"
#include "../Collider/ColliderLine.h"
#include "../Collider/ColliderModel.h"
#include "../Item.h"


Player::Player(Item* itm): ActorBase(), item_(itm), autoHealCnt_(0), autoHealHp_(0),
	dodgeCnt_(), dodgeFlg_(), healCount_(0), isAttack_(false), isHealMax_(), isHeal_(false), isStaminaMax_(false), knockBackDir_(0.0f),
	overFlg_(false), power_(0), staminaMaxCnt_(), stamina_(MAX_STAMINA), state_(STATE::WAIT), effectDir_(),se_(true),
	barEX_(), barHpEY_(), barHpSY_(), barSize_(), barSX_(),	barStaEY_(), barStaSY_(), damage_(BASIC_DAMAGE), goodDodge_(), greatDodge_(), guageEX_(),
	guageSize_(), guageSX_(), guageSY_(),hpBar_(), powerGauge_(), powerUp_(), powerUpCnt_(), dodgeBottomPos_(), dodgeTopPos_(), buff_(1.0), effectType_(EFFECT::NON)
{
}

Player::~Player(void)
{
}

void Player::InitLoad()
{
	//モデルのロード
	transform_.SetModel(MV1LoadModel((Application::PATH_MODEL + "Player.mv1").c_str()));
	powerGauge_ = LoadSoftImage((Application::PATH_IMAGE + "Power.png").c_str());
	hpBar_ = LoadSoftImage((Application::PATH_IMAGE + "HpBar.png").c_str());
}

void Player::InitAnim()
{
	//アニメーションのロード
	animationCtrl_ = new AnimationController(transform_.modelId);

	animationCtrl_->AddInFbx(0, 60.0f, 0);
			
	animationCtrl_->Add(1, 60.0f, (Application::PATH_ANIMATION + "Walking.mv1").c_str());
	animationCtrl_->Add(2, 60.0f, (Application::PATH_ANIMATION + "Run.mv1").c_str());
	animationCtrl_->Add(3, 90.0f, (Application::PATH_ANIMATION + "Slash.mv1").c_str());
	animationCtrl_->Add(4, 90.0f, (Application::PATH_ANIMATION + "Slash_1.mv1").c_str());
	animationCtrl_->Add(5, 90.0f, (Application::PATH_ANIMATION + "Slash_2.mv1").c_str());
	animationCtrl_->Add(6, 90.0f, (Application::PATH_ANIMATION + "Slash_3.mv1").c_str());
	animationCtrl_->Add(7, 100.0f, (Application::PATH_ANIMATION + "Dodge.mv1").c_str());
	animationCtrl_->Add(8, 90.0f, (Application::PATH_ANIMATION + "Hit_Light.mv1").c_str());
	animationCtrl_->Add(9, 60.0f, (Application::PATH_ANIMATION + "Hit_Heavy.mv1").c_str());
	animationCtrl_->Add(10, 200.0f, (Application::PATH_ANIMATION + "Hit_Up.mv1").c_str());
	animationCtrl_->Add(11, 45.0f, (Application::PATH_ANIMATION + "KO.mv1").c_str());
	animationCtrl_->Add(12, 330.0f, (Application::PATH_ANIMATION + "Drinking.mv1").c_str());
}

void Player::InitTransform()
{
	FindHpAndPower();

	transform_.pos = DEFAULT_POS;
	transform_.prevPos = transform_.pos;
	transform_.rot = Utility::VECTOR_ZERO;
	transform_.localRot = VScale(Utility::AXIS_Y, DX_PI_F);
	transform_.scl = Utility::VECTOR_ONE;
	
	swordTransform_.pos = MV1GetFramePosition(transform_.modelId, 58);
	swordTransform_.matRot = MV1GetFrameLocalWorldMatrix(transform_.modelId, 37);
	swordTransform_.Update();

	speed_ = 6.0f;
	hp_ = MAX_HP;
	moveDir_ = Utility::DIR_F;

	DoChangeState(STATE::WAIT);
}

void Player::InitCollider()
{
	// モデルコライダ
	ColliderModel* colModel = new ColliderModel(&transform_);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::MODEL), colModel);
	
	// 主に地面との衝突で仕様する線分コライダ
	ColliderLine* colLine = new ColliderLine(&transform_, COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule = new ColliderCapsule(&transform_, COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS, COL_CAPSULE_RADIUS);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);

	// 武器用のカプセルコライダ
	colCapsule = new ColliderCapsule(&swordTransform_, SWORD_POS, Utility::VECTOR_ZERO, COL_CAPSULE_RADIUS);
	swordColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);
}

void Player::Update(void)
{
	transform_.prevPos = transform_.pos;

	//状態別更新処理
	switch (state_) {

	case STATE::WAIT:
		
		UpdateWait();
		break;

	case STATE::MOVE:

		UpdateMove();
		break;

	case STATE::ATTACK:
		
		UpdateAttack();
		break;

	case STATE::COMBO:
		
		UpdateCombo();
		break;

	case STATE::DOGDE:
		
		UpdateDodge();
		break;

	case STATE::DAMAGED_LIGHT:
		
		UpdateDamagedLight();
		break;

	case STATE::DAMAGED_HEAVY:
		
		UpdateDamagedHeavy();
		break;

	case STATE::KO:

		UpdateKO();
		break;

	case STATE::DRINK:

		UpdateDrink();
		break;
	}

	Status();
	EffectUpdate();

	//モデルの設定
	transform_.Update();
	animationCtrl_->Update();
	MV1RefreshCollInfo(transform_.modelId);
}

void Player::DoChangeState(STATE state)
{
	state_ = state;

	//歩く走るの音を止める
	StopSE();

	switch (state_)
	{
	case Player::STATE::WAIT:

		ChangeWait();
		break;

	case Player::STATE::MOVE:
		
		ChangeMove();
		break;

	case Player::STATE::ATTACK:
		
		ChangeAttack();
		break;

	case Player::STATE::COMBO:

		ChangeCombo();
		break;

	case Player::STATE::DOGDE:
		
		ChangeDodge();
		break;

	case Player::STATE::DAMAGED_LIGHT:
		
		ChangeDamagedLight();
		break;

	case Player::STATE::DAMAGED_HEAVY:
	
		ChangeDamagedHeavy();
		break;

	case Player::STATE::KO:

		ChangeKO();
		break;

	case Player::STATE::DRINK:

		ChangeDrink();
		break;
	}
}

void Player::Draw(void) const
{
	if (effectCnt_ >= 0) {
		
		SetUseLighting(false);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 250);

		for (int i = 0; i < 9; i++) {
			switch (effectType_)
			{
			case Player::EFFECT::GREAT_DODGE:

				DrawCone3D(dodgeTopPos_[i], dodgeBottomPos_[i], 12.0f, 32, 0xffff00, 0xffff00, true);
				break;

			case Player::EFFECT::GOOD_DODGE:

				DrawCone3D(dodgeTopPos_[i], dodgeBottomPos_[i], 12.0f, 32, 0xffffff, 0xffffff, true);
				break;

			case Player::EFFECT::HEAL:

				DrawCone3D(dodgeTopPos_[i], dodgeBottomPos_[i], 12.0f, 32, 0x00ff00, 0x44cc44, true);
				break;

			case Player::EFFECT::STAMINA:

				DrawCone3D(dodgeTopPos_[i], dodgeBottomPos_[i], 12.0f, 32, 0xff5500, 0xaa3300, true);
				break;
			}
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		SetUseLighting(true);
	}
	DrawHpAndPower();
}

void Player::Release(void) const
{
	ActorBase::Release();

	DeleteSoftImage(hpBar_);
	DeleteSoftImage(powerGauge_);
}

void Player::Damage(int damage, float dir)
{
	hp_ -= damage;
	autoHealHp_ = damage / 3;
	knockBackDir_ = dir;
	greatDodge_ = goodDodge_ = false;

	AudioManager::GetInstance()->PlaySE(SoundID::SE_DAMAGE);

	if (isHealMax_) {

		isHealMax_ = false;
	}
	if (isHeal_) {

		isHeal_ = false;
	}

	if (damage >= 15) {

		DoChangeState(STATE::DAMAGED_HEAVY);
	}
	else {

		DoChangeState(STATE::DAMAGED_LIGHT);
	}

	if (hp_ <= 0) {

		hp_ = 0;
		autoHealHp_ = 0;
		SceneManager::GetInstance().SetScreenImage();
		DoChangeState(STATE::KO);
	}
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

		power_ += 3;
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
	//当たり判定の中心
	swordTransform_.pos = MV1GetFramePosition(transform_.modelId, 58);
	swordTransform_.matRot = MV1GetFrameLocalWorldMatrix(transform_.modelId, 37);
	swordTransform_.Update();

	Controller& ctrl = Controller::GetInstance();
	//ゲームパッドの情報を取得
	Controller::JOYPAD_IN_STATE padState = ctrl.GetJPadState(Controller::JOYPAD_NO::PAD1);

	if ((state_ != STATE::DOGDE && animationCtrl_->GetPlayType() != static_cast<int>(ANIM_TYPE::RUN)) || isStaminaMax_) {
		if (stamina_ < MAX_STAMINA) {

			stamina_++;
		}
	}
	if (stamina_ < 0.0f) {

		stamina_ = 0.0f;
	}
	if (isHeal_) {
		
		Heal();

		if (healCount_ >= HEAL_COUNT) {

				isHeal_ = false;
				healCount_ = 0;
		}
	}
	if (isHealMax_) {

		Heal();
	}
	if (isStaminaMax_) {
		if (staminaMaxCnt_ > STAMINA_MAX_TIME) {

			staminaMaxCnt_ = 0;
			isStaminaMax_ = false;
		}
		else {

			staminaMaxCnt_++;
		}
	}
	if (autoHealHp_ > 0) {

		autoHealCnt_++;

		if (autoHealCnt_ >= 65) {

			autoHealCnt_ = 0;
			autoHealHp_--;
			hp_++;
		}
	}
	if (powerUp_) {

		powerUpCnt_++;

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
	else {
		if (power_ >= MAX_POWER) {

			power_ = MAX_POWER;
			damage_ = BASIC_DAMAGE * 1.5;
			powerUp_ = true;
		}
	}
}

void Player::Heal(void)
{
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

			if (r ==0 && g == 0 && b == 255 && a > 0) {
				if (!first) {

					first = true;
					barSX_ = x;
					barHpSY_ = y;
				}
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

			if (r == 0 && g == 0 && b == 255 && a > 0) {
				if (!first) {

					first = true;
					barStaSY_ = y;
				}
				if (barStaEY_ < y) {

					barStaEY_ = y;
				}
			}
		}
	}
	barSize_ = barEX_ - barSX_;
	first = false;

	GetSoftImageSize(powerGauge_, &dx, &dy);

	float gx = 0;;

	for (float y = 0; y < static_cast<float>(dy); y++) {
		for (float x = 0; x <= static_cast<float>(dx); x++) {

			GetPixelSoftImage(powerGauge_, static_cast<int>(x), static_cast<int>(y), &r, &g, &b, &a);

			if (r == 0 && g == 0 && b == 255 && a > 0) {
				if (!first) {

					first = true;
					guageSX_ = x;
					guageSY_ = y;
				}
				gx = x;
			}
		}
		if (gx > 0) {
			guageEX_.push_back(gx + 1);
			gx = 0;
		}
	}
}

void Player::DrawHpAndPower(void) const
{
	float barRate = static_cast<float>(barSize_ / MAX_HP);
	float barNorm = barRate * hp_;
	float barRed = barRate * autoHealHp_;

	for (int y = static_cast<int>(barHpSY_); y <= static_cast<int>(barHpEY_); y++) {
	
		DrawLineSoftImage(hpBar_, static_cast<int>(barSX_), y, static_cast<int>(barEX_) + 1, y, 0, 0, 0, 255);
	}
	for (int y = static_cast<int>(barStaSY_); y <= static_cast<int>(barStaEY_); y++) {
	
		DrawLineSoftImage(hpBar_, static_cast<int>(barSX_), y, static_cast<int>(barEX_) + 1, y, 0, 0, 0, 255);
	}
	DrawSoftImage(190, 0, hpBar_);

	DrawBoxAA(190 + barSX_, barHpSY_, 190 + barSX_ + barNorm + barRed, barHpEY_, 0xff0000, true);
	DrawBoxAA(190 + barSX_, barHpSY_, 190 + barSX_ + barNorm, barHpEY_, 0x00ff00, true);

	barRate = barSize_ / static_cast<int>(MAX_STAMINA);
	barNorm = barRate * stamina_;

	if (isStaminaMax_) {

		DrawBoxAA(190 + barSX_, barStaSY_, 190 + barSX_ + barNorm, barStaEY_, GetColor(255, 255, (int)(std::abs(staminaMaxCnt_ % 101 - 50) * 5.1f)), true);
	}
	else {
		if (stamina_ <= DOGDE_STAMINA) {

			DrawBoxAA(190 + barSX_, barStaSY_, 190 + barSX_ + barNorm, barStaEY_, 0xff0000, true);
		}
		else {
			DrawBoxAA(190 + barSX_, barStaSY_, 190 + barSX_ + barNorm, barStaEY_, 0xffff00, true);
		}
	}

	float powerRate = (guageEX_.front() - guageSX_) / MAX_POWER;
	float power = guageSX_ + powerRate * power_;

	for (float y = 0; y < guageEX_.size(); y++) {

		float dy = y + guageSY_;
		DrawLineSoftImage(powerGauge_, guageSX_, dy, guageEX_.at(y), dy, 100, 100, 100, 255);

		if (!powerUp_) {
			if (power <= guageEX_.at(y)) {

				DrawLineSoftImage(powerGauge_, guageSX_, dy, power, dy, 255, 0, 0, 255);
			}
			else {

				DrawLineSoftImage(powerGauge_, guageSX_, dy, guageEX_.at(y), dy, 255, 0, 0, 255);
			}
		}
		else {
			int color = abs(powerUpCnt_ % 30 - 15) * 17;
			if (power <= guageEX_.at(y)) {

				DrawLineSoftImage(powerGauge_, guageSX_, dy, power, dy, 255, color, color, 255);
			}
			else {

				DrawLineSoftImage(powerGauge_, guageSX_, dy, guageEX_.at(y), dy, 255, color, color, 255);
			}
		}
	}
	DrawSoftImage(240, 100, powerGauge_);
}

void Player::ChangeWait(void) const
{
	//待機モーション
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
}

void Player::ChangeMove(void) const
{
}

void Player::ChangeAttack(void)
{
	//攻撃モーション
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::ATTACK), false);
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
	
		stamina_ -= DOGDE_STAMINA;
	}
}

void Player::ChangeDamagedLight(void) const
{
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::DAMAGED_LIGHT), false);
}

void Player::ChangeDamagedHeavy(void) const
{
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::DAMAGED_HEAVY), false);
}

void Player::ChangeKO(void) const
{
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::KO), false);
}

void Player::ChangeDrink(void) const
{
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::DRINK), false);
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
	Controller& ctrl = Controller::GetInstance();
	//ゲームパッドの情報を取得
	Controller::JOYPAD_IN_STATE padState = ctrl.GetJPadState(Controller::JOYPAD_NO::PAD1);

	//行列と方向の設定
	MATRIX mat = MGetIdent();
	VECTOR dir = { 0.0f, 0.0f, 0.0f };

	//前後左右の移動処理
	//方向の取得
	dir = ctrl.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);
	
	if (!VectorUtility::EqualsVZero(dir)) {

		Camera* camera = SceneManager::GetInstance().GetCamera();
		//カメラの角度を得る
		VECTOR angle = camera->GetCameraAngles();

		//回転行列を取る
		//mat = MMult(mat, MGetRotX(angle.x));
		mat = MMult(mat, MGetRotY(angle.y));
		//mat = MMult(mat, MGetRotZ(angle.z));

		//進む向きに回転させる
		dir = VNorm(dir);
		moveDir_ = VTransform(dir, mat);

		if (padState.IsNew[static_cast<int>(Controller::JOYPAD_BTN::R)]) {
			if (stamina_ > 0.0f) {

				//移動モーション
				animationCtrl_->Play(static_cast<int>(ANIM_TYPE::RUN), true);
				
				if (se_) {
					if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_WALK)) {

						AudioManager::GetInstance()->StopSE(SoundID::SE_WALK);
					}
					if (!AudioManager::GetInstance()->IsPlaySE(SoundID::SE_RUN)) {

						AudioManager::GetInstance()->PlaySE(SoundID::SE_RUN);
					}
				}

				//移動させる
				transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_ * 1.75f));

				if (!isStaminaMax_) {
				
					stamina_--;
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
			
			if (se_) {
				if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_RUN)) {

					AudioManager::GetInstance()->StopSE(SoundID::SE_RUN);
				}
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

		DoChangeState(STATE::WAIT);
	}
}

void Player::UpdateAttack(void)
{
	if (animationCtrl_->GetTime() >= 35.0f && animationCtrl_->GetTime() <= 36.0f) {
		
		isAttack_ = true;
	}
	if (animationCtrl_->GetTime() >= 61.0f && animationCtrl_->GetTime() <= 62.0f) {
		
		isAttack_ = true;
	}
	if (animationCtrl_->GetTime() >= 92.0f && animationCtrl_->GetTime() <= 93.0f) {

		isAttack_ = false;
	}
	if (animationCtrl_->GetTime() >= 138.0f && animationCtrl_->GetTime() <= 139.0f) {

		buff_ = 1.1;
		isAttack_ = true;
	}
	if (animationCtrl_->GetTime() >= 160.0f) {

		isAttack_ = false;
	}
	if (animationCtrl_->GetTime() <= 138.0f) {
	
		//移動させる
		transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_ * 0.2f));
	}

	if (animationCtrl_->IsEnd()) {

		buff_ = 1.0;
		isAttack_ = false;
		//待機モーションに移行
		DoChangeState(STATE::WAIT);
	}
}

void Player::UpdateCombo(void)
{
	Controller& ctrl = Controller::GetInstance();
	//ゲームパッドの情報を取得
	Controller::JOYPAD_IN_STATE padState = ctrl.GetJPadState(Controller::JOYPAD_NO::PAD1);

	if (animationCtrl_->GetPlayType() == static_cast<int>(ANIM_TYPE::COMBO_1)) {
		if (animationCtrl_->GetTime() >= 32.0f && animationCtrl_->GetTime() <= 34.0f) {

			isAttack_ = true;
		}
		if (animationCtrl_->GetTime() >= 67.5f) {
			
			isAttack_ = false;

			if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::RIGHT)]) {

				animationCtrl_->Play(static_cast<int>(ANIM_TYPE::COMBO_2), false);
			}
		}
	}
	if (animationCtrl_->GetPlayType() == static_cast<int>(ANIM_TYPE::COMBO_2)) {
		if (animationCtrl_->GetTime() >= 40.0f && animationCtrl_->GetTime() <= 41.0f) {

			isAttack_ = true;

		}
		if (animationCtrl_->GetTime() >= 75.0f) {
			
			isAttack_ = false;

			if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::RIGHT)]) {

				animationCtrl_->Play(static_cast<int>(ANIM_TYPE::COMBO_3), false);
			}
		}
	}
	if (animationCtrl_->GetPlayType() == static_cast<int>(ANIM_TYPE::COMBO_3)) {
		if (animationCtrl_->GetTime() >= 43.0f && animationCtrl_->GetTime() <= 44.0f) {

			buff_ = 1.1;
			isAttack_ = true;
		}
		if (animationCtrl_->GetTime() >= 61.0f) {
			
			buff_ = 1.0;
			isAttack_ = false;
		}
	}
	if (animationCtrl_->IsEnd()) {

		isAttack_ = false;
		//待機モーションに移行
		DoChangeState(STATE::WAIT);
	}
}

void Player::UpdateDodge(void)
{
	//移動させる
	transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_ * 1.1f));

	if (dodgeFlg_) {
		if (dodgeCnt_ <= 15.0f) {

			dodgeCnt_++;
		}
		else {

			dodgeFlg_ = false;
			dodgeCnt_ = 0;
		}
	}

	if (animationCtrl_->IsEnd()) {

		//待機モーションに移行
		DoChangeState(STATE::WAIT);

		greatDodge_ = goodDodge_ = false;
	}
}

void Player::UpdateDamagedLight(void)
{
	if (animationCtrl_->GetTime() >= 40) {

		DoChangeState(STATE::WAIT);
	}
}

void Player::UpdateDamagedHeavy(void)
{
	static int steps = 0;
	static bool prevPause = false;

	if (animationCtrl_->GetPlayType() == static_cast<int>(ANIM_TYPE::DAMAGED_HEAVY)) {
		if (animationCtrl_->GetTime() <= 110) {

			KnockBack();
		}
		if (!prevPause){
			if (animationCtrl_->GetTime() >= 130) {
				
				prevPause = true;
				animationCtrl_->ChangePause(true);
			}
		}
		if (animationCtrl_->IsPause()) {

			steps++;

			Controller& ctrl = Controller::GetInstance();
			//ゲームパッドの情報を取得
			Controller::JOYPAD_IN_STATE padState = ctrl.GetJPadState(Controller::JOYPAD_NO::PAD1);

			if (padState.Anyone || steps >= 90) {

				steps = 0;
				animationCtrl_->ChangePause(false);
			}
		}
		if (animationCtrl_-> GetTime() >= 160) {

			prevPause = false;
			animationCtrl_->Play(static_cast<int>(ANIM_TYPE::STAND_UP), false);
		}
	}
	if (animationCtrl_->GetPlayType() == static_cast<int>(ANIM_TYPE::STAND_UP)) {
		if (animationCtrl_->GetTime() >= 210) {

			DoChangeState(STATE::WAIT);
		}
	}
}

void Player::UpdateKO(void)
{
	overFlg_ = true;
}

void Player::UpdateDrink(void)
{
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
	EffectCreate();
	DoChangeState(STATE::WAIT);
}

void Player::BoolChangeMove(void)
{
	//ゲームパッドの情報を取得
	Controller::JOYPAD_IN_STATE padState = Controller::GetInstance().GetJPadState(Controller::JOYPAD_NO::PAD1);

	if (padState.AKeyLX != 0 || padState.AKeyLY != 0) {

		//移動モーションに移行
		DoChangeState(STATE::MOVE);
	}
}

void Player::BoolChangeAttack(void)
{
	//ゲームパッドの情報を取得
	Controller::JOYPAD_IN_STATE padState = Controller::GetInstance().GetJPadState(Controller::JOYPAD_NO::PAD1);

	if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::TOP)]) {

		//攻撃モーションに移行
		DoChangeState(STATE::ATTACK);
	}
}

void Player::BoolChangeCombo(void)
{
	//ゲームパッドの情報を取得
	Controller::JOYPAD_IN_STATE padState = Controller::GetInstance().GetJPadState(Controller::JOYPAD_NO::PAD1);

	if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::RIGHT)]) {

		//攻撃モーションに移行
		DoChangeState(STATE::COMBO);
	}
}

void Player::BoolChangeDodge(void)
{
	//ゲームパッドの情報を取得
	Controller::JOYPAD_IN_STATE padState = Controller::GetInstance().GetJPadState(Controller::JOYPAD_NO::PAD1);

	if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::DOWN)]) {
		if (stamina_ >= DOGDE_STAMINA || isStaminaMax_) {

			//回避モーションに移行
			DoChangeState(STATE::DOGDE);
		}
	}
}

void Player::BoolChangeDrink(void)
{
	//ゲームパッドの情報を取得
	Controller::JOYPAD_IN_STATE padState = Controller::GetInstance().GetJPadState(Controller::JOYPAD_NO::PAD1);

	if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::LEFT)] || CheckHitKey(KEY_INPUT_0)) {
		if (!isHeal_ && !isHealMax_) {

			//飲むモーションに移行
			DoChangeState(STATE::DRINK);
		}
	}
}

void Player::StopSE(void)
{
	if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_RUN)) {

		AudioManager::GetInstance()->StopSE(SoundID::SE_RUN);
	}
	if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_WALK)) {
				
		AudioManager::GetInstance()->StopSE(SoundID::SE_WALK);
	}
}

void Player::EffectCreate(void)
{
	effectSize_ = EFFECT_MAX_SIZE / 3;

	for (int i = 0; i < EFFECT_NUM; i++) {

		effectDir_[i].x = sinf(AngleUtility::Deg2RadF((float)GetRand(360)));
		effectDir_[i].y = sinf(AngleUtility::Deg2RadF((float)GetRand(360)));
		effectDir_[i].z = sinf(AngleUtility::Deg2RadF((float)GetRand(360)));

		dodgeTopPos_[i] = VAdd(VAdd(transform_.pos, { 0.0f, 70.0f, 0.0f }), VScale(effectDir_[i], effectSize_));
		dodgeBottomPos_[i] = VAdd(transform_.pos, { 0.0f, 70.0f, 0.0f });
	}
	effectCnt_ = 30;
}

void Player::EffectUpdate(void)
{
	if (effectCnt_ < 0) return;

	effectSize_ += (EFFECT_MAX_SIZE - effectSize_) / 8;

	for (int i = 0; i < EFFECT_NUM; i++) {

		dodgeTopPos_[i] = VAdd(VAdd(transform_.pos, { 0.0f, 70.0f, 0.0f }), VScale(effectDir_[i], effectSize_));
		dodgeBottomPos_[i] = VAdd(transform_.pos, { 0.0f, 70.0f, 0.0f });
	}
	effectCnt_--;
}
