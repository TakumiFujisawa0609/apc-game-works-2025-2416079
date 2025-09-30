#include "../../Application.h"
#include "../../Utility/AngleUtility.h"
#include "../Common/AnimationController.h"
#include "../../Manager/SceneManager.h"
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

	cnt_ = 0;
	attackFlg_ = false;
	attackPos1_ = attackPos2_ = ATTACK_POS;

	MV1SetPosition(modelId_, pos_);
	MV1SetRotationMatrix(modelId_, AngleUtility::Multiplication(DIFF_ANGLES, angles_));
	MV1SetScale(modelId_, scales_);

	LookPlayer();

	MV1SetupCollInfo(modelId_);
}

void EnemyBase::Update(void)
{
	if (cnt_ < SceneManager::GetInstance().GetDeltaTime() * 144) {

		cnt_ += SceneManager::GetInstance().GetDeltaTime();
	}
	else {

		cnt_ = 0;
		attackFlg_ = !attackFlg_;
		attackPos1_ = VAdd(pos_, VTransform(ATTACK_POS, AngleUtility::GetMatrixRotateXYZ(angles_)));
		attackDir_ = VSub(player_->GetPos(), attackPos1_);
	}

	if (attackFlg_) {

		attackPos1_ = VAdd(pos_, VTransform(ATTACK_POS, AngleUtility::GetMatrixRotateXYZ(angles_)));
		attackPos2_ = VAdd(attackPos1_, attackDir_);
	}
	else {
		
		LookPlayer();
	}
}

void EnemyBase::ChangeState(STATE state)
{
}

void EnemyBase::Draw(void)
{
	MV1DrawModel(modelId_);
	DrawFormatString(Application::SCREEN_SIZE_X - 100, 20, 0x000000, "%f", cnt_);
	if (attackFlg_) {
	
		DrawCapsule3D(attackPos1_, attackPos2_, 10.0f, 16, 0x00ffff, 0x00ffff, false);
	}
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
