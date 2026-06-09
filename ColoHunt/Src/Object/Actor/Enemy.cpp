#include <cmath>
#include "../../Application.h"
#include "../../Utility/Utility.h"
#include "../../Utility/AngleUtility.h"
#include "../Common/AnimationController.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Audio/AudioManager.h"
#include "../../Manager/EffectResManager.h"
#include "../Collider/ColliderModel.h"
#include "../Collider/ColliderSphere.h"
#include "../Collider/ColliderLine.h"
#include "../Collider/ColliderCapsule.h"
#include "Player.h"
#include "Enemy.h"
#include <EffekseerForDXLib.h>


Enemy::Enemy(Player* pl) : ActorBase(), attackAFlg_(false), attackBFlg_(false), attackCFlg_(false),
	attackDiff_(0), attackDir_(), speed_(SPEED),
	clearFlg_(false), cnt_(0), player_(pl), state_(STATE::WAIT), targetAngles_(), downCnt_(0), attackSpeed_(ATTACK_SPEED),
	baseAttackDiff_(BASE_ATTACK_DIFF), angryFlg_(false), first_(false)
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

	moveDir_ = Utility::DIR_B;

	hp_ = MAX_HP;
}

void Enemy::InitCollider()
{
	// モデルコライダ
	ColliderModel* colModel = new ColliderModel(&transform_);
	ownColliders_.emplace(COLLIDER_TAG::MODEL, colModel);

	// 主に地面との衝突で仕様する線分コライダ
	ColliderLine* colLine = new ColliderLine(&transform_, COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
	ownColliders_.emplace(COLLIDER_TAG::LINE, colLine);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule = new ColliderCapsule(&transform_,
		COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS, COL_CAPSULE_RADIUS);
	ownColliders_.emplace(COLLIDER_TAG::CAPSULE, colCapsule);

	// 攻撃用の球体コライダ
	ColliderSphere* colSphere = new ColliderSphere(&shotTransform_, Utility::VECTOR_ZERO, ATTACK_RADIUS);
	ownColliders_.emplace(COLLIDER_TAG::SPHERE, colSphere);

	// 各部位のコライダー座標
	headPosStart_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Hals"));
	headPosEnd_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Mauloben"));;

	armPosRStart_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Oberarm_R"));;
	armPosREnd_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Vorderpfote_R"));;

	armPosLStart_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Oberarm_L"));;
	armPosLEnd_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Vorderpfote_L"));;

	legPosRStart_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Oberschenkel_R"));;
	legPosREnd_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Pfote2_R"));;

	legPosLStart_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Oberschenkel_L"));;
	legPosLEnd_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Pfote2_L"));;

	bodyPosStart_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Hals_fett_end_end"));;
	bodyPosEnd_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Schwanz"));;

	// 各部位のコライダー
	colCapsule = new ColliderCapsule(headPosStart_, headPosEnd_, COL_BODY_HEAD_RADIUS);
	ownColliders_.emplace(COLLIDER_TAG::HEAD, colCapsule);

	colCapsule = new ColliderCapsule(armPosRStart_, armPosREnd_, COL_PARTS_RADIUS);
	ownColliders_.emplace(COLLIDER_TAG::ARM_R , colCapsule);

	colCapsule = new ColliderCapsule(armPosLStart_, armPosLEnd_, COL_PARTS_RADIUS);
	ownColliders_.emplace(COLLIDER_TAG::ARM_L, colCapsule);

	colCapsule = new ColliderCapsule(legPosRStart_, legPosREnd_, COL_PARTS_RADIUS);
	ownColliders_.emplace(COLLIDER_TAG::LEG_R, colCapsule);

	colCapsule = new ColliderCapsule(legPosLStart_, legPosLEnd_, COL_PARTS_RADIUS);
	ownColliders_.emplace(COLLIDER_TAG::LEG_L, colCapsule);

	colCapsule = new ColliderCapsule(bodyPosStart_, bodyPosEnd_, COL_BODY_HEAD_RADIUS);
	ownColliders_.emplace(COLLIDER_TAG::BODY, colCapsule);

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
	if (attackAFlg_) {
	
		shotTransform_.pos = VAdd(shotTransform_.pos, VScale(attackDir_, attackSpeed_));

		//位置等々の設定
		SetPosPlayingEffekseer3DEffect(effectHandle_, shotTransform_.pos.x, shotTransform_.pos.y, shotTransform_.pos.z);
		SetScalePlayingEffekseer3DEffect(effectHandle_, FIRE_SIZE, FIRE_SIZE, FIRE_SIZE);
		SetRotationPlayingEffekseer3DEffect(effectHandle_, 0.0f, 0.0f, 0.0f);
	}
	//モデルの更新
	animationCtrl_->Update();
	transform_.Update();

	// 各部位のコライダー座標
	headPosStart_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Hals"));
	headPosEnd_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Mauloben"));;

	armPosRStart_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Oberarm_R"));;
	armPosREnd_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Vorderpfote_R"));;

	armPosLStart_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Oberarm_L"));;
	armPosLEnd_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Vorderpfote_L"));;

	legPosRStart_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Oberschenkel_R"));;
	legPosREnd_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Pfote2_R"));;

	legPosLStart_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Oberschenkel_L"));;
	legPosLEnd_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Pfote2_L"));;

	bodyPosStart_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Hals_fett_end_end"));;
	bodyPosEnd_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, "Schwanz"));;

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
	/*for (auto info : ownColliders_) {
		info.second->Draw();
	}*/
}

void Enemy::DeleteShot(void)
{
	attackAFlg_ = false;

	StopEffekseer3DEffect(effectHandle_);

	//リソースを得る
	int resource = EffectResManager::GetInstance().GetResourceId(EffectResManager::TYPE::BURST);

	//エフェクトの再生
	int effect = PlayEffekseer3DEffect(resource);

	//位置等々の設定
	SetPosPlayingEffekseer3DEffect(effect, shotTransform_.pos.x, shotTransform_.pos.y, shotTransform_.pos.z);
	SetScalePlayingEffekseer3DEffect(effect, EFFECT_SIZE, EFFECT_SIZE, EFFECT_SIZE);
	SetRotationPlayingEffekseer3DEffect(effect, 0.0f, 0.0f, 0.0f);
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
	if (hp_ <= ANGERY_HP && !angryFlg_) {

		Anger();
	}
	hp_ -= damage;

	//HPがゼロならクリア
	if (hp_ <= 0 && !clearFlg_) {

		// 画面のイメージのみ瞬間を保存させる
		SceneManager::GetInstance().SetScreenImage();
		ChangeState(STATE::KO);
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
	transform_.rot.y = AngleUtility::LerpAngle(transform_.rot.y, targetAngles_.y, LERP);
	MV1SetRotationMatrix(transform_.modelId, AngleUtility::Multiplication(DIFF_ANGLES, transform_.rot));

	if (fabsf(transform_.rot.y - targetAngles_.y) <= NEAR_STOP_DIFF || fabsf(transform_.rot.y - targetAngles_.y) >= Utility::ONE_CIRCLE - NEAR_STOP_DIFF) {

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
	attackDiff_ = GetRand(RAND_ATTACK_DIFF) + baseAttackDiff_;
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
	AudioManager::GetInstance()->PlaySE(SoundID::SE_WOLF_ATTACK);

	float v = VSize(VSub(player_->GetTransform().pos, transform_.pos));
	if (v >= ATTACK_A_DIFF) {

		animationCtrl_->Play(static_cast<int>(ANIM_TYPE::ATTACK_A), false);
		
		attack_ = ATTACK::SHOT;
	}
	else if (v <= ATTACK_C_DIFF) {
		if (GetRand(2) == 1) {

			animationCtrl_->Play(static_cast<int>(ANIM_TYPE::ATTACK_C), false);

			attack_ = ATTACK::HEAD;
		}
		else {

			animationCtrl_->Play(static_cast<int>(ANIM_TYPE::ATTACK_B), false);

			attack_ = ATTACK::ARM;
		}
	}
	else {

		animationCtrl_->Play(static_cast<int>(ANIM_TYPE::ATTACK_B), false);

		attack_ = ATTACK::ARM;
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

void Enemy::Anger(void)
{
	ChangeState(STATE::DOWN);

	angryFlg_ = true;

	animationCtrl_->ChangeSpeed(static_cast<int>(ANIM_TYPE::ATTACK_A), 60);
	animationCtrl_->ChangeSpeed(static_cast<int>(ANIM_TYPE::ATTACK_B), 60);
	animationCtrl_->ChangeSpeed(static_cast<int>(ANIM_TYPE::ATTACK_C), 60);

	attackSpeed_ = ATTACK_SPEED * 1.5f;
	speed_ = SPEED * 1.5f;
	baseAttackDiff_ = BASE_ATTACK_DIFF / 2;
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
	else if(GetRand(attackDiff_) >= 80 && VSize(VSub(player_->GetTransform().pos, transform_.pos)) >= CHANGE_MOVE_DIFF) {

		cnt_ = 0;
		ChangeState(STATE::MOVE);
	}
}

void Enemy::UpdateMove(void)
{
	if (!Turn()) {

		return;
	}

	float prevDist = VSize(VSub(player_->GetTransform().pos, transform_.pos));

	if (!AudioManager::GetInstance()->IsPlaySE(SoundID::SE_WOLF_RUN)) {

		AudioManager::GetInstance()->PlaySE(SoundID::SE_WOLF_RUN);
	}

	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::RUN), true);

	transform_.pos.x += moveDir_.x * speed_;
	transform_.pos.z += moveDir_.z * speed_;

	if (VSize(VSub(player_->GetTransform().pos, transform_.pos)) <= FAR_STOP_DIFF || prevDist <= NEAR_STOP_DIFF ) {

		AudioManager::GetInstance()->StopSE(SoundID::SE_WOLF_RUN);
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
		first_ = false;
	}
}

void Enemy::UpdateAttackA(void)
{
	if (animationCtrl_->GetTime() >= START_TIMING_A) {
		if (!first_) {

			attackAFlg_ = true;

			//リソースを得る
			int resource = EffectResManager::GetInstance().GetResourceId(EffectResManager::TYPE::SHOT);
			//エフェクトの再生
			effectHandle_ = PlayEffekseer3DEffect(resource);

			first_ = true;
		}
	}
	else {

		DirectionPlayer();
		Turn();

		shotTransform_.pos = VAdd(transform_.pos, VTransform(ATTACK_POS_A, AngleUtility::GetMatrixRotateXYZ(transform_.rot)));
		attackDir_ = VSub(VAdd(player_->GetTransform().pos, SHOT_AIM), shotTransform_.pos);
		attackDir_ = VNorm(attackDir_);
	}
	shotTransform_.Update();
}

void Enemy::UpdateAttackB(void)
{
	if (animationCtrl_->GetTime() >= STOP_TIMING) {

		attackBFlg_ = false;
	}
	else if (animationCtrl_->GetTime() >= START_TIMING_B) {
		
		attackBFlg_ = true;
	}
}

void Enemy::UpdateAttackC(void)
{
	if (animationCtrl_->GetTime() >= STOP_TIMING) {

		attackCFlg_ = false;
	}
	else if (animationCtrl_->GetTime() >= START_TIMING_C) {

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
	if (downCnt_ >= DOWN_NUM) {

		downCnt_ = 0;
		animationCtrl_->Play(static_cast<int>(ANIM_TYPE::UP), false);
	}
}

void Enemy::UpdateKO(void)
{
	clearFlg_ = true;
}
