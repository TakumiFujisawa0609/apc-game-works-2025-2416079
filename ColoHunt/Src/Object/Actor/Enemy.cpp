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
	attackDiff_(0), attackDir_(), speed_(SPEED), effectHandle_(),
	clearFlg_(false), cnt_(0), player_(pl), state_(STATE::WAIT), targetAngles_(), downCnt_(0),
	attackSpeedA_(), attackSpeedB_(), attackSpeedC_(),
	baseAttackDiff_(BASE_ATTACK_DIFF), angryFlg_(false), first_(false),
	armPosLEnd_(),armPosLStart_(),armPosREnd_(),armPosRStart_(), bodyPosEnd_() ,bodyPosStart_(),
	headPosEnd_(), headPosStart_(), legPosLEnd_(), legPosLStart_(), legPosREnd_(), legPosRStart_(),
	StateChange(), StateUpdate(), damageA_(), damageB_(), damageC_()
{
}

Enemy::~Enemy(void)
{
}

void Enemy::InitLoad()
{
	transform_.SetModel(MV1LoadModel((Application::PATH_MODEL + MODEL_NAME).c_str()));
}

void Enemy::InitAnim()
{
	animationCtrl_ = new AnimationController(transform_.modelId);

	for (const ANIMATION_FBX& init : ANIM_FBX) {

		animationCtrl_->AddInFbx(init.num, init.speed, init.fbx);
	}
}

void Enemy::InitTransform()
{
	// 初期座標
	transform_.pos = DEFAULT_POS;
	DirectionPlayer();
	transform_.rot = targetAngles_;
	transform_.localRot = DIFF_ANGLES;
	transform_.scl = SCALE;
	moveDir_ = Utility::DIR_B;

	// HP系の初期化
	hp_ = MAX_HP;
	for (int i = static_cast<int>(COLLIDER_TAG::HEAD); i < static_cast<int>(COLLIDER_TAG::MAX); i++) {

		partsHp_.emplace(static_cast<COLLIDER_TAG>(i), MAX_HP);
		partsBroke_.emplace(static_cast<COLLIDER_TAG>(i), false);
	}

	// 攻撃系の初期化
	damageA_ = POWER_A;
	damageB_ = POWER_B;
	damageC_ = POWER_C;
	attackSpeedA_ = ATTACK_SPEED;
	attackSpeedB_ = attackSpeedC_ = ATTACK_ANIMATION_SPEED;

#pragma region 関数ポインタのセットアップ
	StateUpdate[(int)STATE::WAIT] = &Enemy::UpdateWait;
	StateUpdate[(int)STATE::MOVE] = &Enemy::UpdateMove;
	StateUpdate[(int)STATE::ATTACK_A] = &Enemy::UpdateAttackA;
	StateUpdate[(int)STATE::ATTACK_B] = &Enemy::UpdateAttackB;
	StateUpdate[(int)STATE::ATTACK_C] = &Enemy::UpdateAttackC;
	StateUpdate[(int)STATE::KO] = &Enemy::UpdateKO;
	StateUpdate[(int)STATE::DOWN] = &Enemy::UpdateDown;

	StateChange[(int)STATE::WAIT] = &Enemy::ChangeWait;
	StateChange[(int)STATE::MOVE] = &Enemy::ChangeMove;
	StateChange[(int)STATE::ATTACK] = &Enemy::ChangeAttack;
	StateChange[(int)STATE::KO] = &Enemy::ChangeKO;
	StateChange[(int)STATE::DOWN] = &Enemy::ChangeDown;
#pragma endregion

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

	// フレームの座標の登録
	SetFramePos();

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
	(this->*StateUpdate[static_cast<int>(state_)])();

	if (attackAFlg_) {
	
		shotTransform_.pos = VAdd(shotTransform_.pos, VScale(attackDir_, attackSpeedA_));
		shotTransform_.Update();

		//位置等々の設定
		SetPosPlayingEffekseer3DEffect(effectHandle_, shotTransform_.pos.x, shotTransform_.pos.y, shotTransform_.pos.z);
		SetScalePlayingEffekseer3DEffect(effectHandle_, FIRE_SIZE, FIRE_SIZE, FIRE_SIZE);
		SetRotationPlayingEffekseer3DEffect(effectHandle_, 0.0f, 0.0f, 0.0f);
	}

	for (int i = static_cast<int>(COLLIDER_TAG::ARM_R); i < static_cast<int>(COLLIDER_TAG::MAX); i++) {
		if (partsBroke_.at(static_cast<COLLIDER_TAG>(i))) {

			VECTOR pos{};
			switch (static_cast<COLLIDER_TAG>(i))
			{
			case ActorBase::COLLIDER_TAG::ARM_R:

				pos = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, R_ARM_BREAK_FRAME));
				break;

			case ActorBase::COLLIDER_TAG::ARM_L:
				
				pos = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, L_ARM_BREAK_FRAME));
				break;

			case ActorBase::COLLIDER_TAG::LEG_R:
				
				pos = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, R_LEG_BREAK_FRAME));
				break;

			case ActorBase::COLLIDER_TAG::LEG_L:
				
				pos = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, L_LEG_BREAK_FRAME));
				break;

			default:
				break;
			}

			SetPosPlayingEffekseer3DEffect(partsBrokeEffect_.at(static_cast<COLLIDER_TAG>(i)), pos.x, pos.y, pos.z);
		}
	}

	//モデルの更新
	animationCtrl_->Update();
	transform_.Update();

	// フレームの座標の更新
	SetFramePos();

	//当たり判定を更新
	MV1RefreshCollInfo(transform_.modelId);
}

void Enemy::ChangeState(STATE state)
{
	state_ = state;

	//ステータス変更時の初期化
	(this->*StateChange[static_cast<int>(state_)])();
}

void Enemy::Draw(void) const
{
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

void Enemy::Damage(COLLIDER_TAG tag, int damage)
{
	// Hpが一定値を下回ったら怒って強くなる
	if (hp_ <= ANGRY_HP && !angryFlg_) {

		Angry();
	}
	// HPにダメージ
	hp_ -= damage;
	
	if (tag != COLLIDER_TAG::NON) {
		// 部位に15倍のダメージ
		if (partsHp_.at(tag) > 0) {

			partsHp_.at(tag) -= damage * 15;
		}

		// パーツのHPがゼロなら各効果を与える
		if (partsHp_.at(tag) <= 0 && !partsBroke_.at(tag)) {
			partsBroke_.at(tag) = true;
			AudioManager::GetInstance()->PlaySE(SoundID::SE_WOLF_BREAK);
			switch (tag)
			{
			case ActorBase::COLLIDER_TAG::HEAD:
			{
				// ダメージを下げ攻撃を遅くする
				damageC_ -= 5;
				attackSpeedC_ = 30.0f;
				animationCtrl_->ChangeSpeed(static_cast<int>(ANIM_TYPE::ATTACK_C), attackSpeedC_);
				if (partsBroke_.at(COLLIDER_TAG::BODY)) {

					MV1SetMeshVisible(transform_.modelId, FUR_MESH, false);
				}
				break;
			}
			case ActorBase::COLLIDER_TAG::BODY:
			{
				// ダメージを下げ攻撃を遅くする
				damageA_ -= 5;
				attackSpeedA_ -= 5.0f;
				if (partsBroke_.at(COLLIDER_TAG::HEAD)) {

					MV1SetMeshVisible(transform_.modelId, FUR_MESH, false);
				}
				break;
			}
			case ActorBase::COLLIDER_TAG::ARM_R:
			{
				// ダメージを下げる
				damageB_ -= 5;

				//リソースを得る
				int resource = EffectResManager::GetInstance().GetResourceId(EffectResManager::TYPE::BROKE);

				//エフェクトの再生
				partsBrokeEffect_.emplace(tag, PlayEffekseer3DEffect(resource));

				//位置等々の設定
				VECTOR pos = pos = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, R_ARM_BREAK_FRAME));
				SetPosPlayingEffekseer3DEffect(partsBrokeEffect_.at(tag), pos.x, pos.y, pos.z);

				break;
			}
			case ActorBase::COLLIDER_TAG::ARM_L:
			{
				// 攻撃を遅くする
				attackSpeedB_ = 30.0f;
				animationCtrl_->ChangeSpeed(static_cast<int>(ANIM_TYPE::ATTACK_B), attackSpeedB_);

				//リソースを得る
				int resource = EffectResManager::GetInstance().GetResourceId(EffectResManager::TYPE::BROKE);

				//エフェクトの再生
				partsBrokeEffect_.emplace(tag, PlayEffekseer3DEffect(resource));

				//位置等々の設定
				VECTOR pos = pos = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, L_ARM_BREAK_FRAME));
				SetPosPlayingEffekseer3DEffect(partsBrokeEffect_.at(tag), pos.x, pos.y, pos.z);

				break;
			}
			case ActorBase::COLLIDER_TAG::LEG_R:
			{
				// 足を遅くする
				speed_ -= 0.5f;

				//リソースを得る
				int resource = EffectResManager::GetInstance().GetResourceId(EffectResManager::TYPE::BROKE);

				//エフェクトの再生
				partsBrokeEffect_.emplace(tag, PlayEffekseer3DEffect(resource));

				//位置等々の設定
				VECTOR pos = pos = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, R_LEG_BREAK_FRAME));
				SetPosPlayingEffekseer3DEffect(partsBrokeEffect_.at(tag), pos.x, pos.y, pos.z);

				break;
			}
			case ActorBase::COLLIDER_TAG::LEG_L:
			{
				// 足を遅くする
				speed_ -= 0.5f;

				//リソースを得る
				int resource = EffectResManager::GetInstance().GetResourceId(EffectResManager::TYPE::BROKE);

				//エフェクトの再生
				partsBrokeEffect_.emplace(tag, PlayEffekseer3DEffect(resource));

				//位置等々の設定
				VECTOR pos = pos = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, L_LEG_BREAK_FRAME));
				SetPosPlayingEffekseer3DEffect(partsBrokeEffect_.at(tag), pos.x, pos.y, pos.z);

				break;
			}
			default:
			{
				break;
			}
			}
			if (tag != COLLIDER_TAG::HEAD && tag != COLLIDER_TAG::BODY) {

				SetScalePlayingEffekseer3DEffect(partsBrokeEffect_.at(tag), BROKE_SIZE, BROKE_SIZE, BROKE_SIZE);
				SetRotationPlayingEffekseer3DEffect(partsBrokeEffect_.at(tag), 0.0f, 0.0f, 0.0f);
			}
		}
	}

	//HPがゼロならクリア
	if (hp_ <= 0 && !clearFlg_) {

		// 画面のイメージのみ瞬間を保存させる
		SceneManager::GetInstance().SetScreenImage();
		ChangeState(STATE::KO);
	}
}

void Enemy::DirectionPlayer(void)
{
	// プレイヤーを見る
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
		if (state_ != STATE::ATTACK_A) {
		
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
	// プレイヤーの方向を向ける
	DirectionPlayer();
	Turn();

	//進める方向の更新
	moveDir_.x = sinf(targetAngles_.y);
	moveDir_.z = cosf(targetAngles_.y);
}

void Enemy::ChangeAttack(void)
{
	// 攻撃時の音を出す
	AudioManager::GetInstance()->PlaySE(SoundID::SE_WOLF_ATTACK);

	// 距離を出す
	float v = VSize(VSub(player_->GetTransform().pos, transform_.pos));

	// 遠ければ遠距離攻撃を出す
	if (v >= ATTACK_A_DIFF) {

		// 念のため
		first_ = false;

		animationCtrl_->Play(static_cast<int>(ANIM_TYPE::ATTACK_A), false);
		
		state_ = STATE::ATTACK_A;
	}
	// 近いなら頭攻撃も選択肢に入れる
	else if (v <= ATTACK_C_DIFF) {
		// 1/3で頭それ以外は腕攻撃
		if (GetRand(2) == 1) {

			animationCtrl_->Play(static_cast<int>(ANIM_TYPE::ATTACK_C), false);

			state_ = STATE::ATTACK_C;
		}
		else {

			animationCtrl_->Play(static_cast<int>(ANIM_TYPE::ATTACK_B), false);

			state_ = STATE::ATTACK_B;
		}
	}
	// それ以外は腕の攻撃
	else {

		animationCtrl_->Play(static_cast<int>(ANIM_TYPE::ATTACK_B), false);

		state_ = STATE::ATTACK_B;
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

void Enemy::Angry(void)
{
	ChangeState(STATE::DOWN);

	angryFlg_ = true;

	// 攻撃アニメーションスピードを4/3倍する
	attackSpeedA_ *= 4.0f / 3.0f;
	attackSpeedB_ *= 4.0f / 3.0f;
	attackSpeedC_ *= 4.0f / 3.0f;

	animationCtrl_->ChangeSpeed(static_cast<int>(ANIM_TYPE::ATTACK_A), attackSpeedA_);
	animationCtrl_->ChangeSpeed(static_cast<int>(ANIM_TYPE::ATTACK_B), attackSpeedB_);
	animationCtrl_->ChangeSpeed(static_cast<int>(ANIM_TYPE::ATTACK_C), attackSpeedC_);

	// 移動等の強化
	attackSpeedA_ *= 1.5f;
	speed_ *= 1.5f;
	baseAttackDiff_ = static_cast<int>(ANGRY_DIFF);
}

void Enemy::UpdateWait(void)
{
	// プレイヤーを向く
	DirectionPlayer();
	if (Turn()) {

		animationCtrl_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
	}

	// カウントする
	cnt_++;

	// カウントが攻撃間隔カウントを超えると攻撃に移行
	if (cnt_ >= attackDiff_) {

		cnt_ = 0;
		ChangeState(STATE::ATTACK);
	}
	// 距離があった場合確率で移動する
	else if(GetRand(attackDiff_) >= CHANGE_MOVE_RAND && VSize(VSub(player_->GetTransform().pos, transform_.pos)) >= CHANGE_MOVE_DIFF) {

		cnt_ = 0;
		ChangeState(STATE::MOVE);
	}
}

void Enemy::UpdateMove(void)
{
	// 回転中だから返す
	if (!Turn()) {

		return;
	}

	// 一フレ前の座標を出す
	float prevDist = VSize(VSub(player_->GetTransform().pos, transform_.pos));

	// 再生されてないなら移動SEを出す
	if (!AudioManager::GetInstance()->IsPlaySE(SoundID::SE_WOLF_RUN)) {

		AudioManager::GetInstance()->PlaySE(SoundID::SE_WOLF_RUN);
	}

	// 移動させる
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::RUN), true);
	transform_.pos.x += moveDir_.x * speed_;
	transform_.pos.z += moveDir_.z * speed_;

	// ある程度近づくか岩に引っかかっていたら止める
	if (VSize(VSub(player_->GetTransform().pos, transform_.pos)) <= FAR_STOP_DIFF || prevDist < VSize(VSub(player_->GetTransform().pos, transform_.pos)) || prevDist <= NEAR_STOP_DIFF ) {

		AudioManager::GetInstance()->StopSE(SoundID::SE_WOLF_RUN);
		ChangeState(STATE::WAIT);
	}
}

void Enemy::UpdateAttackA(void)
{
	// 攻撃アニメーションの始まりのタイミングを過ぎた時
	if (animationCtrl_->GetTime() >= START_TIMING_A) {
		// エフェクトが出てない状態なら出す
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

		// プレイヤーの方向を向く
		DirectionPlayer();
		Turn();

		// 攻撃の位置と方向を更新
		shotTransform_.pos = VAdd(transform_.pos, VTransform(ATTACK_POS_A, AngleUtility::GetMatrixRotateXYZ(transform_.rot)));
		shotTransform_.Update();
		attackDir_ = VSub(VAdd(player_->GetTransform().pos, SHOT_AIM), shotTransform_.pos);
		attackDir_ = VNorm(attackDir_);
	}
	// アニメーションが終わっていたら止める
	if (animationCtrl_->IsEnd()) {

		ChangeState(STATE::WAIT);
		// 遠距離攻撃用のフラグを変える
		first_ = false;
	}
}

void Enemy::UpdateAttackB(void)
{
	// 攻撃アニメーションの終わりのタイミングを過ぎた時
	// 攻撃フラグを消す
	if (animationCtrl_->GetTime() >= STOP_TIMING) {

		attackBFlg_ = false;
	}
	// 攻撃アニメーションの始まりのタイミングを過ぎた時
	// 攻撃フラグを出す
	else if (animationCtrl_->GetTime() >= START_TIMING_B) {
		
		attackBFlg_ = true;
	}
	// アニメーションが終わっていたら止める
	if (animationCtrl_->IsEnd()) {

		ChangeState(STATE::WAIT);
	}
}

void Enemy::UpdateAttackC(void)
{
	// 攻撃アニメーションの終わりのタイミングを過ぎた時
	// 攻撃フラグを消す
	if (animationCtrl_->GetTime() >= STOP_TIMING) {

		attackCFlg_ = false;
	}
	// 攻撃アニメーションの始まりのタイミングを過ぎた時
	// 攻撃フラグを出す
	else if (animationCtrl_->GetTime() >= START_TIMING_C) {

		attackCFlg_ = true;
	}
	// アニメーションが終わっていたら止める
	if (animationCtrl_->IsEnd()) {

		ChangeState(STATE::WAIT);
	}
}

void Enemy::UpdateDown(void)
{
	// アニメーションが終わっていたら
	if (animationCtrl_->IsEnd()) {
		// 起き上がるアニメーションなら止めて返す
		if (animationCtrl_->GetPlayType() == static_cast<int>(ANIM_TYPE::UP)) {

			ChangeState(STATE::WAIT);
			return;
		}
		// もう一度もがかせる
		animationCtrl_->Play(static_cast<int>(ANIM_TYPE::STRUGGLE), true);
	}

	// もがいた回数を溜める
	if (animationCtrl_->GetTime() >= animationCtrl_->GetTotalTime()) {
	
		downCnt_++;
	}
	// もがいた数が一定数を超えると起き上がらせる
	if (downCnt_ >= DOWN_NUM) {

		downCnt_ = 0;
		animationCtrl_->Play(static_cast<int>(ANIM_TYPE::UP), false);
	}
}

void Enemy::UpdateKO(void)
{
	clearFlg_ = true;
}

void Enemy::SetFramePos(void)
{
	// 各部位のコライダー用座標
	headPosStart_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, HEAD_START_FRAME));
	headPosEnd_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, HEAD_END_FRAME));;

	armPosRStart_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, R_ARM_START_FRAME));
	armPosREnd_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, R_ARM_END_FRAME));

	armPosLStart_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, L_ARM_START_FRAME));
	armPosLEnd_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, L_ARM_END_FRAME));

	legPosRStart_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, R_LEG_START_FRAME));
	legPosREnd_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, R_LEG_END_FRAME));

	legPosLStart_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, L_LEG_START_FRAME));
	legPosLEnd_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, L_LEG_END_FRAME));

	bodyPosStart_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, BODY_START_FRAME));
	bodyPosEnd_ = MV1GetFramePosition(transform_.modelId, MV1SearchFrame(transform_.modelId, BODY_END_FRAME));

}
