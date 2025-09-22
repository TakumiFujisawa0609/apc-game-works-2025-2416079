#include "../../Application.h"
#include "../../Utility/AngleUtility.h"
#include "../Common/AnimationController.h"
#include "../Player.h"
#include "EnemyBase.h"


EnemyBase::EnemyBase(Player* pl)
{
	player_ = pl;
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Init()
{
	modelId_ = MV1LoadModel((Application::PATH_MODEL + "Wolf.mv1").c_str());

	pos_ = DEFAULT_POS;
	angles_ = { 0.0f, 0.0f, 0.0f };
	scales_ = { 10.0f, 10.0f ,10.0f };

	MV1SetPosition(modelId_, pos_);
	MV1SetRotationMatrix(modelId_, AngleUtility::Multiplication(DIFF_ANGLES, angles_));
	MV1SetScale(modelId_, scales_);

	MV1SetupCollInfo(modelId_);
}

void EnemyBase::Update(void)
{
	LookPlayer();
}

void EnemyBase::ChangeState(STATE state)
{
}

void EnemyBase::Draw(void)
{
	MV1DrawModel(modelId_);
}

void EnemyBase::Release(void)
{
	MV1DeleteModel(modelId_);
}

void EnemyBase::LookPlayer(void)
{
	VECTOR dir = VSub(player_->GetPos(), pos_);
	angles_.y = atan2f(dir.x, dir.z);

	MV1SetRotationMatrix(modelId_, AngleUtility::Multiplication(DIFF_ANGLES, angles_));
	MV1RefreshCollInfo(modelId_);
}
