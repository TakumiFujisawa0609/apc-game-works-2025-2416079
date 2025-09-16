#include "Player.h"
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "Common/AnimationController.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"


Player::Player(void)
{
}

Player::~Player(void)
{
}

void Player::Init()
{
	modelId_ = MV1LoadModel((Application::PATH_MODEL + "Walking.mv1").c_str());
	bladeModel_ = MV1LoadModel((Application::PATH_MODEL + "blade.mv1").c_str());

	pos_ = DEFAULT_POS;
	angles_ = VAdd({ 0.0f, 0.0f, 0.0f }, DIFF_ANGLES);
	scales_ = { 1.0f, 1.0f, 1.0f };

	speed_ = 5.0f;

	state_ = STATE::WAIT;

	MV1SetPosition(modelId_, pos_);
	MV1SetRotationXYZ(modelId_, angles_);
	MV1SetScale(modelId_, scales_);

	animationController_ = new AnimationController(modelId_);

	for (int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++) {
		if (i <= 1) {
			
			animationController_->AddInFbx(i, 60.0f, i);
		}
		else {

			animationController_->Add(i, 60.0f, (Application::PATH_ANIMATION + "Sword And Shield Slash.mv1").c_str());
		}
	}
}

void Player::Update(void)
{
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
	}

	MV1SetPosition(modelId_, pos_);
	MV1SetRotationXYZ(modelId_, angles_);
	MV1SetScale(modelId_, scales_);

	animationController_->Update();
}

void Player::ChangeState(STATE state)
{
}

void Player::Draw(void)
{
	MV1DrawModel(modelId_);
	MV1DrawModel(bladeModel_);
	DrawSphere3D(pos, 25.0f, 16, 0x00ff00, 0x00ff00, true);

}

void Player::Release(void)
{
	animationController_->Release();
	delete animationController_;

	MV1DeleteModel(modelId_);
	MV1DeleteModel(bladeModel_);
}

VECTOR Player::GetCollisionPos(void)
{
	return VECTOR();
}

void Player::Damage(int damage)
{
}

bool Player::IsCollisionState(void)
{
	return false;
}

bool Player::IsDead(void)
{
	return false;
}

void Player::ChangeWait(void)
{
	animationController_->Play(static_cast<int>(ANIM_TYPE::T), true);

	state_ = STATE::WAIT;
}

void Player::ChangeMove(void)
{
	animationController_->Play(static_cast<int>(ANIM_TYPE::WALK), true);

	state_ = STATE::MOVE;
}

void Player::ChangeAttack(void)
{
	animationController_->Play(static_cast<int>(ANIM_TYPE::ATTACK), false);

	state_ = STATE::ATTACK;
}

void Player::ChangeEnd(void)
{
}

void Player::UpdateWait(void)
{
	if (CheckHitKey(KEY_INPUT_SPACE) == 1) {

		ChangeAttack();
	}
	if (CheckHitKey(KEY_INPUT_W) == 1 || CheckHitKey(KEY_INPUT_A) == 1 || CheckHitKey(KEY_INPUT_S) == 1 || CheckHitKey(KEY_INPUT_D) == 1) {

		ChangeMove();
	}
}

void Player::UpdateMove(void)
{
	MATRIX mat = MGetIdent();
	VECTOR pos = { 0.0f, 0.0f, 0.0f };

	if (CheckHitKey(KEY_INPUT_W) == 1 || CheckHitKey(KEY_INPUT_A) == 1 || CheckHitKey(KEY_INPUT_S) == 1 || CheckHitKey(KEY_INPUT_D) == 1) {

		Camera* camera = SceneManager::GetInstance().GetCamera();
		mat = MMult(mat, MGetRotY(camera->GetCameraAngles().y));
	}
	else {

		ChangeWait();
		return;
	}

	if (CheckHitKey(KEY_INPUT_W) == 1) {

		pos = VAdd(pos, VTransform({ 0.0f, 0.0f, 1.0f }, mat));
	}
	if (CheckHitKey(KEY_INPUT_S) == 1) {

		pos = VAdd(pos, VTransform({ 0.0f, 0.0f, -1.0f }, mat));
	}
	if (CheckHitKey(KEY_INPUT_D) == 1) {

		pos = VAdd(pos, VTransform({ 1.0f, 0.0f, 0.0f }, mat));
	}
	if (CheckHitKey(KEY_INPUT_A) == 1) {

		pos = VAdd(pos, VTransform({ -1.0f, 0.0f, 0.0f }, mat));
	}

	animationController_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
	pos = VNorm(pos);
	pos_ = VAdd(pos_, VScale(pos, speed_));
	angles_.y = atan2f(pos.x, pos.z) + DX_PI_F;
}

void Player::UpdateAttack(void)
{
	//“–‚½‚è”»’è‚Ì’†S
	pos = MV1GetFramePosition(modelId_, 46);

	
	if (animationController_->IsEnd()) {

		ChangeWait();
	}
}

void Player::UpdateEnd(void)
{
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