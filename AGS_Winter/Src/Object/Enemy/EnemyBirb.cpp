#include <DxLib.h>
#include "EnemyBirb.h"
#include "../Common/AnimationController.h"


EnemyBirb::EnemyBirb()
{
}

EnemyBirb::~EnemyBirb()
{
}

void EnemyBirb::SetParam(void)
{
	scales_ = { 0.3f, 0.3f, 0.3f };
	speed_ = 1.8f;
	hp_ = 4;
	collisionRadius_ = 35.0f;
	collisionLocalPos_ = { 0.0f, 50.0f, 0.0f };
}

void EnemyBirb::ChangeStandby(void)
{
	animationController_->Play(static_cast<int>(ANIM_TYPE_A::WALK), true);
}

void EnemyBirb::ChangeHitReact(void)
{
	animationController_->Play(static_cast<int>(ANIM_TYPE_A::HIT_REACT), false);
}

void EnemyBirb::Move(void)
{
	pos_.x += speed_;
	// モデルの座標を設定する​
	MV1SetPosition(modelId_, pos_);
}
