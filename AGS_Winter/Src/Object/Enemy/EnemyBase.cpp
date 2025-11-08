#include <cmath>
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

	animationController_ = new AnimationController(modelId_);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::WALK), 60.0f, 5);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::WALK), 60.0f, 5);

	pos_ = DEFAULT_POS;
	angles_ = { 0.0f, 0.0f, 0.0f };
	scales_ = { 10.0f, 10.0f ,10.0f };

	hp_ = 20;
	clearFlg_ = false;

	cnt_ = 0;
	attackAFlg_ = false;
	attackPos_ = attackPos1_ = attackPos2_ = ATTACK_POS;
	attackSpeed_ = 0.025f;

	MV1SetPosition(modelId_, pos_);
	MV1SetRotationMatrix(modelId_, AngleUtility::Multiplication(DIFF_ANGLES, angles_));
	MV1SetScale(modelId_, scales_);

	LookPlayer();

	MV1SetupCollInfo(modelId_);
}

void EnemyBase::Update(void)
{
	animationController_->Update();

	if (!attackAFlg_) {
		if (cnt_ < SceneManager::GetInstance().GetDeltaTime() * 180) {

			cnt_ += SceneManager::GetInstance().GetDeltaTime();
		}
		else {

			cnt_ = 0;
			attackAFlg_ = true;
			attackPos_ = VAdd(pos_, VTransform(ATTACK_POS, AngleUtility::GetMatrixRotateXYZ(angles_)));
			//attackPos1_ = VAdd(pos_, VTransform(ATTACK_POS, AngleUtility::GetMatrixRotateXYZ(angles_)));
			attackDir_ = VSub(VAdd(player_->GetPos(), { 0.0f, 80.0f, 0.0f }), attackPos_);
		}
		VECTOR prevAngles = angles_;
		LookPlayer();
		
		if (std::abs(angles_.y - prevAngles.y) > 0.001f) {

			animationController_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
		}
		else {

			animationController_->Play(static_cast<int>(ANIM_TYPE::T), true);
		}
	}
	else {

		animationController_->Play(static_cast<int>(ANIM_TYPE::T), true);

		attackPos_ = VAdd(attackPos_, VScale(attackDir_, attackSpeed_));
		//attackPos1_ = VAdd(pos_, VTransform(ATTACK_POS, AngleUtility::GetMatrixRotateXYZ(angles_)));
		//attackPos2_ = VAdd(attackPos1_, attackDir_);
		if (attackPos_.y < -30.0f) {

			attackAFlg_ = false;
		}
	}
	if (hp_ <= 0) {

		clearFlg_ = true;
	}
}

void EnemyBase::ChangeState(STATE state)
{
}

void EnemyBase::Draw(void)
{
	MV1DrawModel(modelId_);
	DrawFormatString(Application::SCREEN_SIZE_X - 100, 20, 0x000000, "%.0f", 3.0f - cnt_, SetFontSize(25));

	if (attackAFlg_) {
	
		DrawSphere3D(attackPos_, 30.0f, 16, 0xff0000, 0xff0000, true);
		//DrawCapsule3D(attackPos1_, attackPos2_, 10.0f, 16, 0x00ffff, 0x00ffff, true);
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
