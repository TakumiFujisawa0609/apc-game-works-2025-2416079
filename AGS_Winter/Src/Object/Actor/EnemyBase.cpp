#include <cmath>
#include "../../Application.h"
#include "../../Utility/Utility.h"
#include "../../Utility/AngleUtility.h"
#include "../Common/AnimationController.h"
#include "../../Manager/SceneManager.h"
#include "Player.h"
#include "EnemyBase.h"


EnemyBase::EnemyBase(Player* pl):angles_(), animationController_(nullptr), attackAFlg_(false), attackBFlg_(false), attackCFlg_(false),
	attackDiff_(120), attackDir_(), attackPos1_(Utility::VECTOR_ZERO), attackPos2_(Utility::VECTOR_ZERO), attackPrevPos_(), attackShowFlg_(),
	attackSpeed_(28.0f), clearFlg_(false), cnt_(0), coolDown_(0), hp_(), isCoolDown_(false), modelId_(), moveDir_(Utility::VECTOR_ZERO),
	player_(pl), pos_(DEFAULT_POS), prevPos_(DEFAULT_POS), scales_(), speed_(), state_(), targetAngles_()
{
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Init()
{
	modelId_ = MV1LoadModel((Application::PATH_MODEL + "Wolf.mv1").c_str());

	animationController_ = new AnimationController(modelId_);

	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::ATTACK_A), 45, 0);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::ATTACK_B), 45, 1);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::ATTACK_C), 45, 2);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::IDLE), 30, 3);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::WALK), 45, 6);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::RUN), 60, 4);

	DirectionPlayer();
	angles_ = targetAngles_;

	MV1SetPosition(modelId_, pos_);
	MV1SetRotationMatrix(modelId_, AngleUtility::Multiplication(DIFF_ANGLES, angles_));
	MV1SetScale(modelId_, scales_);

	MV1SetupCollInfo(modelId_);
}

void EnemyBase::Update(void)
{
	//前のステータスを持っておく
	prevPos_ = pos_;

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

	//HPがゼロならクリア
	if (hp_ <= 0) {

		clearFlg_ = true;
	}
	//当たり判定を更新
	MV1RefreshCollInfo(modelId_);
}

void EnemyBase::ChangeState(STATE state)
{
	state_ = state;

	//ステータス変更時の初期化
	switch (state_)
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

void EnemyBase::Draw(void) const
{
	//DrawFormatString(Application::SCREEN_SIZE_X - 100, 20, 0x000000, "%.2f", (300.0f - cnt_) / 60.0f, SetFontSize(25));
	//DrawFormatString(100, 20, 0x000000, "%.2f", angles_.y, SetFontSize(25));

	if (attackAFlg_ && attackShowFlg_) {
	
		DrawSphere3D(attackPos1_, ATTACK_RADIUS, 16, 0xffaa55, 0xffaa55, true);
		//DrawCapsule3D(attackPos1_, attackPos2_, 10.0f, 16, 0x00ffff, 0x00ffff, true);
	}
	//if (attackBFlg_) {

	//	DrawCapsule3D(attackPos1_, attackPos2_, ATTACK_RADIUS, 16, 0xff0000, 0xff0000, false);
	//}
	//if (attackCFlg_) {

	//	DrawCapsule3D(attackPos1_, attackPos2_, ATTACK_RADIUS * 2, 16, 0xff0000, 0xff0000, false);
	//}
}

void EnemyBase::Release(void)
{
	animationController_->Release();
	delete animationController_;

	MV1DeleteModel(modelId_);
}

void EnemyBase::DrawModel(void) const
{
	MV1DrawModel(modelId_);
}

bool EnemyBase::IsAttack(void) const
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
	targetAngles_.y = atan2f(dir.x, dir.z);
}

bool EnemyBase::Turn(void)
{
	//ターゲットの角度までゆっくり動く
	angles_.y = AngleUtility::LerpAngle(angles_.y, targetAngles_.y, 0.1f);
	MV1SetRotationMatrix(modelId_, AngleUtility::Multiplication(DIFF_ANGLES, angles_));

	if (fabsf(angles_.y - targetAngles_.y) <= 0.0001f || fabsf(angles_.y - targetAngles_.y) >= DX_PI_F * 2 - 0.0002f) {

		//近似値まで来たので終了
		return true;
	}
	else{
		if (state_ != STATE::ATTACK) {
		
			//攻撃中以外軸合わせは歩きモーション使う
			animationController_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
		}

		return false;
	}
}

void EnemyBase::ChangeWait(void)
{
	attackDiff_ = GetRand(120) + 120;
}

void EnemyBase::ChangeMove(void)
{
	DirectionPlayer();
	Turn();

	//進める方向の更新
	moveDir_.x = sinf(targetAngles_.y);
	moveDir_.z = cosf(targetAngles_.y);
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
	DirectionPlayer();
	
	if (Turn()) {

		animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
	}

	cnt_++;

	if (cnt_ >= attackDiff_) {

		cnt_ = 0;
		ChangeState(STATE::ATTACK);
	}
	else if(GetRand(attackDiff_ - cnt_) >= 180){

		cnt_ = 0;
		ChangeState(STATE::MOVE);
	}
}

void EnemyBase::UpdateMove(void)
{
	if (!Turn()) {

		return;
	}

	float prevDist = fabsf(VSize(VSub(player_->GetPos(), pos_)));

	animationController_->Play(static_cast<int>(ANIM_TYPE::RUN), true);

	pos_.x += moveDir_.x * 8.5f;
	pos_.z += moveDir_.z * 8.5f;

	if (prevDist <= fabsf(VSize(VSub(player_->GetPos(), pos_))) || fabsf(VSize(VSub(player_->GetPos(), pos_))) <= 350.0f){
			
		ChangeState(STATE::ATTACK);
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

		ChangeState(STATE::WAIT);
	}
}

void EnemyBase::UpdateAttackA(void)
{
	attackPrevPos_ = attackPos1_;

	if (animationController_->GetTime() >= 33) {

		attackPos1_ = VAdd(attackPos1_, VScale(attackDir_, attackSpeed_));

		if (!attackShowFlg_) {

			attackShowFlg_ = true;
		}
	}
	else {

		DirectionPlayer();
		Turn();

		attackPos1_ = VAdd(pos_, VTransform(ATTACK_POS_A, AngleUtility::GetMatrixRotateXYZ(angles_)));
		attackDir_ = VSub(VAdd(player_->GetPos(), { 0.0f, 80.0f, 0.0f }), attackPos1_);
		attackDir_ = VNorm(attackDir_);
	}
	//attackPos1_ = VAdd(pos_, VTransform(ATTACK_POS_A, AngleUtility::GetMatrixRotateXYZ(angles_)));
	//attackPos2_ = VAdd(attackPos1_, attackDir_);
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
