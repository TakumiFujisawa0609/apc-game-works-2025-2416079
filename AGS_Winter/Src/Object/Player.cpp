#include "Player.h"
#include "../Application.h"
#include "../Utility/Utility.h"
#include "../Utility/VectorUtility.h"
#include "../Utility/AngleUtility.h"
#include "Common/AnimationController.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/Controller.h"


Player::Player(void)
{
}

Player::~Player(void)
{
}

void Player::Init()
{
	//モデルのロード
	modelId_ = MV1LoadModel((Application::PATH_MODEL + "Player.mv1").c_str());

	//変数の初期化
	pos_ = DEFAULT_POS;
	angles_ = Utility::VECTOR_ZERO;
	moveDir_ = { 0.0f, 0.0f, 1.0f };
	scales_ = { 1.0f, 1.0f, 1.0f };

	knockBackDir_ = 0.0f;

	power_ = 1;
	isAttack_ = false;

	hp_ = MAX_HP;
	stamina_ = MAX_STAMINA;
	overFlg_ = false;

	speed_ = 6.0f;

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

			animationController_->Add(i, 90.0f, (Application::PATH_ANIMATION + "Dodge.mv1").c_str());
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
	state_ = STATE::WAIT;
	ChangeState(state_);
}

void Player::Update(void)
{
	Player::STATE prevState = state_;

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
	if (prevState != state_) {

		ChangeState(state_);
	}
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

	//モデルの設定
	MV1SetPosition(modelId_, pos_);
	MV1SetRotationMatrix(modelId_, AngleUtility::Multiplication(DIFF_ANGLES, angles_));
	MV1SetScale(modelId_, scales_);

	animationController_->Update();
	MV1SetupCollInfo(modelId_);
}

void Player::ChangeState(STATE state)
{
	switch (state)
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
	//モデルの描画
	MV1DrawModel(modelId_);

	if (power_ >= 3) {

		DrawSphere3D(MV1GetFramePosition(modelId_, 58), 10, 16, 0xff0000, 0xff0000, true);
	}
	else if (power_ >= 2) {

		DrawSphere3D(MV1GetFramePosition(modelId_, 58), 10, 16, 0xffffff, 0xffffff, true);
	}

	float x = Application::SCREEN_SIZE_X - 50.0f;
	float dx = x / MAX_HP;
	float d2x = x / MAX_STAMINA;
	x += 27.5f;
	
	dx *= hp_;
	dx += 25.0f;
	d2x *= stamina_;
	d2x += 25.0f;

	DrawBoxAA(22.5f, 17.5f, x, 57.5f, 0x222222, true);
	DrawBoxAA(25.0f, 20.0f, dx, 35.0f, 0x00ff00, true);
	DrawBoxAA(25.0f, 40.0f, d2x, 55.0f, 0xffff00, true);

	//デバック
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
}

void Player::Damage(int damage, float dir)
{
	hp_ -= damage;
	knockBackDir_ = dir;

	if (damage >= 2) {

		state_ = STATE::DAMAGED_HEAVY;
	}
	else {

		state_ = STATE::DAMAGED_LIGHT;
	}
	ChangeState(state_);

	if (hp_ <= 0) {

		hp_ = 0;
		overFlg_ = true;
	}
}

bool Player::IsCollisionState(void)
{
	return false;
}

bool Player::IsAttackMotion(void)
{
	if (state_ == STATE::ATTACK || state_ == STATE::COMBO) {

		return true;
	}
	else {

		return false;
	}
}

bool Player::IsHit(void)
{
	if (state_ != STATE::DAMAGED_LIGHT && state_ != STATE::DAMAGED_HEAVY) {

		return true;
	}
	else {

		return false;
	}
}

void Player::KnockBack()
{
	pos_.x += sinf(knockBackDir_) * 2.0f;
	pos_.z += cosf(knockBackDir_) * 2.0f;

	angles_.y = knockBackDir_ - DX_PI_F;
}

void Player::ChangeWait(void)
{
	//待機モーション
	animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
}

void Player::ChangeMove(void)
{
}

void Player::ChangeAttack(void)
{
	//攻撃モーション
	animationController_->Play(static_cast<int>(ANIM_TYPE::ATTACK), false);
	isAttack_ = true;
}

void Player::ChangeCombo(void)
{
	//攻撃モーション
	animationController_->Play(static_cast<int>(ANIM_TYPE::COMBO_1), false);
	isAttack_ = true;
}

void Player::ChangeDodge(void)
{
	animationController_->Play(static_cast<int>(ANIM_TYPE::DODGE), false);
	dodgeCnt_ = 0;
	stamina_-= DOGDE_STAMINA;
}

void Player::ChangeDamagedLight(void)
{
	animationController_->Play(static_cast<int>(ANIM_TYPE::DAMAGED_LIGHT), false);
}

void Player::ChangeDamagedHeavy(void)
{
	animationController_->Play(static_cast<int>(ANIM_TYPE::DAMAGED_HEAVY), false);
}

void Player::UpdateWait(void)
{
	if (GetJoypadNum() == 0) {
		if (CheckHitKey(KEY_INPUT_F) == 1) {
		
			//攻撃モーションに移行
			state_ = STATE::ATTACK;
		}
		if (CheckHitKey(KEY_INPUT_G) == 1) {
		
			//攻撃モーションに移行
			state_ = STATE::COMBO;
		}
		if (CheckHitKey(KEY_INPUT_W) == 1 || CheckHitKey(KEY_INPUT_A) == 1 || CheckHitKey(KEY_INPUT_S) == 1 || CheckHitKey(KEY_INPUT_D) == 1) {

			//移動モーションに移行
			state_ = STATE::MOVE;
		}
		if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
			if (stamina_ >= DOGDE_STAMINA) {

				//回避モーションに移行
				state_ = STATE::DOGDE;
			}
		}
	}
	else {

		Controller& ctrl = Controller::GetInstance();
		//ゲームパッドの情報を取得
		Controller::JOYPAD_IN_STATE padState = ctrl.GetJPadState(Controller::JOYPAD_NO::PAD1);
		
		if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::TOP)]) {

			//攻撃モーションに移行
			state_ = STATE::ATTACK;
		}		
		if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::RIGHT)]) {

			//攻撃モーションに移行
			state_ = STATE::COMBO;
		}
		if (padState.AKeyLX != 0 || padState.AKeyLY != 0) {

			//移動モーションに移行
			state_ = STATE::MOVE;
		}
		if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::DOWN)]) {
			if (stamina_ >= DOGDE_STAMINA) {

				//回避モーションに移行
				state_ = STATE::DOGDE;
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

				//移動させる
				pos_ = VAdd(pos_, VScale(moveDir_, speed_ * 1.75f));

				stamina_--;
			}
			else {

				//移動モーション
				animationController_->Play(static_cast<int>(ANIM_TYPE::WALK), true);

				//移動させる
				pos_ = VAdd(pos_, VScale(moveDir_, speed_));
			}
		}
		else {

			//移動モーション
			animationController_->Play(static_cast<int>(ANIM_TYPE::WALK), true);

			//移動させる
			pos_ = VAdd(pos_, VScale(moveDir_, speed_));
		}
		if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::TOP)]) {

			//攻撃モーションに移行
			state_ = STATE::ATTACK;
		}
		if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::RIGHT)]) {

			//攻撃モーションに移行
			state_ = STATE::COMBO;
		}
		if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::DOWN)]) {
			if (stamina_ >= DOGDE_STAMINA) {

				//回避モーションに移行
				state_ = STATE::DOGDE;
			}
		}
		angles_.y = atan2f(moveDir_.x, moveDir_.z);
	}
	else {

		state_ = STATE::WAIT;
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

		isAttack_ = true;
		power_ = 3;
	}
	//当たり判定の中心
	attackPos1_ = MV1GetFramePosition(modelId_, 58);
	attackPos2_ = VTransform(SWORD_POS, MV1GetFrameLocalWorldMatrix(modelId_, 37));

	if (animationController_->IsEnd()) {

		power_ = 1;
		isAttack_ = false;
		//待機モーションに移行
		state_ = STATE::WAIT;
	}
}

void Player::UpdateCombo(void)
{
	//当たり判定の中心
	attackPos1_ = MV1GetFramePosition(modelId_, 58);
	attackPos2_ = VTransform(SWORD_POS, MV1GetFrameLocalWorldMatrix(modelId_, 37));

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
			}
		}
	}
	if (animationController_->IsEnd()) {

		isAttack_ = false;
		//待機モーションに移行
		state_ = STATE::WAIT;
	}
}

void Player::UpdateDodge(void)
{
	if (animationController_->GetTime() <= 70.0f) {
	
		//移動させる
		pos_ = VAdd(pos_, VScale(moveDir_, speed_ * 1.1f));
		dodgeCnt_++;
		
		if (!dodgeFlg_) {
		
			dodgeFlg_ = true;
		}
	}
	else if (dodgeFlg_){

		dodgeFlg_ = false;
		dodgeCnt_ = 0;
	}

	if (animationController_->IsEnd()) {

		//待機モーションに移行
		state_ = STATE::WAIT;
	}
}

void Player::UpdateDamagedLight(void)
{
	if (animationController_->GetTime() >= 40) {

		state_ = STATE::WAIT;
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

			state_ = STATE::WAIT;
		}
	}
}
