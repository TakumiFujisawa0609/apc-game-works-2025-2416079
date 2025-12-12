#include "ActorBase.h"
#include "../../Utility/AngleUtility.h"
#include "../Common/AnimationController.h"


ActorBase::ActorBase() : angles_(), animationCtrl_(), hp_(), modelId_(), moveDir_(), pos_(), prevPos_()
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
	InitOwn();
	InitModel();
}

void ActorBase::InitModel() const
{
	//êŠ”X‚Ì‰Šú‰»
	MV1SetPosition(modelId_, pos_);
	MV1SetRotationMatrix(modelId_, AngleUtility::Multiplication(localAngles_, angles_));
	MV1SetScale(modelId_, scales_);

	//“–‚½‚è”»’è‚Ì‰Šú‰»
	MV1SetupCollInfo(modelId_);
}

void ActorBase::DrawModel() const
{
	MV1DrawModel(modelId_);
}

void ActorBase::Release() const
{
	if (animationCtrl_ != nullptr) {
	
		animationCtrl_->Release();
		delete animationCtrl_;
	}

	MV1DeleteModel(modelId_);
}
