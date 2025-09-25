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

	speed_ = 5.0f;

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

			animationController_->Add(i, 60.0f, (Application::PATH_ANIMATION + "Slash.mv1").c_str());
			break;	
	
		case 4:

			animationController_->Add(i, 60.0f, (Application::PATH_ANIMATION + "Slash_1.mv1").c_str());
			break;	
	
		case 5:

			animationController_->Add(i, 60.0f, (Application::PATH_ANIMATION + "Slash_2.mv1").c_str());
			break;	
			
		case 6:

			animationController_->Add(i, 60.0f, (Application::PATH_ANIMATION + "Slash_3.mv1").c_str());
			break;	

		case 7:

			animationController_->Add(i, 60.0f, (Application::PATH_ANIMATION + "Dodge.mv1").c_str());
			break;
		}
	}

	ChangeWait();
}

void Player::Update(void)
{
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
	}
	//モデルの設定
	MV1SetPosition(modelId_, pos_);
	MV1SetRotationMatrix(modelId_, AngleUtility::Multiplication(DIFF_ANGLES, angles_));
	MV1SetScale(modelId_, scales_);

	animationController_->Update();
}

void Player::ChangeState(STATE state)
{
}

void Player::Draw(void)
{
	//モデルの描画
	MV1DrawModel(modelId_);
	DrawCapsule3D(attackPos1_, attackPos2_, 10.0f, 16, 0x00ffff, 0x00ffff, false);
	//DrawSphere3D(pos, 25.0f, 16, 0x00ff00, 0x00ff00, true);
	Controller& ctrl = Controller::GetInstance();
	//ゲームパッドの情報を取得
	Controller::JOYPAD_IN_STATE padState = ctrl.GetJPadInputState(Controller::JOYPAD_NO::PAD1);
	for (int i = 0; i < static_cast<int>(Controller::JOYPAD_BTN::MAX); i++) {
		DrawFormatString(0, 20 * i, 0x000000, "%d", padState.ButtonsNew[i]);
	}
	DrawFormatString(50, 15, 0xfffff , "%.2f", animationController_->GetTime());
}

void Player::Release(void)
{
	//アニメーションコントローラーのリリース
	animationController_->Release();
	delete animationController_;

	//モデルの削除
	MV1DeleteModel(modelId_);
}

void Player::Damage(int damage)
{
}

bool Player::IsCollisionState(void)
{
	return false;
}

bool Player::IsAttack(void)
{
	if (state_ == STATE::ATTACK || state_ == STATE::COMBO) {

		return true;
	}
	else {

		return false;
	}
}

void Player::ChangeWait(void)
{
	//待機モーション
	animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);

	state_ = STATE::WAIT;
}

void Player::ChangeMove(void)
{
	state_ = STATE::MOVE;
}

void Player::ChangeAttack(void)
{
	//攻撃モーション
	animationController_->Play(static_cast<int>(ANIM_TYPE::ATTACK), false);

	state_ = STATE::ATTACK;
}

void Player::ChangeCombo(void)
{
	//攻撃モーション
	animationController_->Play(static_cast<int>(ANIM_TYPE::COMBO_1), false);

	state_ = STATE::COMBO;
}

void Player::ChangeDodge(void)
{
	animationController_->Play(static_cast<int>(ANIM_TYPE::DODGE), false);

	state_ = STATE::DOGDE;
}

void Player::UpdateWait(void)
{
	if (GetJoypadNum() == 0) {
		if (CheckHitKey(KEY_INPUT_F) == 1) {
		
			//攻撃モーションに移行
			ChangeAttack();
		}
		if (CheckHitKey(KEY_INPUT_G) == 1) {
		
			//攻撃モーションに移行
			ChangeCombo();
		}
		if (CheckHitKey(KEY_INPUT_W) == 1 || CheckHitKey(KEY_INPUT_A) == 1 || CheckHitKey(KEY_INPUT_S) == 1 || CheckHitKey(KEY_INPUT_D) == 1) {

			//移動モーションに移行
			ChangeMove();
		}
		if (CheckHitKey(KEY_INPUT_SPACE) == 1) {

			//回避モーションに移行
			ChangeDodge();
		}
	}
	else {

		Controller& ctrl = Controller::GetInstance();
		//ゲームパッドの情報を取得
		Controller::JOYPAD_IN_STATE padState = ctrl.GetJPadInputState(Controller::JOYPAD_NO::PAD1);
		
		if (padState.ButtonsNew[static_cast<int>(Controller::JOYPAD_BTN::TOP)]) {

			//攻撃モーションに移行
			ChangeAttack();
		}		
		if (padState.ButtonsNew[static_cast<int>(Controller::JOYPAD_BTN::RIGHT)]) {

			//攻撃モーションに移行
			ChangeCombo();
		}
		if (padState.AKeyLX != 0 || padState.AKeyLY != 0) {

			//移動モーションに移行
			ChangeMove();
		}
		if (padState.ButtonsNew[static_cast<int>(Controller::JOYPAD_BTN::DOWN)]) {

			//回避モーションに移行
			ChangeDodge();
		}
	}
}

void Player::UpdateMove(void)
{
	Controller& ctrl = Controller::GetInstance();
	//ゲームパッドの情報を取得
	Controller::JOYPAD_IN_STATE padState = ctrl.GetJPadInputState(Controller::JOYPAD_NO::PAD1);

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

		if (padState.ButtonsNew[static_cast<int>(Controller::JOYPAD_BTN::R)]) {

			//移動モーション
			animationController_->Play(static_cast<int>(ANIM_TYPE::RUN), true);

			//移動させる
			pos_ = VAdd(pos_, VScale(moveDir_, speed_ * 1.5f));
		}
		else {

			//移動モーション
			animationController_->Play(static_cast<int>(ANIM_TYPE::WALK), true);

			//移動させる
			pos_ = VAdd(pos_, VScale(moveDir_, speed_));

			if (padState.ButtonsNew[static_cast<int>(Controller::JOYPAD_BTN::TOP)]) {

				//攻撃モーションに移行
				ChangeAttack();
			}
			if (padState.ButtonsNew[static_cast<int>(Controller::JOYPAD_BTN::RIGHT)]) {

				//攻撃モーションに移行
				ChangeCombo();
			}
		}
		if (padState.ButtonsNew[static_cast<int>(Controller::JOYPAD_BTN::DOWN)]) {

			//回避モーションに移行
			ChangeDodge();
		}
		angles_.y = atan2f(moveDir_.x, moveDir_.z);
	}
	else {

		ChangeWait();
	}
}

void Player::UpdateAttack(void)
{
	if (animationController_->GetTime() <= 138.0f) {
	
		//移動させる
		pos_ = VAdd(pos_, VScale(moveDir_, speed_ * 0.2f));
	}
	//当たり判定の中心
	attackPos1_ = MV1GetFramePosition(modelId_, 58);
	attackPos2_ = VTransform(SWORD_POS, MV1GetFrameLocalWorldMatrix(modelId_, 37));

	if (animationController_->IsEnd()) {

		//待機モーションに移行
		ChangeWait();
	}
}

void Player::UpdateCombo(void)
{
	//当たり判定の中心
	attackPos1_ = MV1GetFramePosition(modelId_, 58);
	attackPos2_ = VTransform(SWORD_POS, MV1GetFrameLocalWorldMatrix(modelId_, 37));

	Controller& ctrl = Controller::GetInstance();
	//ゲームパッドの情報を取得
	Controller::JOYPAD_IN_STATE padState = ctrl.GetJPadInputState(Controller::JOYPAD_NO::PAD1);

	if (animationController_->GetPlayType() == static_cast<int>(ANIM_TYPE::COMBO_1)) {
		if (animationController_->GetTime() >= 67.5f) {
			if (padState.ButtonsNew[static_cast<int>(Controller::JOYPAD_BTN::RIGHT)]) {

				animationController_->Play(static_cast<int>(ANIM_TYPE::COMBO_2), false);
			}
		}
	}
	if (animationController_->GetPlayType() == static_cast<int>(ANIM_TYPE::COMBO_2)) {
		if (animationController_->GetTime() >= 75.0f) {
			if (padState.ButtonsNew[static_cast<int>(Controller::JOYPAD_BTN::RIGHT)]) {

				animationController_->Play(static_cast<int>(ANIM_TYPE::COMBO_3), false);
			}
		}
	}
	if (animationController_->IsEnd()) {

		//待機モーションに移行
		ChangeWait();
	}
}

void Player::UpdateDodge(void)
{
	if (animationController_->GetTime() <= 83.0f) {
	
		//移動させる
		pos_ = VAdd(pos_, VScale(moveDir_, speed_ * 0.9f));
	}
	if (animationController_->IsEnd()) {

		//待機モーションに移行
		ChangeWait();
	}
}

void Player::DrawStandby(void)
{
}

void Player::DrawDeadReact(void)
{
}

void Player::DrawHitReact(void)
{
}

void Player::DrawEnd(void)
{
}