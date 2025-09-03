#include "../../Application.h"
#include "../../Utility/AsoUtility.h"
#include "../Common/AnimationController.h"
#include "EnemyBase.h"


EnemyBase::EnemyBase(void)
{
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Init(TYPE type, int baseModelId)
{
	// エネミー種別
	type_ = type;
	// モデルのロード
	modelId_ = MV1DuplicateModel(baseModelId);

	// 角度の設定
	angles_ = { 0.0f, - DX_PI_F / 2.0f , 0.0f };
	// 位置の設定
	pos_ = { -350.0f, 30.0f, 75.0f };

	// 移動方向
	moveDir_ = { 1.0f, 0.0f, 0.0f };

	// パラメータ設定
	SetParam();
	// 大きさ
	MV1SetScale(modelId_, scales_);
	// 角度
	MV1SetRotationXYZ(modelId_, angles_);

	switch (type){
	case EnemyBase::TYPE::BIRB:
	
		// アニメーションの設定
		animationController_ = new AnimationController(modelId_);

		for (int i = 0; i < static_cast<int>(ANIM_TYPE_A::MAX); i++) {

			animationController_->AddInFbx(i, 30.0f, i);
		}
		// アニメーションの再生
		animationController_->Play(static_cast<int>(ANIM_TYPE_A::WALK), true);
		break;

	case EnemyBase::TYPE::BEE:

		// アニメーションの設定
		animationController_ = new AnimationController(modelId_);

		for (int i = 0; i < static_cast<int>(ANIM_TYPE_B::MAX); i++) {

			animationController_->AddInFbx(i, 30.0f, i);
		}
		// アニメーションの再生
		animationController_->Play(static_cast<int>(ANIM_TYPE_B::FLY_MOVE), true);
		break;
	}
	
	// 初期状態
	ChangeState(STATE::STANDBY);
}

void EnemyBase::Update(void)
{
	switch (state_){
	case EnemyBase::STATE::STANDBY:

		UpdateStandby();
		break;

	case EnemyBase::STATE::DEAD_REACT:

		UpdateDeadReact();
		break;

	case EnemyBase::STATE::HIT_REACT:

		UpdateHitReact();
		break;

	case EnemyBase::STATE::END:

		UpdateEnd();
		break;
	}
	// アニメーションの更新
	animationController_->Update();
}

void EnemyBase::UpdateStandby(void)
{
	// プレイヤーの方向へ移動
	Move();
}

void EnemyBase::UpdateHitReact(void)
{
	if (animationController_->IsEnd()){

		ChangeState(STATE::STANDBY);
	}
}

void EnemyBase::UpdateDeadReact(void)
{
	if (animationController_->IsEnd()){

		ChangeState(STATE::END);
	}
}

void EnemyBase::UpdateEnd(void)
{
}

void EnemyBase::ChangeState(STATE state)
{
	state_ = state;

	switch (state_) {
	case EnemyBase::STATE::STANDBY:

		ChangeStandby();
		break;

	case EnemyBase::STATE::DEAD_REACT:

		ChangeDeadReact();
		break;
		
	case EnemyBase::STATE::HIT_REACT:

		ChangeHitReact();
		break;

	case EnemyBase::STATE::END:

		ChangeEnd();
		break;
	}
}

void EnemyBase::ChangeDeadReact(void)
{
	animationController_->Play(static_cast<int>(ANIM_TYPE_A::DEATH), false);
}

void EnemyBase::ChangeEnd(void)
{
}

void EnemyBase::Draw(void)
{
	switch (state_){
	case EnemyBase::STATE::STANDBY:

		DrawStandby();
		break;

	case EnemyBase::STATE::DEAD_REACT:

		DrawDeadReact();
		break;

	case EnemyBase::STATE::HIT_REACT:

		DrawHitReact();
		break;

	case EnemyBase::STATE::END:

		DrawEnd();
		break;
	}
}

void EnemyBase::Release(void)
{
	// モデルの解放
	MV1DeleteModel(modelId_);
	// アニメーションコントローラの解放
	animationController_->Release();
	delete animationController_;
}

void EnemyBase::DrawStandby(void)
{
	// モデルの描画
	MV1DrawModel(modelId_);
	//DrawSphere3D(GetCollisionPos(), collisionRadius_, 16, 0x0000ff, 0x0000ff, false);
}

void EnemyBase::DrawDeadReact(void)
{
	MV1DrawModel(modelId_);
}

void EnemyBase::DrawHitReact(void)
{
	MV1DrawModel(modelId_);
}

void EnemyBase::DrawEnd(void)
{
}

VECTOR EnemyBase::GetCollisionPos(void)
{
	return VAdd(pos_, collisionLocalPos_);
}

void EnemyBase::Damage(int damage)
{
	hp_ -= damage;

	if (hp_ <= 0){

		ChangeState(STATE::DEAD_REACT);
	}
	else {

		ChangeState(STATE::HIT_REACT);
	}
}

bool EnemyBase::IsCollisionState(void)
{
	if (state_ == STATE::STANDBY) {

		return true;
	}
	return false;
}

bool EnemyBase::IsDead(void)
{
	if (state_ == STATE::DEAD_REACT) {

		return true;
	}
	return false;
}