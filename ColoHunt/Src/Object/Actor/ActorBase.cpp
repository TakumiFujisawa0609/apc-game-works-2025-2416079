#include "ActorBase.h"
#include "../../Utility/AngleUtility.h"
#include "../Common/AnimationController.h"


ActorBase::ActorBase() : transform_(), animationCtrl_(), hp_(), moveDir_()
{
}

ActorBase::~ActorBase()
{
}

void ActorBase::InitAll()
{
	InitLoad();
	Init();
}

void ActorBase::Init()
{
	InitAnim();
	InitTransform();
	transform_.Update();
	InitCollider();
	MV1SetupCollInfo(transform_.modelId);
}

void ActorBase::DrawModel() const
{
	MV1DrawModel(transform_.modelId);
}

void ActorBase::Release() const
{
	if (animationCtrl_ != nullptr) {
	
		animationCtrl_->Release();
		delete animationCtrl_;
	}

	MV1DeleteModel(transform_.modelId);
}