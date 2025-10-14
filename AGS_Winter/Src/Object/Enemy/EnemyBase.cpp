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

	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::WALK), 30, 5);

	pos_ = DEFAULT_POS;
	angles_ = { 0.0f, 0.0f, 0.0f };
	scales_ = { 8.0f, 8.0f ,8.0f };

	hp_ = 20;
	clearFlg_ = false;

	attack_ = 0;
	coolDown_ = 0;
	isCoolDown_ = false;

	cnt_ = 0;
	attackSpeed_ = 22.0f;
	attackAFlg_ = attackBFlg_ = false;
	attackPos_ = attackPos1_ = attackPos2_ = ATTACK_POS_A;

	MV1SetPosition(modelId_, pos_);
	MV1SetRotationMatrix(modelId_, AngleUtility::Multiplication(DIFF_ANGLES, angles_));
	MV1SetScale(modelId_, scales_);

	LookPlayer();

	MV1SetupCollInfo(modelId_);
}

void EnemyBase::Update(void)
{
	animationController_->Update();

	if (!attackAFlg_ && !attackBFlg_ && !isCoolDown_) {

		VECTOR prevAngles = angles_;
		LookPlayer();

		if (std::abs(prevAngles.y - angles_.y) >= 0.0025f) {
		
			animationController_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
		}
		else {
			
			animationController_->Play(static_cast<int>(ANIM_TYPE::T), true);
		}

		cnt_++;

		if (cnt_ > 300) {

			cnt_ = 0;
			animationController_->Play(static_cast<int>(ANIM_TYPE::ATTACK), true);
			
			if (VSize(VSub(player_->GetPos(), pos_)) >= 800.0f) {

				attackAFlg_ = true;

				attackPos_ = VAdd(pos_, VTransform(ATTACK_POS_A, AngleUtility::GetMatrixRotateXYZ(angles_)));
				attackDir_ = VSub(VAdd(player_->GetPos(), { 0.0f, 80.0f, 0.0f }), attackPos_);
				attackDir_ = VNorm(attackDir_);
			}
			else {

				attackBFlg_ = true;

				attackPos_ = VAdd(pos_, VTransform(ATTACK_POS_B, AngleUtility::GetMatrixRotateXYZ(angles_)));
				attackPos1_ = VAdd(pos_, VTransform(VAdd(ATTACK_POS_B, { -400.0f, 0.0f, 250.0f }), AngleUtility::GetMatrixRotateXYZ(angles_)));
				attackPos2_ = VAdd(pos_, VTransform(VAdd(ATTACK_POS_B, { 400.0f, 0.0f, 250.0f }), AngleUtility::GetMatrixRotateXYZ(angles_)));
			}
		}
	}
	if (attackAFlg_){

		attackPos_ = VAdd(attackPos_, VScale(attackDir_, attackSpeed_));
		//attackPos1_ = VAdd(pos_, VTransform(ATTACK_POS_A, AngleUtility::GetMatrixRotateXYZ(angles_)));
		//attackPos2_ = VAdd(attackPos1_, attackDir_);
		if (attackPos_.y < ATTACK_RADIUS) {

			attackAFlg_ = false;
		}
	}
	if (attackBFlg_){

		attack_++;

		if (attack_ >= 25) {
		
			attack_ = 0;
			attackBFlg_ = false;
			isCoolDown_ = true;
		}
	}
	if(isCoolDown_) {

		coolDown_++;

		if (coolDown_ >= 120) {

			isCoolDown_ = false;
			coolDown_ = 0;
		}
	}
	if (hp_ <= 0) {

		clearFlg_ = true;
	}
	MV1RefreshCollInfo(modelId_);
}

void EnemyBase::ChangeState(STATE state)
{
}

void EnemyBase::Draw(void)
{
	MV1DrawModel(modelId_);
	DrawFormatString(Application::SCREEN_SIZE_X - 100, 20, 0x000000, "%.2f", (300.0f - cnt_) / 60.0f, SetFontSize(25));
	DrawFormatString(100, 20, 0x000000, "%.d", hp_, SetFontSize(25));

	if (attackAFlg_) {
	
		DrawSphere3D(attackPos_, ATTACK_RADIUS, 16, 0xffaa55, 0xffaa55, true);
		//DrawCapsule3D(attackPos1_, attackPos2_, 10.0f, 16, 0x00ffff, 0x00ffff, true);
	}
	if (attackBFlg_) {

		DrawTriangle3D(attackPos_, attackPos1_, attackPos2_, 0xff55aa, true);
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
}
