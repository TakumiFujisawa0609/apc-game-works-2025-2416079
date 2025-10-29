#include <cmath>
#include "../../Application.h"
#include "../../Utility/Utility.h"
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

	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::ATTACK_A), 30, 0);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::ATTACK_B), 30, 1);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::ATTACK_C), 30, 2);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::IDLE), 30, 3);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::WALK), 30, 6);

	pos_ = DEFAULT_POS;
	angles_ = { 0.0f, 0.0f, 0.0f };
	moveDir_ = Utility::VECTOR_ZERO;
	scales_ = { 8.0f, 8.0f ,8.0f };

	hp_ = 25;
	clearFlg_ = false;

	attack_ = 0;
	coolDown_ = 0;
	isCoolDown_ = false;

	cnt_ = 0;
	attackSpeed_ = 22.0f;
	attackAFlg_ = attackBFlg_ = false;
	attackPos2_ = attackPos1_ = Utility::VECTOR_ZERO;

	MV1SetPosition(modelId_, pos_);
	MV1SetRotationMatrix(modelId_, AngleUtility::Multiplication(DIFF_ANGLES, angles_));
	MV1SetScale(modelId_, scales_);

	DirectionPlayer();

	MV1SetupCollInfo(modelId_);
}

void EnemyBase::Update(void)
{
	//前のステータスを持っておく
	STATE prevState = state_;

	//ステータス別の更新
	switch (state_)
	{
	case EnemyBase::STATE::WAIT:
		
		UpdateWait();
		break;

	case EnemyBase::STATE::MOVE:
		
		UpdateMove();
		break;

	case EnemyBase::STATE::ATTACK:
		
		UpdateAttack();
		break;

	case EnemyBase::STATE::ESCAPE:
		
		UpdateEscape();
		break;
	}
	//モデルの更新
	animationController_->Update();
	MV1SetPosition(modelId_, pos_);

	//ステータスが変わっていれば変える
	if (prevState != state_) {

		ChangeState(state_);
	}

	//HPがゼロならクリア
	if (hp_ <= 0) {

		clearFlg_ = true;
	}
	//当たり判定を更新
	MV1RefreshCollInfo(modelId_);
}

void EnemyBase::ChangeState(STATE state)
{
	//ステータス変更時の初期化
	switch (state)
	{
	case EnemyBase::STATE::WAIT:

		ChangeWait();
		break;

	case EnemyBase::STATE::MOVE:

		ChangeMove();
		break;

	case EnemyBase::STATE::ATTACK:
		
		ChangeAttack();
		break;

	case EnemyBase::STATE::ESCAPE:
		
		ChangeEscape();
		break;
	}
}

void EnemyBase::Draw(void)
{
	MV1DrawModel(modelId_);
	DrawFormatString(Application::SCREEN_SIZE_X - 100, 20, 0x000000, "%.2f", (300.0f - cnt_) / 60.0f, SetFontSize(25));
	//DrawFormatString(100, 20, 0x000000, "%.2f", angles_.y, SetFontSize(25));

	if (attackAFlg_ && attackShowFlg_) {
	
		DrawSphere3D(attackPos1_, ATTACK_RADIUS, 16, 0xffaa55, 0xffaa55, true);
		//DrawCapsule3D(attackPos1_, attackPos2_, 10.0f, 16, 0x00ffff, 0x00ffff, true);
	}
	if (attackBFlg_) {

		DrawCapsule3D(attackPos1_, attackPos2_, ATTACK_RADIUS, 16, 0xff0000, 0xff0000, false);
	}
	if (attackCFlg_) {

		DrawCapsule3D(attackPos1_, attackPos2_, ATTACK_RADIUS * 2, 16, 0xff0000, 0xff0000, false);
	}
}

void EnemyBase::Release(void)
{
	MV1DeleteModel(modelId_);
}

bool EnemyBase::IsAttack(void)
{
	if (attackAFlg_ || attackBFlg_ || attackCFlg_) {

		return true;
	}
	else {

		return false;
	}
}

void EnemyBase::DirectionPlayer(void)
{
	VECTOR dir = VSub(player_->GetPos(), pos_);
	angles_.y = atan2f(dir.x, dir.z);

	MV1SetRotationMatrix(modelId_, AngleUtility::Multiplication(DIFF_ANGLES, angles_));
}

void EnemyBase::ChangeWait(void)
{
}

void EnemyBase::ChangeMove(void)
{
	angles_.y = AngleUtility::Deg2RadF(GetRand(360));

	//進める方向の更新
	moveDir_.x = sinf(angles_.y);
	moveDir_.z = cosf(angles_.y);
	MV1SetRotationMatrix(modelId_, AngleUtility::Multiplication(DIFF_ANGLES, angles_));

	animationController_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
}

void EnemyBase::ChangeAttack(void)
{
	if (VSize(VSub(player_->GetPos(), pos_)) >= 800.0f) {

		animationController_->Play(static_cast<int>(ANIM_TYPE::ATTACK_A), false);

		attackAFlg_ = true;
	}
	else {
		if (GetRand(1) == 1) {

			animationController_->Play(static_cast<int>(ANIM_TYPE::ATTACK_B), false);

			attackBFlg_ = true;
		}
		else {

			animationController_->Play(static_cast<int>(ANIM_TYPE::ATTACK_C), false);

			attackPos1_ = attackPos2_ = { 0.0f, -10000.0f, 0.0f };

			attackCFlg_ = true;
		}
	}
}

void EnemyBase::ChangeEscape(void)
{
}

void EnemyBase::UpdateWait(void)
{
	VECTOR prevAngles = angles_;
	DirectionPlayer();

	if (std::abs(prevAngles.y - angles_.y) >= 0.00375f) {

		animationController_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
	}
	else {
		
		animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
	}

	cnt_++;

	if (cnt_ > 300) {

		cnt_ = 0;
		state_ = EnemyBase::STATE::ATTACK;
	}
}

void EnemyBase::UpdateMove(void)
{
	static int count = 0;

	//移動
	pos_.x += moveDir_.x * 2.0f;
	pos_.z += moveDir_.z * 2.0f;
	count++;

	if (count >= 90){
		if (GetRand(100) == 0) {
			
			count = 0;
			state_ = STATE::WAIT;
		}
	}
}

void EnemyBase::UpdateAttack(void)
{
	if (attackAFlg_) {
		
		UpdateAttackA();
	}
	else if (attackBFlg_) {
		
		UpdateAttackB();
	}
	else if (attackCFlg_) {
		
		UpdateAttackC();
	}
	else {
		if (GetRand(3) >= 0) {

			state_ = STATE::MOVE;
		}
		else {

			state_ = STATE::WAIT;
		}
	}
}

void EnemyBase::UpdateAttackA(void)
{
	if (animationController_->GetTime() >= 33) {

		attackPos1_ = VAdd(attackPos1_, VScale(attackDir_, attackSpeed_));

		if (!attackShowFlg_) {

			attackShowFlg_ = true;
		}
	}
	else {

		DirectionPlayer();

		attackPos1_ = VAdd(pos_, VTransform(ATTACK_POS_A, AngleUtility::GetMatrixRotateXYZ(angles_)));
		attackDir_ = VSub(VAdd(player_->GetPos(), { 0.0f, 80.0f, 0.0f }), attackPos1_);
		attackDir_ = VNorm(attackDir_);
	}
	//attackPos1_ = VAdd(pos_, VTransform(ATTACK_POS_A, AngleUtility::GetMatrixRotateXYZ(angles_)));
	//attackPos2_ = VAdd(attackPos1_, attackDir_);
	if (attackPos1_.y < ATTACK_RADIUS) {

		attackAFlg_ = false;
		attackShowFlg_ = false;
	}
}

void EnemyBase::UpdateAttackB(void)
{
	attackPos1_ = MV1GetFramePosition(modelId_, 57);
	attackPos2_ = MV1GetFramePosition(modelId_, 59);

	if (animationController_->IsEnd()) {

		attackBFlg_ = false;
	}
}

void EnemyBase::UpdateAttackC(void)
{
	if (animationController_->GetTime() >= 14) {

		attackPos1_ = MV1GetFramePosition(modelId_, 6);
		attackPos2_ = MV1GetFramePosition(modelId_, 16);
	}
	if (animationController_->IsEnd()) {

		attackCFlg_ = false;
	}
}

void EnemyBase::UpdateEscape(void)
{
}
