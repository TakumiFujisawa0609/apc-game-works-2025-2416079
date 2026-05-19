#include "Player.h"
#include "../../Application.h"
#include "../../Utility/Utility.h"
#include "../../Utility/VectorUtility.h"
#include "../../Utility/AngleUtility.h"
#include "../../Manager/Audio/AudioManager.h"
#include "../../Manager/Audio/SoundTable.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/Camera.h"
#include "../../Manager/Input/InputManager.h"
#include "../Common/AnimationController.h"
#include "../Collider/ColliderCapsule.h"
#include "../Collider/ColliderLine.h"
#include "../Collider/ColliderModel.h"
#include "../Item.h"


Player::Player(Item* itm): ActorBase(), item_(itm), autoHealCnt_(0), autoHealHp_(0), speed_(), effectSize_(), effectCnt_(), dodge_(), damaged_(), swordPosEnd_(),
	dodgeCnt_(), dodgeFlg_(), healCount_(0), isAttack_(false), isHealMax_(), isHeal_(false), isStaminaMax_(false), knockBackDir_(0.0f), swordPosSta_(),
	overFlg_(false), power_(0), staminaMaxCnt_(), stamina_(MAX_STAMINA), state_(STATE::WAIT), effectDir_(),se_(true), dodgeStamina_(DODGE_STAMINA),
	barEX_(), barHpEY_(), barHpSY_(), barSize_(), barSX_(),	barStaEY_(), barStaSY_(), damage_(BASIC_DAMAGE), goodDodge_(), greatDodge_(), guageEX_(),
	guageSize_(), guageSX_(), guageSY_(),hpBar_(), powerGauge_(), powerUp_(), powerUpCnt_(), effectBottomPos_(), effectTopPos_(), buff_(1.0), effectType_(EFFECT::NON)
{
}

Player::~Player(void)
{
}

void Player::InitLoad()
{
	//モデルのロード
	transform_.SetModel(MV1LoadModel((Application::PATH_MODEL + "Player.mv1").c_str()));
	powerGauge_ = LoadSoftImage((Application::PATH_IMAGE + "Power.png").c_str());
	hpBar_ = LoadSoftImage((Application::PATH_IMAGE + "HpBar.png").c_str());

#pragma region 関数ポインタのセットアップ
	StateUpdate[(int)STATE::WAIT] = &Player::UpdateWait;
	StateUpdate[(int)STATE::MOVE] = &Player::UpdateMove;
	StateUpdate[(int)STATE::ATTACK] = &Player::UpdateAttack;
	StateUpdate[(int)STATE::COMBO] = &Player::UpdateCombo;
	StateUpdate[(int)STATE::DOGDE] = &Player::UpdateDodge;
	StateUpdate[(int)STATE::DAMAGED_LIGHT] = &Player::UpdateDamagedLight;
	StateUpdate[(int)STATE::DAMAGED_HEAVY] = &Player::UpdateDamagedHeavy;
	StateUpdate[(int)STATE::KO] = &Player::UpdateKO;
	StateUpdate[(int)STATE::DRINK] = &Player::UpdateDrink;
	StateUpdate[(int)STATE::END] = &Player::UpdateEnd;
#pragma endregion

}

void Player::InitAnim()
{
	//アニメーションのロード
	animationCtrl_ = new AnimationController(transform_.modelId);

	animationCtrl_->AddInFbx(0, 60.0f, 0);
			
	animationCtrl_->Add(1, 70.0f, (Application::PATH_ANIMATION + "Walking.mv1").c_str());
	animationCtrl_->Add(2, 70.0f, (Application::PATH_ANIMATION + "Run.mv1").c_str());
	animationCtrl_->Add(3, 110.0f, (Application::PATH_ANIMATION + "Slash.mv1").c_str());
	animationCtrl_->Add(4, 110.0f, (Application::PATH_ANIMATION + "Slash_1.mv1").c_str());
	animationCtrl_->Add(5, 110.0f, (Application::PATH_ANIMATION + "Slash_2.mv1").c_str());
	animationCtrl_->Add(6, 110.0f, (Application::PATH_ANIMATION + "Slash_3.mv1").c_str());
	animationCtrl_->Add(7, 100.0f, (Application::PATH_ANIMATION + "Dodge.mv1").c_str());
	animationCtrl_->Add(8, 90.0f, (Application::PATH_ANIMATION + "Hit_Light.mv1").c_str());
	animationCtrl_->Add(9, 60.0f, (Application::PATH_ANIMATION + "Hit_Heavy.mv1").c_str());
	animationCtrl_->Add(10, 200.0f, (Application::PATH_ANIMATION + "Hit_Up.mv1").c_str());
	animationCtrl_->Add(11, 45.0f, (Application::PATH_ANIMATION + "KO.mv1").c_str());
	animationCtrl_->Add(12, 330.0f, (Application::PATH_ANIMATION + "Drinking.mv1").c_str());
	animationCtrl_->Add(13, 90.0f, (Application::PATH_ANIMATION + "Finish.mv1").c_str());
}

void Player::InitTransform()
{
	FindHpAndPower();

	transform_.pos = DEFAULT_POS;
	transform_.prevPos = transform_.pos;
	transform_.rot = Utility::VECTOR_ZERO;
	transform_.localRot = VScale(Utility::AXIS_Y, DX_PI_F);
	transform_.scl = Utility::VECTOR_ONE;

	swordPosSta_ = MV1GetFramePosition(transform_.modelId, 58);
	swordPosEnd_ = VTransform(SWORD_POS, MV1GetFrameLocalWorldMatrix(transform_.modelId, 37));

	speed_ = SPEED;
	hp_ = MAX_HP;
	moveDir_ = Utility::DIR_F;

	DoChangeState(STATE::WAIT);
}

void Player::InitCollider()
{
	// モデルコライダ
	ColliderModel* colModel = new ColliderModel(&transform_);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::MODEL), colModel);
	
	// 主に地面との衝突で仕様する線分コライダ
	ColliderLine* colLine = new ColliderLine(&transform_, COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule = new ColliderCapsule(&transform_, COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS, COL_CAPSULE_RADIUS);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);
}

void Player::Update(void)
{
	transform_.prevPos = transform_.pos;

	//状態別更新処理
	(this->*StateUpdate[(int)state_])();

	Status();
	EffectUpdate();

	//モデルの設定
	transform_.Update();
	//アニメーションの移動
	animationCtrl_->Update();
	//剣の移動
	swordPosSta_ = MV1GetFramePosition(transform_.modelId, 58);
	swordPosEnd_ = VTransform(SWORD_POS, MV1GetFrameLocalWorldMatrix(transform_.modelId, 37));
	//当たり判定の更新
	MV1RefreshCollInfo(transform_.modelId);
}

void Player::DoChangeState(STATE state)
{
	state_ = state;

	//歩く走るの音を止める
	StopSE();

	switch (state_)
	{
	case Player::STATE::WAIT:

		ChangeWait();
		break;

	case Player::STATE::MOVE:
		
		ChangeMove();
		break;

	case Player::STATE::ATTACK:
		
		ChangeAttack();
		break;

	case Player::STATE::COMBO:

		ChangeCombo();
		break;

	case Player::STATE::DOGDE:
		
		ChangeDodge();
		break;

	case Player::STATE::DAMAGED_LIGHT:
		
		ChangeDamagedLight();
		break;

	case Player::STATE::DAMAGED_HEAVY:
	
		ChangeDamagedHeavy();
		break;

	case Player::STATE::KO:

		ChangeKO();
		break;

	case Player::STATE::DRINK:

		ChangeDrink();
		break;
	}
}

void Player::Draw(void) const
{
	//エフェクトの描画
	if (effectCnt_ >= 0) {
		
		SetUseLighting(false);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 250);

		for (int i = 0; i < 9; i++) {
			switch (effectType_)
			{
			case Player::EFFECT::GREAT_DODGE:

				DrawCone3D(effectTopPos_[i], effectBottomPos_[i], 12.0f, 32, 0xffff00, 0xffff00, true);
				break;

			case Player::EFFECT::GOOD_DODGE:

				DrawCone3D(effectTopPos_[i], effectBottomPos_[i], 12.0f, 32, 0xffffff, 0xffffff, true);
				break;

			case Player::EFFECT::HEAL:

				DrawCone3D(effectTopPos_[i], effectBottomPos_[i], 12.0f, 32, 0x00ff00, 0x44cc44, true);
				break;

			case Player::EFFECT::STAMINA:

				DrawCone3D(effectTopPos_[i], effectBottomPos_[i], 12.0f, 32, 0xff5500, 0xaa3300, true);
				break;
			}
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		SetUseLighting(true);
	}
	//ステータスの描画
	DrawHpAndPower();
}

void Player::Release(void) const
{
	//モデルの開放
	ActorBase::Release();

	//画像の開放
	DeleteSoftImage(hpBar_);
	DeleteSoftImage(powerGauge_);
}

void Player::Damage(int damage, float dir)
{
	//状態の更新
	damaged_ = damage;
	autoHealHp_ = damage / 3;
	knockBackDir_ = dir;
	greatDodge_ = goodDodge_ = dodge_ = false;
	isAttack_ = false;
	buff_ = 1.0;

	AudioManager::GetInstance()->PlaySE(SoundID::SE_DAMAGE);

	//回復を止める
	if (isHealMax_) {

		isHealMax_ = false;
	}
	if (isHeal_) {

		isHeal_ = false;
	}

	// パワーアップしていないならモーションをとる
	if (!powerUp_) {
		if (damage >= 15) {

			DoChangeState(STATE::DAMAGED_HEAVY);
		}
		else {

			DoChangeState(STATE::DAMAGED_LIGHT);
		}
	}

	// HPがゼロなら死ぬ
	if (hp_ - damaged_ <= 0) {

		autoHealHp_ = 0;
		// 瞬間の画面のイメージ保存
		SceneManager::GetInstance().SetScreenImage();
		DoChangeState(STATE::KO);
	}
}

bool Player::IsAttackMotion(void) const
{
	if (state_ == STATE::ATTACK || state_ == STATE::COMBO) {

		return true;
	}
	else {

		return false;
	}
}

bool Player::IsHit(void) const
{
	if (state_ != STATE::DAMAGED_LIGHT && state_ != STATE::DAMAGED_HEAVY) {

		return true;
	}
	else {

		return false;
	}
}

void Player::GreatDodge(void)
{
	greatDodge_ = true;

	//エフェクトの生成
	EffectCreate();
	effectType_ = EFFECT::GREAT_DODGE;

	if (!powerUp_) {

		power_ += 3;
	}
	AudioManager::GetInstance()->PlaySE(SoundID::SE_DODGE);
}

void Player::GoodDodge(void)
{
	goodDodge_ = true;

	//エフェクトの生成
	EffectCreate();
	effectType_ = EFFECT::GOOD_DODGE;

	if (!powerUp_) {

		power_++;
	}
	AudioManager::GetInstance()->PlaySE(SoundID::SE_DODGE);
}

void Player::Status(void)
{
	//スタミナ回復の条件
	if ((state_ != STATE::DOGDE && (!InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::RUN).keyNew && state_ != STATE::WAIT)) || isStaminaMax_) {
		if (stamina_ < MAX_STAMINA) {

			stamina_++;
		}
	}
	//マイナスにならないように
	if (stamina_ < 0.0f) {

		stamina_ = 0.0f;
	}
	//一定数回復する
	if (isHeal_) {
		
		Heal();

		if (healCount_ >= HEAL_COUNT) {

				isHeal_ = false;
				healCount_ = 0;
		}
	}
	//全回復
	if (isHealMax_) {

		Heal();
	}
	//スタミナ無消費
	if (isStaminaMax_) {
		if (staminaMaxCnt_ > STAMINA_MAX_TIME) {

			staminaMaxCnt_ = 0;
			dodgeStamina_ = DODGE_STAMINA;
			isStaminaMax_ = false;
		}
		else {

			staminaMaxCnt_++;
		}
	}
	//自動回復
	if (autoHealHp_ > 0) {

		autoHealCnt_++;

		if (autoHealCnt_ >= 90) {

			autoHealCnt_ = 0;
			autoHealHp_--;
			hp_++;
		}
	}
	//ゲージマックス時
	if (powerUp_) {

		powerUpCnt_++;

		//ゲージが減る速度
		if (powerUpCnt_ >= Application::FPS * 3) {
		
			powerUpCnt_ = 0;
			power_--;
		}
		if (power_ <= 0) {

			power_ = 0;
			damage_ = BASIC_DAMAGE;
			powerUp_ = false;
		}
	}
	//ゲージマックスでない時
	else {
		if (power_ >= MAX_POWER) {

			power_ = MAX_POWER;
			damage_ = (int)(BASIC_DAMAGE * 1.5);
			dodgeStamina_ = DODGE_STAMINA / 2.0f;
			powerUp_ = true;
		}
	}
	//シームレスにHPが減る
	if (damaged_ > 0) {

		hp_ -= 1;
		damaged_ -= 1;

		if (hp_ <= 0) {

			hp_ = 0;
		}
	}
}

void Player::Heal(void)
{
	if (hp_ < MAX_HP) {
		if (autoHealHp_ > 0) {

			autoHealHp_--;
		}
		hp_++;
		healCount_++;
	}
	else {

		hp_ = MAX_HP;
		isHeal_ = false;
		isHealMax_ = false;
		healCount_ = 0;
	}
}

void Player::KnockBack()
{
	transform_.pos.x += sinf(knockBackDir_) * 4.0f;
	transform_.pos.z += cosf(knockBackDir_) * 4.0f;

	transform_.rot.y = knockBackDir_ - DX_PI_F;
}

void Player::FindHpAndPower(void)
{
	int dx, dy;
	int r, g, b, a;
	GetSoftImageSize(hpBar_, &dx, &dy);
	bool first = false;

	for (float x = 0; x <= static_cast<float>(dx); x++) {
		for (float y = 0; y < static_cast<float>(dy / 2); y++) {

			GetPixelSoftImage(hpBar_, static_cast<int>(x), static_cast<int>(y), &r, &g, &b, &a);

			//元画像の真っ青のサイズを探す
			if (r ==0 && g == 0 && b == 255 && a > 0) {
				if (!first) {
					
					//バーの最小XとYを出す
					first = true;
					barSX_ = x;
					barHpSY_ = y;
				}
				//サイズ更新したら書き換える
				if (barEX_ < x) {

					barEX_ = x;
				}
				if (barHpEY_ < y) {

					barHpEY_ = y;
				}
			}
		}
	}
	first = false;

	for (float x = 0; x <= static_cast<float>(dx); x++) {
		for (float y = static_cast<float>(dy / 2); y <= static_cast<float>(dy); y++) {

			GetPixelSoftImage(hpBar_, static_cast<int>(x), static_cast<int>(y), &r, &g, &b, &a);

			//元画像の真っ青のサイズを探す
			if (r == 0 && g == 0 && b == 255 && a > 0) {
				if (!first) {

					//HPとXは同じところなので
					//スタミナはYのみ
					first = true;
					barStaSY_ = y;
				}
				if (barStaEY_ < y) {

					barStaEY_ = y;
				}
			}
		}
	}
	barSize_ = barEX_ - barSX_;

	//真っ青の部分を黒に染める
	for (int y = static_cast<int>(barHpSY_); y <= static_cast<int>(barHpEY_); y++) {

		DrawLineSoftImage(hpBar_, static_cast<int>(barSX_), y, static_cast<int>(barEX_) + 1, y, 0, 0, 0, 255);
	}
	for (int y = static_cast<int>(barStaSY_); y <= static_cast<int>(barStaEY_); y++) {

		DrawLineSoftImage(hpBar_, static_cast<int>(barSX_), y, static_cast<int>(barEX_) + 1, y, 0, 0, 0, 255);
	}
	first = false;

	GetSoftImageSize(powerGauge_, &dx, &dy);

	int gx = 0;

	for (int y = 0; y < dy; y++) {
		for (int x = 0; x <= dx; x++) {

			GetPixelSoftImage(powerGauge_, x, y, &r, &g, &b, &a);

			//真っ青な部分を探す
			if (r == 0 && g == 0 && b == 255 && a > 0) {
				if (!first) {

					first = true;
					guageSX_ = x;
					guageSY_ = y;
				}
				gx = x;
			}
		}
		//斜めの部分をそれぞれ出す
		if (gx > 0) {
			guageEX_.push_back(gx + 1);
			gx = 0;
		}
	}
}

void Player::DrawHpAndPower(void) const
{
	//バー全体をHPの最大値分する
	float barRate = static_cast<float>(barSize_ / MAX_HP);
	//現在のHP分かける
	float barNorm = barRate * hp_;
	//赤いゲージを出す分かける
	float barRed = barRate * autoHealHp_;

	DrawSoftImage(190, 0, hpBar_);

	//死んでないなら赤ゲージをだす
	if (hp_ - damaged_ > 0) {
	
		DrawBoxAA(190 + barSX_, barHpSY_, 190 + barSX_ + barRate * (hp_ - damaged_) + barRed, barHpEY_, 0xff0000, true);
	}
	DrawBoxAA(190 + barSX_, barHpSY_, 190 + barSX_ + barNorm, barHpEY_, 0x00ff00, true);

	//バー全体をスタミナの最大値分する
	barRate = barSize_ / static_cast<int>(MAX_STAMINA);
	//現在のスタミナ分かける
	barNorm = barRate * stamina_;

	if (isStaminaMax_) {

		DrawBoxAA(190 + barSX_, barStaSY_, 190 + barSX_ + barNorm, barStaEY_, GetColor(255, 255, (int)(std::abs(staminaMaxCnt_ % 101 - 50) * 5.1f)), true);
	}
	else {
		if (stamina_ <= DODGE_STAMINA) {

			DrawBoxAA(190 + barSX_, barStaSY_, 190 + barSX_ + barNorm, barStaEY_, 0xff0000, true);
		}
		else {
			DrawBoxAA(190 + barSX_, barStaSY_, 190 + barSX_ + barNorm, barStaEY_, 0xffff00, true);
		}
	}

	//一番長いサイズをゲージのマックス値分する
	float powerRate = (static_cast<float>(guageEX_.front()) - static_cast<float>(guageSX_)) / static_cast<float>(MAX_POWER);
	float power = static_cast<float>(guageSX_) + powerRate * static_cast<float>(power_);

	for (int y = 0; y < guageEX_.size(); y++) {

		int dy = y + guageSY_;
		DrawLineSoftImage(powerGauge_, guageSX_, dy, guageEX_.at(y), dy, 100, 100, 100, 255);

		if (!powerUp_) {
			if (power <= static_cast<float>(guageEX_.at(y))) {

				DrawLineSoftImage(powerGauge_, guageSX_, dy, static_cast<int>(power), dy, 255, 0, 0, 255);
			}
			else {

				DrawLineSoftImage(powerGauge_, guageSX_, dy, guageEX_.at(y), dy, 255, 0, 0, 255);
			}
		}
		else {
			int color = abs(powerUpCnt_ % 30 - 15) * 17;
			if (power <= static_cast<float>(guageEX_.at(y))) {

				DrawLineSoftImage(powerGauge_, guageSX_, dy, static_cast<int>(power), dy, 255, color, color, 255);
			}
			else {

				DrawLineSoftImage(powerGauge_, guageSX_, dy, guageEX_.at(y), dy, 255, color, color, 255);
			}
		}
	}
	DrawSoftImage(240, 100, powerGauge_);
}

void Player::ChangeWait(void) const
{
	//待機モーション
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
}

void Player::ChangeMove(void) const
{
}

void Player::ChangeAttack(void)
{
	if (!powerUp_) {
		//攻撃モーション
		animationCtrl_->Play(static_cast<int>(ANIM_TYPE::ATTACK), false);
	}
	else {
		//攻撃モーション
		animationCtrl_->Play(static_cast<int>(ANIM_TYPE::FINISH), false);
	}
}

void Player::ChangeCombo(void)
{
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::COMBO_1), false);
}

void Player::ChangeDodge(void)
{
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::DODGE), false);
	dodgeCnt_ = 0;
	dodgeFlg_ = true;
	if (!isStaminaMax_) {
	
		stamina_ -= DODGE_STAMINA;
	}
}

void Player::ChangeDamagedLight(void) const
{
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::DAMAGED_LIGHT), false);
}

void Player::ChangeDamagedHeavy(void) const
{
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::DAMAGED_HEAVY), false);
}

void Player::ChangeKO(void) const
{
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::KO), false);
}

void Player::ChangeDrink(void) const
{
	animationCtrl_->Play(static_cast<int>(ANIM_TYPE::DRINK), false);
	item_->Use();
}

void Player::UpdateWait(void)
{	
	//状態遷移の判断
	BoolChangeAttack();
	BoolChangeCombo();
	BoolChangeMove();
	BoolChangeDodge();
	BoolChangeDrink();
}

void Player::UpdateMove(void)
{
	//行列と方向の設定
	MATRIX mat = MGetIdent();
	VECTOR dir = { 0.0f, 0.0f, 0.0f };

	//前後左右の移動処理
	//方向の取得
	dir = InputManager::GetInstance().GetDirectionXZAKeyL().at(InputManager::GetInstance().GetMostPriority());
	
	if (!VectorUtility::EqualsVZero(dir)) {

		//カメラの角度を得る
		VECTOR angle = Camera::GetInstance()->GetCameraAngles();

		//回転行列を取る
		mat = MMult(mat, MGetRotY(angle.y));

		//進む向きに回転させる
		dir = VNorm(dir);
		moveDir_ = VTransform(dir, mat);

		if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::RUN).keyNew) {
			if (stamina_ > 0.0f) {

				//移動モーション
				animationCtrl_->Play(static_cast<int>(ANIM_TYPE::RUN), true);
				
				if (se_) {
					if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_WALK)) {

						AudioManager::GetInstance()->StopSE(SoundID::SE_WALK);
					}
					if (!AudioManager::GetInstance()->IsPlaySE(SoundID::SE_RUN)) {

						AudioManager::GetInstance()->PlaySE(SoundID::SE_RUN);
					}
				}

				//移動させる
				transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_ * 1.75f));

				if (!isStaminaMax_) {
					if (!powerUp_) {
					
						stamina_ -= 2;
					}
					else {

						stamina_--;
					}
				}
			}
			else {

				//移動モーション
				animationCtrl_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
				
				//移動させる
				transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_));
			}
		}
		else {

			//移動モーション
			animationCtrl_->Play(static_cast<int>(ANIM_TYPE::WALK), true);
			
			if (se_) {
				if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_RUN)) {

					AudioManager::GetInstance()->StopSE(SoundID::SE_RUN);
				}
				if (!AudioManager::GetInstance()->IsPlaySE(SoundID::SE_WALK)) {

					AudioManager::GetInstance()->PlaySE(SoundID::SE_WALK);
				}
			}
			//移動させる
			transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_));
		}

		// 状態遷移の判断
		BoolChangeAttack();
		BoolChangeCombo();
		BoolChangeDodge();
		BoolChangeDrink();

		transform_.rot.y = atan2f(moveDir_.x, moveDir_.z);
	}
	else {

		DoChangeState(STATE::WAIT);
	}
}

void Player::UpdateAttack(void)
{
	//パワーアップしていないときの連続攻撃
	if (!powerUp_) {
		//モーション時間に合わせて攻撃を発生させる
		if (animationCtrl_->GetTime() >= 34.5f && animationCtrl_->GetTime() <= 36.0f) {

			isAttack_ = true;
		}
		if (animationCtrl_->GetTime() >= 58.5f && animationCtrl_->GetTime() <= 60.0f) {

			isAttack_ = false;
		}
		if (animationCtrl_->GetTime() >= 60.5f && animationCtrl_->GetTime() <= 62.5f) {

			isAttack_ = true;
		}
		if (animationCtrl_->GetTime() >= 92.0f && animationCtrl_->GetTime() <= 94.0f) {

			isAttack_ = false;
		}
		if (animationCtrl_->GetTime() >= 138.0f && animationCtrl_->GetTime() <= 140.0f) {

			buff_ = 1.2;
			isAttack_ = true;
		}
		if (animationCtrl_->GetTime() >= 160.0f) {

			isAttack_ = false;
		}
		if (animationCtrl_->GetTime() <= 138.0f) {

			//移動させる
			transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_ * 0.2f));
		}
	}
	//パワーアップ時のゲージ全消費攻撃
	else {
		if (animationCtrl_->GetTime() >= 90.0f && animationCtrl_->GetTime() <= 91.5f) {

			transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_ * 0.8f));
			buff_ = 2.0;
			isAttack_ = true;
		}
		if (animationCtrl_->GetTime() >= 135.0f) {

			isAttack_ = false;
		}
		if (animationCtrl_->IsEnd()) {
		
			power_ = 0;
		}
	}

	if (animationCtrl_->IsEnd()) {

		buff_ = 1.0;
		isAttack_ = false;
		//待機モーションに移行
		DoChangeState(STATE::WAIT);
	}
}

void Player::UpdateCombo(void)
{
	if (animationCtrl_->GetPlayType() == static_cast<int>(ANIM_TYPE::COMBO_1)) {
		//モーション時間に合わせて攻撃を出す
		if (animationCtrl_->GetTime() >= 32.0f && animationCtrl_->GetTime() <= 34.0f) {

			isAttack_ = true;
		}
		//攻撃判定の消失
		if (animationCtrl_->GetTime() >= 67.5f) {
			
			isAttack_ = false;
			//回避キャンセル
			BoolChangeDodge();

			//同じボタンを押したら二段階目
			if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::COMBO).keyTrgDown) {

				animationCtrl_->Play(static_cast<int>(ANIM_TYPE::COMBO_2), false);
			}
		}
		//攻撃判定がなくなるまで前進させる
		else {

			transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_ * 0.3f));
		}
	}
	if (animationCtrl_->GetPlayType() == static_cast<int>(ANIM_TYPE::COMBO_2)) {
		//モーション時間に合わせて攻撃を出す
		if (animationCtrl_->GetTime() >= 40.0f && animationCtrl_->GetTime() <= 41.2f) {

			isAttack_ = true;

		}
		//攻撃判定の消失
		if (animationCtrl_->GetTime() >= 75.0f) {
			
			isAttack_ = false;
			//回避キャンセル
			BoolChangeDodge();
			
			//同じボタンを押したら三段階目
			if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::COMBO).keyTrgDown) {

				animationCtrl_->Play(static_cast<int>(ANIM_TYPE::COMBO_3), false);
			}
		}
		//攻撃判定がなくなるまで前進させる
		else {

			transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_ * 0.2f));
		}
	}
	if (animationCtrl_->GetPlayType() == static_cast<int>(ANIM_TYPE::COMBO_3)) {
		//モーション時間に合わせて攻撃を出す
		//最終段なので少しだけ攻撃力上昇
		if (animationCtrl_->GetTime() >= 42.8f && animationCtrl_->GetTime() <= 44.2f) {

			buff_ = 1.1;
			isAttack_ = true;
		}
		//攻撃判定の消失
		if (animationCtrl_->GetTime() >= 64.0f) {
			
			buff_ = 1.0;
			isAttack_ = false;
			//回避キャンセル
			BoolChangeDodge();
		}
		//攻撃判定がなくなるまで前進させる
		else {

			transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_ * 0.4f));
		}
	}
	if (animationCtrl_->IsEnd()) {

		isAttack_ = false;
		//待機モーションに移行
		DoChangeState(STATE::WAIT);
	}
}

void Player::UpdateDodge(void)
{
	//移動させる
	transform_.pos = VAdd(transform_.pos, VScale(moveDir_, speed_ * 1.25f));

	if (dodgeFlg_) {
		if (dodgeCnt_ <= 25.0f) {

			dodgeCnt_++;
		}
		else {

			dodgeFlg_ = false;
			dodgeCnt_ = 0;
		}
	}

	if (animationCtrl_->IsEnd()) {

		//待機モーションに移行
		DoChangeState(STATE::WAIT);

		greatDodge_ = goodDodge_ = dodge_ = false;
	}
}

void Player::UpdateDamagedLight(void)
{
	if (animationCtrl_->GetTime() >= 40) {

		DoChangeState(STATE::WAIT);
	}
}

void Player::UpdateDamagedHeavy(void)
{
	static int steps = 0;
	static bool prevPause = false;

	if (animationCtrl_->GetPlayType() == static_cast<int>(ANIM_TYPE::DAMAGED_HEAVY)) {
		if (animationCtrl_->GetTime() <= 110) {

			KnockBack();
		}
		if (!prevPause){
			//吹っ飛んだあとの起き上がりで
			//アニメーションを止める
			if (animationCtrl_->GetTime() >= 130) {
				
				prevPause = true;
				animationCtrl_->ChangePause(true);
			}
		}
		if (animationCtrl_->IsPause()) {

			steps++;

			//何かキーが押されるか一定時間経つまで起き上がらない
			if (InputManager::GetInstance().GetPriorityAnyoneTrg() || steps >= 90) {

				steps = 0;
				animationCtrl_->ChangePause(false);
			}
		}
		if (animationCtrl_-> GetTime() >= 160) {

			prevPause = false;
			animationCtrl_->Play(static_cast<int>(ANIM_TYPE::STAND_UP), false);
		}
	}
	if (animationCtrl_->GetPlayType() == static_cast<int>(ANIM_TYPE::STAND_UP)) {
		//起き上がりきるまで無敵
		if (animationCtrl_->GetTime() >= 210) {

			DoChangeState(STATE::WAIT);
		}
	}
}

void Player::UpdateKO(void)
{
	overFlg_ = true;
}

void Player::UpdateDrink(void)
{
	//飲むモーションが終わるまで回復せず数も減らない
	if (!animationCtrl_->IsEnd()) return;

	AudioManager::GetInstance()->PlaySE(SoundID::SE_HEAL);
	switch (item_->GetUseType())
	{
	case Item::TYPE::HP:

		isHeal_ = true;
		effectType_ = EFFECT::HEAL;
		
		break;

	case Item::TYPE::HP_MAX:
	
		isHealMax_ = true;
		effectType_ = EFFECT::HEAL;
		break;

	case Item::TYPE::STAMINA:

		isStaminaMax_ = true;
		effectType_ = EFFECT::STAMINA;
		break;
	}
	//エフェクト生成
	EffectCreate();
	DoChangeState(STATE::WAIT);
}

void Player::BoolChangeMove(void)
{
	if (!VectorUtility::EqualsVZero(InputManager::GetInstance().GetDirectionXZAKeyL().at(InputManager::GetInstance().GetMostPriority()))) {

		//移動モーションに移行
		DoChangeState(STATE::MOVE);
	}
}

void Player::BoolChangeAttack(void)
{
	if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::ATTACK).keyTrgDown) {

		//攻撃モーションに移行
		DoChangeState(STATE::ATTACK);
	}
}

void Player::BoolChangeCombo(void)
{
	if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::COMBO).keyTrgDown) {

		//攻撃モーションに移行
		DoChangeState(STATE::COMBO);
	}
}

void Player::BoolChangeDodge(void)
{
	if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::DODGE).keyTrgDown) {
		if (stamina_ >= dodgeStamina_ || isStaminaMax_) {

			//回避モーションに移行
			DoChangeState(STATE::DOGDE);
		}
	}
}

void Player::BoolChangeDrink(void)
{
	if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::USE).keyTrgDown) {
		if (!isHeal_ && !isHealMax_ && damaged_ <= 0) {

			//飲むモーションに移行
			DoChangeState(STATE::DRINK);
		}
	}
}

void Player::StopSE(void)
{
	if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_RUN)) {

		AudioManager::GetInstance()->StopSE(SoundID::SE_RUN);
	}
	if (AudioManager::GetInstance()->IsPlaySE(SoundID::SE_WALK)) {
				
		AudioManager::GetInstance()->StopSE(SoundID::SE_WALK);
	}
}

void Player::EffectCreate(void)
{
	effectSize_ = EFFECT_MAX_SIZE / 3;

	//エフェクトの数まで四方八方に生成する
	//(頂点シェーダを使いたかった)
	for (int i = 0; i < EFFECT_NUM; i++) {

		effectDir_[i].x = sinf(AngleUtility::Deg2RadF((float)GetRand(360)));
		effectDir_[i].y = sinf(AngleUtility::Deg2RadF((float)GetRand(360)));
		effectDir_[i].z = sinf(AngleUtility::Deg2RadF((float)GetRand(360)));

		effectTopPos_[i] = VAdd(VAdd(transform_.pos, { 0.0f, 70.0f, 0.0f }), VScale(effectDir_[i], effectSize_));
		effectBottomPos_[i] = VAdd(transform_.pos, { 0.0f, 70.0f, 0.0f });
	}
	effectCnt_ = 30;
}

void Player::EffectUpdate(void)
{
	if (effectCnt_ < 0) return;

	//サイズの更新
	effectSize_ += (EFFECT_MAX_SIZE - effectSize_) / 8;

	//場所の更新
	for (int i = 0; i < EFFECT_NUM; i++) {

		effectTopPos_[i] = VAdd(VAdd(transform_.pos, { 0.0f, 70.0f, 0.0f }), VScale(effectDir_[i], effectSize_));
		effectBottomPos_[i] = VAdd(transform_.pos, { 0.0f, 70.0f, 0.0f });
	}
	effectCnt_--;
}
