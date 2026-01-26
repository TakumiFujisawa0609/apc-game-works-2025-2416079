#include <cmath>
#include "../../Application.h"
#include "../../Utility/Utility.h"
#include "../../Utility/AngleUtility.h"
#include "../Common/AnimationController.h"
#include "../../Manager/SceneManager.h"
#include "../Collider/ColliderModel.h"
#include "../Collider/ColliderSphere.h"
#include "../Collider/ColliderLine.h"
#include "../Collider/ColliderCapsule.h"
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
	transform_.SetModel(MV1LoadModel((Application::PATH_MODEL + "Wolf.mv1").c_str()));
}

void Enemy::InitAnim()
{
	animationCtrl_ = new AnimationController(transform_.modelId);

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

void Enemy::InitTransform()
{
	transform_.pos = DEFAULT_POS;
	DirectionPlayer();
	transform_.rot = targetAngles_;
	transform_.localRot = DIFF_ANGLES;
	transform_.scl = SCALE;

	armPos_ = MV1GetFramePosition(transform_.modelId, 59);
	armTransform_.pos = MV1GetFramePosition(transform_.modelId, 57);
	armTransform_.Update();

	headPos_ = MV1GetFramePosition(transform_.modelId, 16);
	headTransform_.pos = MV1GetFramePosition(transform_.modelId, 6);
	headTransform_.Update();

	moveDir_ = Utility::DIR_B;

	hp_ = MAX_HP;
}

void Enemy::InitCollider()
{
	// モデルコライダ
	ColliderModel* colModel = new ColliderModel(&transform_);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::MODEL), colModel);

	// 主に地面との衝突で仕様する線分コライダ
	ColliderLine* colLine = new ColliderLine(&transform_, COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule = new ColliderCapsule(&transform_,
		COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS, COL_CAPSULE_RADIUS);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);

	// 攻撃用の球体コライダ
	ColliderSphere* colSphere = new ColliderSphere(&shotTransform_, Utility::VECTOR_ZERO, ATTACK_RADIUS);
	shotColliders_.emplace(static_cast<int>(COLLIDER_TYPE::SPHERE), colSphere);

	// 攻撃用のカプセルコライダ1
	colCapsule = new ColliderCapsule(&armTransform_, &armPos_, COL_CAPSULE_RADIUS);
	armColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);

	// 攻撃用のカプセルコライダ2
	colCapsule = new ColliderCapsule(&headTransform_, &headPos_, COL_CAPSULE_RADIUS);
	headColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);
}

void Enemy::Update(void)
{
	//前のステータスを持っておく
	transform_.prevPos = transform_.pos;

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
	transform_.Update();

	//当たり判定を更新
	MV1RefreshCollInfo(transform_.modelId);
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
	if (attackAFlg_) {

		DrawSphere3D(attackPos1_, ATTACK_RADIUS, 16, 0xffaa55, 0xffaa55, true);
	}
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
	if (hp_ <= 200 && hp_ >190) {

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
	VECTOR dir = VSub(player_->GetTransform().pos, transform_.pos);
	targetAngles_.y = atan2f(dir.x, dir.z);
}

bool Enemy::Turn(void)
{
	//ターゲットの角度までゆっくり動く
	transform_.rot.y = AngleUtility::LerpAngle(transform_.rot.y, targetAngles_.y, 0.1f);
	MV1SetRotationMatrix(transform_.modelId, AngleUtility::Multiplication(DIFF_ANGLES, transform_.rot));

	if (fabsf(transform_.rot.y - targetAngles_.y) <= 0.0001f || fabsf(transform_.rot.y - targetAngles_.y) >= DX_PI_F * 2 - 0.0002f) {

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
	attackDiff_ = GetRand(120) + 45;
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
	if (VSize(VSub(player_->GetTransform().pos, transform_.pos)) >= 300.0f) {

		animationCtrl_->Play(static_cast<int>(ANIM_TYPE::ATTACK_A), false);
		
		attack_ = ATTACK::SHOT;
	}
	else if (fabsf(VSize(VSub(player_->GetTransform().pos, transform_.pos))) <= 250.0f){
		if (GetRand(1) == 1) {

			animationCtrl_->Play(static_cast<int>(ANIM_TYPE::ATTACK_B), false);

			attack_ = ATTACK::ARM;
		}
		else {

			animationCtrl_->Play(static_cast<int>(ANIM_TYPE::ATTACK_C), false);

			attack_ = ATTACK::HEAD;
		}
	}
	else {

		ChangeState(STATE::MOVE);
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
	else if(GetRand(attackDiff_) >= 80 && VSize(VSub(player_->GetTransform().pos, transform_.pos)) >= 550.0f) {

		cnt_ = 0;
		ChangeState(STATE::MOVE);
	}
}

void Enemy::UpdateMove(void)
{
	if (!Turn()) {

		return;
	}

	float prevDist = fabsf(VSize(VSub(player_->GetTransform().pos, transform_.pos)));

	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::RUN), true);

	transform_.pos.x += moveDir_.x * SPEED;
	transform_.pos.z += moveDir_.z * SPEED;

	if (fabsf(VSize(VSub(player_->GetTransform().pos, transform_.pos))) <= 250.0f || prevDist <= fabsf(VSize(VSub(player_->GetTransform().pos, transform_.pos)))) {

		ChangeState(STATE::WAIT);
	}
}

void Enemy::UpdateAttack(void)
{
	switch (attack_)
	{
	case Enemy::ATTACK::SHOT:
	
		UpdateAttackA();
		break;

	case Enemy::ATTACK::ARM:
	
		UpdateAttackB();
		break;

	case Enemy::ATTACK::HEAD:
	
		UpdateAttackC();
		break;
	}
	if (animationCtrl_->IsEnd()) {

		ChangeState(STATE::WAIT);
	}
}

void Enemy::UpdateAttackA(void)
{
	if (animationCtrl_->GetTime() >= 33) {

		shotTransform_.pos = VAdd(shotTransform_.pos, VScale(attackDir_, ATTACK_SPEED));

		if (!attackAFlg_) {

			attackAFlg_ = true;
		}
	}
	else {

		DirectionPlayer();
		Turn();

		shotTransform_.pos = VAdd(transform_.pos, VTransform(ATTACK_POS_A, AngleUtility::GetMatrixRotateXYZ(transform_.rot)));
		attackDir_ = VSub(VAdd(player_->GetTransform().pos, { 0.0f, 80.0f, 0.0f }), shotTransform_.pos);
		attackDir_ = VNorm(attackDir_);
	}
	shotTransform_.Update();
}

void Enemy::UpdateAttackB(void)
{
	armPos_ = MV1GetFramePosition(transform_.modelId, 59);
	armTransform_.pos = MV1GetFramePosition(transform_.modelId, 57);
	armTransform_.Update();

	if (animationCtrl_->GetTime() >= 50) {

		attackBFlg_ = false;
	}
	else if (animationCtrl_->GetTime() >= 26) {
		
		attackBFlg_ = true;
	}
}

void Enemy::UpdateAttackC(void)
{
	headPos_ = MV1GetFramePosition(transform_.modelId, 16);
	headTransform_.pos = MV1GetFramePosition(transform_.modelId, 6);
	headTransform_.Update();

	if (animationCtrl_->GetTime() >= 50) {

		attackCFlg_ = false;
	}
	else if (animationCtrl_->GetTime() >= 30) {

		attackCFlg_ = true;
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
