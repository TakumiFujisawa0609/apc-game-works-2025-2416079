#include "Player.h"
#include "../Application.h"


Player::Player(void)
{
}

Player::~Player(void)
{
}

void Player::Init()
{
	modelId_ = MV1LoadModel((Application::PATH_MODEL + "Walking.mv1").c_str());

	pos_ = { 0.0f, 0.0f, 0.0f };
	angles_ = VAdd({ 0.0f, 0.0f, 0.0f }, DIFF_ANGLES);
	scales_ = { 1.0f, 1.0f, 1.0f };

	MV1SetPosition(modelId_, pos_);
	MV1SetRotationXYZ(modelId_, angles_);
	MV1SetScale(modelId_, scales_);
}

void Player::Update(void)
{
}

void Player::ChangeState(STATE state)
{
}

void Player::Draw(void)
{
	MV1DrawModel(modelId_);
}

void Player::Release(void)
{
	MV1DeleteModel(modelId_);
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

void Player::ChangeStandby(void)
{
}

void Player::ChangeDeadReact(void)
{
}

void Player::ChangeHitReact(void)
{
}

void Player::ChangeEnd(void)
{
}

void Player::UpdateStandby(void)
{
}

void Player::UpdateHitReact(void)
{
}

void Player::UpdateDeadReact(void)
{
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

void Player::Move(void)
{
}
