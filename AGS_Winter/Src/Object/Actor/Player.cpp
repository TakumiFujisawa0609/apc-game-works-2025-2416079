#include "Player.h"
#include "../../Application.h"
#include "../../Utility/Utility.h"
#include "../../Utility/VectorUtility.h"
#include "../../Utility/AngleUtility.h"
#include "../Common/AnimationController.h"
#include "../../Manager/Audio/AudioManager.h"
#include "../../Manager/Audio/SoundTable.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Camera.h"
#include "../../Manager/Input/Controller.h"


Player::Player(void):angles_(Utility::VECTOR_ZERO), animationController_(nullptr), attackPos1_(), attackPos2_(),autoHealCnt_(0), autoHealHp_(0),
	dodgeCnt_(), dodgeFlg_(), healCount_(0), hp_(MAX_HP), isAttack_(false), isHealMax_(), isHeal_(false), isStaminaMax_(), knockBackDir_(0.0f),
	modelId_(), moveDir_(Utility::DIR_F), overFlg_(false), pos_(DEFAULT_POS), power_(MAX_POWER), prevPos_(DEFAULT_POS), scales_(Utility::VECTOR_ONE),
	speed_(6.0f), staminaMaxCnt_(), stamina_(MAX_STAMINA), state_(STATE::WAIT), effectDir_(), barEX_(), barHpEY_(), barHpSY_(), barSize_(), barSX_(),
	barStaEY_(), barStaSY_(), damage_(), goodDodge_(), greatDodge_(), guageEX_(),guageSize_(), guageSX_(), guageSY_(),hpBar_(), powerGauge_(),
	powerUp_(), powerUpCnt_(), dodgeBottomPos_(), dodgeTopPos_()
{
}

Player::~Player(void)
{
}

void Player::Init()
{
	//モデルのロード
	modelId_ = MV1LoadModel((Application::PATH_MODEL + "Player.mv1").c_str());
	powerGauge_ = LoadSoftImage((Application::PATH_IMAGE + "Power.png").c_str());
	hpBar_ = LoadSoftImage((Application::PATH_IMAGE + "HpBar.png").c_str());

	FindHpAndPower();

	//モデルの設定
	MV1SetPosition(modelId_, pos_);
	MV1SetRotationMatrix(modelId_, AngleUtility::Multiplication(DIFF_ANGLES, angles_));
	MV1SetScale(modelId_, scales_);
	MV1SetupCollInfo(modelId_);

	//アニメーションのロード
	animationController_ = new AnimationController(modelId_);

	for (int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++) {
		switch (i) {
		case 0:
	
			animationController_->AddInFbx(i, 60.0f, i);
			break;

		case 1:

			animationController_->Add(i, 60.0f, (Application::PATH_ANIMATION + "Walking.mv1").c_str());
			break;

		case 2:

			animationController_->Add(i, 60.0f, (Application::PATH_ANIMATION + "Run.mv1").c_str());
			break;
		
		case 3:

			animationController_->Add(i, 90.0f, (Application::PATH_ANIMATION + "Slash.mv1").c_str());
			break;	
	
		case 4:

			animationController_->Add(i, 90.0f, (Application::PATH_ANIMATION + "Slash_1.mv1").c_str());
			break;	
	
		case 5:

			animationController_->Add(i, 90.0f, (Application::PATH_ANIMATION + "Slash_2.mv1").c_str());
			break;	
			
		case 6:

			animationController_->Add(i, 90.0f, (Application::PATH_ANIMATION + "Slash_3.mv1").c_str());
			break;	

		case 7:

			animationController_->Add(i, 100.0f, (Application::PATH_ANIMATION + "Dodge.mv1").c_str());
			break;

		case 8:

			animationController_->Add(i, 90.0f, (Application::PATH_ANIMATION + "Hit_Light.mv1").c_str());
			break;	
			
		case 9:

			animationController_->Add(i, 60.0f, (Application::PATH_ANIMATION + "Hit_Heavy.mv1").c_str());
			break;	

		case 10:

			animationController_->Add(i, 200.0f, (Application::PATH_ANIMATION + "Hit_Up.mv1").c_str());
			break;
		}
	}
	ChangeState(STATE::WAIT);
}

void Player::Update(void)
{
	prevPos_ = pos_;

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
	}

	Status();

	//モデルの設定
	MV1SetPosition(modelId_, pos_);
	MV1SetRotationMatrix(modelId_, AngleUtility::Multiplication(DIFF_ANGLES, angles_));
	MV1SetScale(modelId_, scales_);

	animationController_->Update();
	MV1SetupCollInfo(modelId_);
}

void Player::ChangeState(STATE state)
{
	state_ = state;

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
	}

}

void Player::Draw(void)
{
	if (greatDodge_) {
		for (int i = 0; i < 9; i++) {
			DrawCone3D(dodgeTopPos_[i], dodgeBottomPos_[i], 12.0f, 32, 0xffff00, 0xffffff, true);
		}
	}
	else if (goodDodge_) {
		for (int i = 0; i < 9; i++) {
			DrawCone3D(dodgeTopPos_[i], dodgeBottomPos_[i], 12.0f, 32, 0xffffff, 0xffffff, true);
		}
	}
	DrawHpAndPower();

	//デバック

	//DrawFormatString(0, 0, 0x000000, "%f", SceneManager::GetInstance().GetDeltaTime());

	//DrawBoxAA(22.5f, 17.5f, x, 57.5f, 0x222222, true);
	//DrawBoxAA(25.0f, 20.0f, dx + d3x, 35.0f, 0xff0000, true);
	//DrawBoxAA(25.0f, 20.0f, dx, 35.0f, 0x00ff00, true);
	//if (isStaminaMax_) {

	//	DrawBoxAA(25.0f, 40.0f, d2x, 55.0f, GetColor(255, 255, (int)(std::abs(staminaMaxCnt_ % 101 - 50) * 5.1f)), true);
	//}
	//else {
	//	if (stamina_ <= DOGDE_STAMINA) {
	//		DrawBoxAA(25.0f, 40.0f, d2x, 55.0f, 0xff0000, true);
	//	}
	//	else {
	//		DrawBoxAA(25.0f, 40.0f, d2x, 55.0f, 0xffff00, true);
	//	}
	//}
	/*int i = Controller::GetInstance().GetJPadInputState(Controller::JOYPAD_NO::PAD1).IsTrgDown[4];
	DrawFormatString(5, 5, 0xffffff, "%d", i);*/
	//DrawCapsule3D(attackPos1_, attackPos2_, 10.0f, 16, 0x00ffff, 0x00ffff, false);
	//DrawSphere3D(pos, 25.0f, 16, 0x00ff00, 0x00ff00, true);
	//Controller& ctrl = Controller::GetInstance();
	////ゲームパッドの情報を取得
	//Controller::JOYPAD_IN_STATE padState = ctrl.GetJPadInputState(Controller::JOYPAD_NO::PAD1);
	//for (int i = 0; i < static_cast<int>(Controller::JOYPAD_BTN::MAX); i++) {
	//	DrawFormatString(0, 20 * i, 0x000000, "%d", padState.IsTrgDown[i]);
	//}
	//DrawFormatString(50, 15, 0xfffff , "%.2f", animationController_->GetTime());
}

void Player::Release(void)
{
	//アニメーションコントローラーのリリース
	animationController_->Release();
	delete animationController_;

	//モデルの削除
	MV1DeleteModel(modelId_);
	DeleteSoftImage(hpBar_);
	DeleteSoftImage(powerGauge_);
}

void Player::DrawModel(void) const
{
	MV1DrawModel(modelId_);
}

void Player::Damage(int damage, float dir)
{
	hp_ -= damage;
	autoHealHp_ = damage / 3;
	knockBackDir_ = dir;
	greatDodge_ = goodDodge_ = false;

	if (damage >= 15) {

		ChangeState(STATE::DAMAGED_HEAVY);
	}
	else {

		ChangeState(STATE::DAMAGED_LIGHT);
	}

	if (hp_ <= 0) {

		hp_ = 0;
		overFlg_ = true;
	}
}

bool Player::Healable(void) const
{
	return animationController_->GetPlayType() == static_cast<int>(ANIM_TYPE::IDLE) || animationController_->GetPlayType() == static_cast<int>(ANIM_TYPE::WALK) || animationController_->GetPlayType() == static_cast<int>(ANIM_TYPE::RUN);
}

bool Player::IsCollisionState(void)
{
	return false;
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

	for (int i = 0; i < EFFECT_NUM; i++) {

		effectDir_[i].x = sinf(AngleUtility::Deg2RadF((float)GetRand(360)));
		effectDir_[i].y = sinf(AngleUtility::Deg2RadF((float)GetRand(360)));
		effectDir_[i].z = sinf(AngleUtility::Deg2RadF((float)GetRand(360)));

		effectDir_[i] = VScale(VNorm(effectDir_[i]), 20.0f);

		dodgeTopPos_[i] = VAdd(VAdd(pos_, { 0.0f, 70.0f, 0.0f }), effectDir_[i]);
		dodgeBottomPos_[i] = VAdd(pos_, { 0.0f, 70.0f, 0.0f });
	}
	if (!powerUp_) {

		power_ += 3;
	}
}

void Player::GoodDodge(void)
{
	goodDodge_ = true;

	for (int i = 0; i < EFFECT_NUM; i++) {

		effectDir_[i].x = sinf(AngleUtility::Deg2RadF((float)GetRand(360)));
		effectDir_[i].y = sinf(AngleUtility::Deg2RadF((float)GetRand(360)));
		effectDir_[i].z = sinf(AngleUtility::Deg2RadF((float)GetRand(360)));

		effectDir_[i] = VScale(VNorm(effectDir_[i]), 20.0f);

		dodgeTopPos_[i] = VAdd(VAdd(pos_, { 0.0f, 70.0f, 0.0f }), VScale(effectDir_[i], 30.0f));
		dodgeBottomPos_[i] = VAdd(pos_, { 0.0f, 70.0f, 0.0f });
	}
	if (!powerUp_) {

		power_++;
	}
	AudioManager::GetInstance()->PlaySE(SoundID::SE_DODGE);
}

void Player::Status(void)
{
	//当たり判定の中心
	attackPos1_ = MV1GetFramePosition(modelId_, 58);
	attackPos2_ = VTransform(SWORD_POS, MV1GetFrameLocalWorldMatrix(modelId_, 37));

	Controller& ctrl = Controller::GetInstance();
	//ゲームパッドの情報を取得
	Controller::JOYPAD_IN_STATE padState = ctrl.GetJPadState(Controller::JOYPAD_NO::PAD1);

	if (state_ != STATE::DOGDE && !padState.IsNew[static_cast<int>(Controller::JOYPAD_BTN::R)]) {
		if (stamina_ < MAX_STAMINA) {

			stamina_++;
		}
	}
	if (stamina_ < 0.0f) {

		stamina_ = 0.0f;
	}
	if (isHeal_) {
		if (hp_ < MAX_HP) {
			if (autoHealHp_ > 0) {

				autoHealHp_--;
			}
			hp_++;
			healCount_++;
		}
		if (healCount_ >= HEAL_COUNT || hp_ >= MAX_HP) {

			isHeal_ = false;
			healCount_ = 0;
		}
	}
	if (isHealMax_) {
		if (hp_ < MAX_HP) {
			if (autoHealHp_ > 0) {

				autoHealHp_--;
			}
			hp_++;
		}
		else {

			isHealMax_ = false;
		}
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
			powerUp_ = false;
		}
	}
	if (power_ >= MAX_POWER) {

		power_ = MAX_POWER;

		if (!powerUp_) {

			damage_ = BASIC_DAMAGE * 1.5;
			powerUp_ = true;
		}
	}
}

void Player::KnockBack()
{
	pos_.x += sinf(knockBackDir_) * 2.0f;
	pos_.z += cosf(knockBackDir_) * 2.0f;

	angles_.y = knockBackDir_ - DX_PI_F;
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

void Player::DrawHpAndPower(void)
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

			DrawBoxAA(240 + barSX_, barStaSY_, 190 + barSX_ + barNorm, barStaEY_, 0xff0000, true);
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

void Player::ChangeWait(void)
{
	if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_RUN)) {

		AudioManager::GetInstance()->StopSE(SoundID::SE_RUN);
	}
	if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_WALK)) {

		AudioManager::GetInstance()->StopSE(SoundID::SE_WALK);
	}
	//待機モーション
	animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
}

void Player::ChangeMove(void)
{
}

void Player::ChangeAttack(void)
{
	if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_RUN)) {

		AudioManager::GetInstance()->StopSE(SoundID::SE_RUN);
	}
	if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_WALK)) {

		AudioManager::GetInstance()->StopSE(SoundID::SE_WALK);
	}
	//攻撃モーション
	animationController_->Play(static_cast<int>(ANIM_TYPE::ATTACK), false);
	isAttack_ = true;
}

void Player::ChangeCombo(void)
{
	if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_RUN)) {

		AudioManager::GetInstance()->StopSE(SoundID::SE_RUN);
	}
	if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_WALK)) {

		AudioManager::GetInstance()->StopSE(SoundID::SE_WALK);
	}
	//攻撃モーション
	animationController_->Play(static_cast<int>(ANIM_TYPE::COMBO_1), false);
	isAttack_ = true;
}

void Player::ChangeDodge(void)
{
	if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_RUN)) {

		AudioManager::GetInstance()->StopSE(SoundID::SE_RUN);
	}
	if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_WALK)) {

		AudioManager::GetInstance()->StopSE(SoundID::SE_WALK);
	}
	animationController_->Play(static_cast<int>(ANIM_TYPE::DODGE), false);
	dodgeCnt_ = 0;
	dodgeFlg_ = true;
	if (!isStaminaMax_) {
	
		stamina_ -= DOGDE_STAMINA;
	}
}

void Player::ChangeDamagedLight(void)
{
	if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_RUN)) {

		AudioManager::GetInstance()->StopSE(SoundID::SE_RUN);
	}
	if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_WALK)) {

		AudioManager::GetInstance()->StopSE(SoundID::SE_WALK);
	}
	animationController_->Play(static_cast<int>(ANIM_TYPE::DAMAGED_LIGHT), false);
}

void Player::ChangeDamagedHeavy(void)
{
	if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_RUN)) {

		AudioManager::GetInstance()->StopSE(SoundID::SE_RUN);
	}
	if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_WALK)) {

		AudioManager::GetInstance()->StopSE(SoundID::SE_WALK);
	}
	animationController_->Play(static_cast<int>(ANIM_TYPE::DAMAGED_HEAVY), false);
}

void Player::UpdateWait(void)
{
	if (GetJoypadNum() == 0) {
		if (CheckHitKey(KEY_INPUT_F) == 1) {
		
			//攻撃モーションに移行
			ChangeState(STATE::ATTACK);
		}
		if (CheckHitKey(KEY_INPUT_G) == 1) {
		
			//攻撃モーションに移行
			ChangeState(STATE::COMBO);
		}
		if (CheckHitKey(KEY_INPUT_W) == 1 || CheckHitKey(KEY_INPUT_A) == 1 || CheckHitKey(KEY_INPUT_S) == 1 || CheckHitKey(KEY_INPUT_D) == 1) {

			//移動モーションに移行
			ChangeState(STATE::MOVE);
		}
		if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
			if (stamina_ >= DOGDE_STAMINA) {

				//回避モーションに移行
				ChangeState(STATE::DOGDE);
			}
		}
	}
	else {

		Controller& ctrl = Controller::GetInstance();
		//ゲームパッドの情報を取得
		Controller::JOYPAD_IN_STATE padState = ctrl.GetJPadState(Controller::JOYPAD_NO::PAD1);
		
		if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::TOP)]) {

			//攻撃モーションに移行
			ChangeState(STATE::ATTACK);
		}
		if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::RIGHT)]) {

			//攻撃モーションに移行
			ChangeState(STATE::COMBO);
		}
		if (padState.AKeyLX != 0 || padState.AKeyLY != 0) {

			//移動モーションに移行
			ChangeState(STATE::MOVE);
		}
		if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::DOWN)]) {
			if (stamina_ >= DOGDE_STAMINA) {

				//回避モーションに移行
				ChangeState(STATE::DOGDE);
			}
		}
	}
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
	if (GetJoypadNum() == 0) {

		if (CheckHitKey(KEY_INPUT_W) == 1) {

			dir = VAdd(dir, { 0.0f, 0.0f, 1.0f });
		}
		if (CheckHitKey(KEY_INPUT_S) == 1) {

			dir = VAdd(dir, { 0.0f, 0.0f, -1.0f });
		}
		if (CheckHitKey(KEY_INPUT_D) == 1) {

			dir = VAdd(dir, { 1.0f, 0.0f, 0.0f });
		}
		if (CheckHitKey(KEY_INPUT_A) == 1) {

			dir = VAdd(dir, { -1.0f, 0.0f, 0.0f });
		}
	}
	else {
		//方向の取得
		dir = ctrl.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);
	}
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
				animationController_->Play(static_cast<int>(ANIM_TYPE::RUN), true);
				
				if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_WALK)) {

					AudioManager::GetInstance()->StopSE(SoundID::SE_WALK);
				}
				if (!AudioManager::GetInstance()->IsPlaySE(SoundID::SE_RUN)) {

					AudioManager::GetInstance()->PlaySE(SoundID::SE_RUN);
				}

				//移動させる
				pos_ = VAdd(pos_, VScale(moveDir_, speed_ * 1.75f));

				if (!isStaminaMax_) {
				
					stamina_--;
				}
			}
			else {

				//移動モーション
				animationController_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
				
				if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_RUN)) {

					AudioManager::GetInstance()->StopSE(SoundID::SE_RUN);
				}
				if (!AudioManager::GetInstance()->IsPlaySE(SoundID::SE_WALK)) {
				
					AudioManager::GetInstance()->PlaySE(SoundID::SE_WALK);
				}
				//移動させる
				pos_ = VAdd(pos_, VScale(moveDir_, speed_));
			}
		}
		else {

			//移動モーション
			animationController_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
			
			if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_RUN)) {

				AudioManager::GetInstance()->StopSE(SoundID::SE_RUN);
			}
			if (!AudioManager::GetInstance()->IsPlaySE(SoundID::SE_WALK)) {
			
				AudioManager::GetInstance()->PlaySE(SoundID::SE_WALK);
			}
			//移動させる
			pos_ = VAdd(pos_, VScale(moveDir_, speed_));
		}
		if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::TOP)]) {

			//攻撃モーションに移行
			ChangeState(STATE::ATTACK);
		}
		if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::RIGHT)]) {

			//攻撃モーションに移行
			ChangeState(STATE::COMBO);
		}
		if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::DOWN)]) {
			if (stamina_ >= DOGDE_STAMINA) {

				//回避モーションに移行
				ChangeState(STATE::DOGDE);
			}
		}
		angles_.y = atan2f(moveDir_.x, moveDir_.z);
	}
	else {
		if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_RUN)) {

			AudioManager::GetInstance()->StopSE(SoundID::SE_RUN);
		}
		if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_WALK)) {

			AudioManager::GetInstance()->StopSE(SoundID::SE_WALK);
		}
		ChangeState(STATE::WAIT);
	}
}

void Player::UpdateAttack(void)
{
	if (animationController_->GetTime() >= 61.0f && animationController_->GetTime() <= 62.0f) {
		
		isAttack_ = true;
	}
	if (animationController_->GetTime() >= 92.0f && animationController_->GetTime() <= 93.0f) {

		isAttack_ = false;
	}
	if (animationController_->GetTime() >= 115.0f && animationController_->GetTime() <= 116.0f) {

		isAttack_ = true;
	}
	if (animationController_->GetTime() <= 138.0f) {
	
		//移動させる
		pos_ = VAdd(pos_, VScale(moveDir_, speed_ * 0.2f));
	}
	else {

		damage_ *= 1.2;
		isAttack_ = true;
	}

	if (animationController_->IsEnd()) {

		isAttack_ = false;
		//待機モーションに移行
		ChangeState(STATE::WAIT);
	}
}

void Player::UpdateCombo(void)
{
	Controller& ctrl = Controller::GetInstance();
	//ゲームパッドの情報を取得
	Controller::JOYPAD_IN_STATE padState = ctrl.GetJPadState(Controller::JOYPAD_NO::PAD1);

	if (animationController_->GetPlayType() == static_cast<int>(ANIM_TYPE::COMBO_1)) {
		if (animationController_->GetTime() >= 67.5f) {
			
			isAttack_ = false;

			if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::RIGHT)]) {

				isAttack_ = true;
				animationController_->Play(static_cast<int>(ANIM_TYPE::COMBO_2), false);
			}
		}
	}
	if (animationController_->GetPlayType() == static_cast<int>(ANIM_TYPE::COMBO_2)) {
		if (animationController_->GetTime() >= 75.0f) {
			
			isAttack_ = false;

			if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::RIGHT)]) {

				isAttack_ = true;
				animationController_->Play(static_cast<int>(ANIM_TYPE::COMBO_3), false);
				damage_ *= 1.1;
			}
		}
	}
	if (animationController_->IsEnd()) {

		isAttack_ = false;
		//待機モーションに移行
		ChangeState(STATE::WAIT);
	}
}

void Player::UpdateDodge(void)
{
	//移動させる
	pos_ = VAdd(pos_, VScale(moveDir_, speed_ * 1.1f)); 

	if (dodgeFlg_) {
		if (dodgeCnt_ <= 13.0f) {

			dodgeCnt_++;
		}
		else {

			dodgeFlg_ = false;
			dodgeCnt_ = 0;
		}
	}
	if (goodDodge_ || greatDodge_) {
		for (int i = 0; i < EFFECT_NUM; i++) {

			effectDir_[i] = VScale(effectDir_[i], 1.05f);

			dodgeTopPos_[i] = VAdd(VAdd(pos_, { 0.0f, 70.0f, 0.0f }), effectDir_[i]);
			dodgeBottomPos_[i] = VAdd(pos_, { 0.0f, 70.0f, 0.0f });
		}
	}

	if (animationController_->IsEnd()) {

		//待機モーションに移行
		ChangeState(STATE::WAIT);

		greatDodge_ = goodDodge_ = false;
	}
}

void Player::UpdateDamagedLight(void)
{
	if (animationController_->GetTime() >= 40) {

		ChangeState(STATE::WAIT);
	}
}

void Player::UpdateDamagedHeavy(void)
{
	static int steps = 0;
	static bool prevPause = false;

	if (animationController_->GetPlayType() == static_cast<int>(ANIM_TYPE::DAMAGED_HEAVY)) {
		if (animationController_->GetTime() <= 110) {

			KnockBack();
		}
		if (!prevPause){
			if (animationController_->GetTime() >= 130) {
				
				prevPause = true;
				animationController_->ChangePause(true);
			}
		}
		if (animationController_->IsPause()) {

			steps++;

			Controller& ctrl = Controller::GetInstance();
			//ゲームパッドの情報を取得
			Controller::JOYPAD_IN_STATE padState = ctrl.GetJPadState(Controller::JOYPAD_NO::PAD1);

			if (padState.Anyone || steps >= 90) {

				steps = 0;
				animationController_->ChangePause(false);
			}
		}
		if (animationController_-> GetTime() >= 160) {

			prevPause = false;
			animationController_->Play(static_cast<int>(ANIM_TYPE::STAND_UP), false);
		}
	}


	if (animationController_->GetPlayType() == static_cast<int>(ANIM_TYPE::STAND_UP)) {
		if (animationController_->GetTime() >= 210) {

			ChangeState(STATE::WAIT);
		}
	}
}
