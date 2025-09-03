#include <DxLib.h>
#include "EnemyBee.h"
#include "../Common/AnimationController.h"


EnemyBee::EnemyBee()
{
}

EnemyBee::~EnemyBee()
{
}

void EnemyBee::SetParam(void)
{
    scales_ = { 0.3f, 0.3f, 0.3f };
    speed_ = 1.5f;
    hp_ = 3; 
    collisionRadius_ = 35.0f;
    collisionLocalPos_ = { 0.0f, 50.0f, 0.0f };
    speedY_ = 0.0f;
}

void EnemyBee::ChangeStandby(void)
{
    animationController_->Play(static_cast<int>(ANIM_TYPE_B::FLY_MOVE), true);
}

void EnemyBee::ChangeHitReact(void)
{
    animationController_->Play(static_cast<int>(ANIM_TYPE_B::HIT_REACT), false);
}

void EnemyBee::Move(void)
{
    pos_.x += speed_;
    pos_.y += sinf(speedY_ * DX_PI_F) * 1.75f;
    speedY_ += 0.01f;
    MV1SetPosition(modelId_, pos_);
}