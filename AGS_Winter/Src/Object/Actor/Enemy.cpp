#include <cmath>
#include "../../Application.h"
#include "../../Utility/Utility.h"
#include "../../Utility/AngleUtility.h"
#include "../Common/AnimationController.h"
#include "../../Manager/SceneManager.h"
#include "Player.h"
#include "Enemy.h"


Enemy::Enemy(Player* pl): ActorBase(), attackAFlg_(false), attackBFlg_(false), attackCFlg_(false),
	attackDiff_(120), attackDir_(), attackPos1_(Utility::VECTOR_ZERO), attackPos2_(Utility::VECTOR_ZERO), attackPrevPos_(Utility::VECTOR_ZERO),
	attackShowFlg_(false), clearFlg_(false), cnt_(0), coolDown_(0), isCoolDown_(false),	player_(pl), state_(STATE::WAIT), targetAngles_(), downCnt_(0)
{
}

Enemy::~Enemy(void)
{
}

void Enemy::InitLoad()
{
	modelId_ = MV1LoadModel((Application::PATH_MODEL + "Wolf.mv1").c_str());
}

void Enemy::InitAnim()
{
	animationCtrl_ = new AnimationController(modelId_);

	animationCtrl_->AddInFbx(static_cast<int>(ANIM_TYPE::ATTACK_A), 45, 0);
	animationCtrl_->AddInFbx(static_cast<int>(ANIM_TYPE::ATTACK_B), 45, 1);
	animationCtrl_->AddInFbx(static_cast<int>(ANIM_TYPE::ATTACK_C), 45, 2);
	animationCtrl_->AddInFbx(static_cast<int>(ANIM_TYPE::IDLE), 30, 4);
	animationCtrl_->AddInFbx(static_cast<int>(ANIM_TYPE::WALK), 45, 9);
	animationCtrl_->AddInFbx(static_cast<int>(ANIM_TYPE::RUN), 60, 7);
	animationCtrl_->AddInFbx(static_cast<int>(ANIM_TYPE::DOWN), 30, 3);
	animationCtrl_->AddInFbx(static_cast<int>(ANIM_TYPE::STRUGGLE), 45, 5);
	animationCtrl_->AddInFbx(static_cast<int>(ANIM_TYPE::UP), 60, 6);
}

void Enemy::InitOwn()
{
	pos_ = DEFAULT_POS;
	prevPos_ = pos_;

	DirectionPlayer();
	angles_ = targetAngles_;
	localAngles_ = DIFF_ANGLES;

	scales_ = SCALE;

	moveDir_ = Utility::DIR_B;

	hp_ = MAX_HP;
}

void Enemy::Update(void)
{
	//前のステータスを持っておく
	prevPos_ = pos_;

	//ステータス別の更新
	switch (state_)
	{
	case Enemy::STATE::WAIT:
		
		UpdateWait();
		break;

	case Enemy::STATE::MOVE:
		
		UpdateMove();
		break;

	case Enemy::STATE::ATTACK:
		
		UpdateAttack();
		break;

	case Enemy::STATE::DOWN:

		UpdateDown();
		break;

	case Enemy::STATE::KO:
		
		UpdateKO();
		break;
	}

	//モデルの更新
	animationCtrl_->Update();
	MV1SetPosition(modelId_, pos_);

	//当たり判定を更新
	MV1RefreshCollInfo(modelId_);
}

void Enemy::ChangeState(STATE state)
{
	state_ = state;

	//ステータス変更時の初期化
	switch (state_)
	{
	case Enemy::STATE::WAIT:

		ChangeWait();
		break;

	case Enemy::STATE::MOVE:

		ChangeMove();
		break;

	case Enemy::STATE::ATTACK:
		
		ChangeAttack();
		break;

	case Enemy::STATE::DOWN:

		ChangeDown();
		break;

	case Enemy::STATE::KO:
		
		ChangeKO();
		break;
	}
}

void Enemy::Draw(void) const
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

bool Enemy::IsAttack(void) const
{
	if (attackAFlg_ || attackBFlg_ || attackCFlg_) {

		return true;
	}
	else {

		return false;
	}
}

void Enemy::Damage(int damage)
{
	if (hp_ < 200 && hp_ >190) {

		ChangeState(STATE::DOWN);
	}
	hp_ -= damage;

	//HPがゼロならクリア
	if (hp_ <= 0 && !clearFlg_) {

		ChangeState(STATE::KO);
		SceneManager::GetInstance().SetResultImage();
	}
}

void Enemy::DirectionPlayer(void)
{
	VECTOR dir = VSub(player_->GetPos(), pos_);
	targetAngles_.y = atan2f(dir.x, dir.z);
}

bool Enemy::Turn(void)
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
			animationCtrl_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
		}

		return false;
	}
}

void Enemy::ChangeWait(void)
{
	attackDiff_ = GetRand(120) + 60;
}

void Enemy::ChangeMove(void)
{
	DirectionPlayer();
	Turn();

	//進める方向の更新
	moveDir_.x = sinf(targetAngles_.y);
	moveDir_.z = cosf(targetAngles_.y);
}

void Enemy::ChangeAttack(void)
{
	attackPos1_ = attackPos2_ = { -100000.0f, -10000.0f, -100000.0f };

	if (VSize(VSub(player_->GetPos(), pos_)) >= 350.0f) {

		animationCtrl_->Play(static_cast<int>(ANIM_TYPE::ATTACK_A), false);

		attackAFlg_ = true;
	}
	else {
		if (GetRand(1) == 1) {

			animationCtrl_->Play(static_cast<int>(ANIM_TYPE::ATTACK_B), false);

			attackBFlg_ = true;
		}
		else {

			animationCtrl_->Play(static_cast<int>(ANIM_TYPE::ATTACK_C), false);

			attackCFlg_ = true;
		}
	}
}

void Enemy::ChangeDown(void)
{
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::DOWN), false);
}

void Enemy::ChangeKO(void)
{
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::DOWN), false);
}

void Enemy::UpdateWait(void)
{
	DirectionPlayer();
	
	if (Turn()) {

		animationCtrl_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
	}

	cnt_++;

	if (cnt_ >= attackDiff_) {

		cnt_ = 0;
		ChangeState(STATE::ATTACK);
	}
	else if(GetRand(attackDiff_ - cnt_) >= 180 && VSize(VSub(player_->GetPos(), pos_)) >= 220.0f) {

		cnt_ = 0;
		ChangeState(STATE::MOVE);
	}
}

void Enemy::UpdateMove(void)
{
	if (!Turn()) {

		return;
	}

	float prevDist = fabsf(VSize(VSub(player_->GetPos(), pos_)));

	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::RUN), true);

	pos_.x += moveDir_.x * SPEED;
	pos_.z += moveDir_.z * SPEED;

	if (fabsf(VSize(VSub(player_->GetPos(), pos_))) <= 300.0f || prevDist <= fabsf(VSize(VSub(player_->GetPos(), pos_)))) {

		ChangeState(STATE::WAIT);
	}
}

void Enemy::UpdateAttack(void)
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

void Enemy::UpdateAttackA(void)
{
	attackPrevPos_ = attackPos1_;

	if (animationCtrl_->GetTime() >= 33) {

		attackPos1_ = VAdd(attackPos1_, VScale(attackDir_, ATTACK_SPEED));

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

void Enemy::UpdateAttackB(void)
{
	if (animationCtrl_->GetTime() >= 50) {

		attackPos1_ = attackPos2_ = { -100000.0f, -10000.0f, -100000.0f };
	}
	else if (animationCtrl_->GetTime() >= 26) {
	
		attackPos1_ = MV1GetFramePosition(modelId_, 57);
		attackPos2_ = MV1GetFramePosition(modelId_, 59);
	}
	if (animationCtrl_->IsEnd()) {

		attackBFlg_ = false;
	}
}

void Enemy::UpdateAttackC(void)
{
	if (animationCtrl_->GetTime() >= 50) {

		attackPos1_ = attackPos2_ = { -100000.0f, -10000.0f, -100000.0f };
	}
	else if (animationCtrl_->GetTime() >= 30) {

		attackPos1_ = MV1GetFramePosition(modelId_, 6);
		attackPos2_ = MV1GetFramePosition(modelId_, 16);
	}
	if (animationCtrl_->IsEnd()) {

		attackCFlg_ = false;
	}
}

void Enemy::UpdateDown(void)
{
	if (animationCtrl_->IsEnd()) {
		if (animationCtrl_->GetPlayType() == static_cast<int>(ANIM_TYPE::UP)) {

			ChangeState(STATE::WAIT);
			return;
		}
		animationCtrl_->Play(static_cast<int>(ANIM_TYPE::STRUGGLE), true);
	}
	if (animationCtrl_->GetTime() >= animationCtrl_->GetTotalTime()) {
	
		downCnt_++;
	}
	if (downCnt_ >= 3) {

		downCnt_ = 0;
		animationCtrl_->Play(static_cast<int>(ANIM_TYPE::UP), false);
	}
}

void Enemy::UpdateKO(void)
{
	clearFlg_ = true;
}
