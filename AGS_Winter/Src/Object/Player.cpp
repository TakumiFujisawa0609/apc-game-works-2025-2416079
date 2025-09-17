#include "Player.h"
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "Common/AnimationController.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"


Player::Player(void)
{
}

Player::~Player(void)
{
}

void Player::Init()
{
	//モデルのロード
	modelId_ = MV1LoadModel((Application::PATH_MODEL + "Walking.mv1").c_str());

	//変数の初期化
	pos_ = DEFAULT_POS;
	angles_ = VAdd({ 0.0f, 0.0f, 0.0f }, DIFF_ANGLES);
	moveDir_ = angles_;
	scales_ = { 1.0f, 1.0f, 1.0f };

	speed_ = 5.0f;

	state_ = STATE::WAIT;

	//モデルの設定
	MV1SetPosition(modelId_, pos_);
	MV1SetRotationXYZ(modelId_, angles_);
	MV1SetScale(modelId_, scales_);

	//アニメーションのロード
	animationController_ = new AnimationController(modelId_);

	for (int i = 0; i < static_cast<int>(ANIM_TYPE::MAX); i++) {
		if (i <= 1) {
			
			animationController_->AddInFbx(i, 60.0f, i);
		}
		else {

			animationController_->Add(i, 60.0f, (Application::PATH_ANIMATION + "Sword And Shield Slash.mv1").c_str());
		}
	}
}

void Player::Update(void)
{
	//状態別更新処理
	switch (state_) {

	case STATE::WAIT:
		
		UpdateWait();
		break;

	case STATE::MOVE:

		UpdateMove();
		break;

	case STATE::ATTACK:
		
		UpdateAttack();
		break;
	}
	//モデルの設定
	MV1SetPosition(modelId_, pos_);
	MV1SetRotationXYZ(modelId_, angles_);
	MV1SetScale(modelId_, scales_);

	animationController_->Update();
}

void Player::ChangeState(STATE state)
{
}

void Player::Draw(void)
{
	//モデルの描画
	MV1DrawModel(modelId_);
	MV1DrawModel(bladeModel_);
	//DrawSphere3D(pos, 25.0f, 16, 0x00ff00, 0x00ff00, true);
}

void Player::Release(void)
{
	//アニメーションコントローラーのリリース
	animationController_->Release();
	delete animationController_;

	//モデルの削除
	MV1DeleteModel(modelId_);
}

VECTOR Player::GetCollisionPos(void)
{
	return VECTOR();
}

void Player::Damage(int damage)
{
}

bool Player::IsCollisionState(void)
{
	return false;
}

bool Player::IsDead(void)
{
	return false;
}

void Player::ChangeWait(void)
{
	//待機モーション
	animationController_->Play(static_cast<int>(ANIM_TYPE::T), true);

	state_ = STATE::WAIT;
}

void Player::ChangeMove(void)
{
	//移動モーション
	animationController_->Play(static_cast<int>(ANIM_TYPE::WALK), true);

	state_ = STATE::MOVE;
}

void Player::ChangeAttack(void)
{
	//攻撃モーション
	animationController_->Play(static_cast<int>(ANIM_TYPE::ATTACK), false);

	state_ = STATE::ATTACK;
}

void Player::ChangeEnd(void)
{
}

void Player::UpdateWait(void)
{
	if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
		
		//攻撃モーションに移行
		ChangeAttack();
	}
	if (CheckHitKey(KEY_INPUT_W) == 1 || CheckHitKey(KEY_INPUT_A) == 1 || CheckHitKey(KEY_INPUT_S) == 1 || CheckHitKey(KEY_INPUT_D) == 1) {

		//移動モーションに移行
		ChangeMove();
	}
}

void Player::UpdateMove(void)
{
	//行列と方向の設定
	MATRIX mat = MGetIdent();
	VECTOR dir = { 0.0f, 0.0f, 0.0f };

	//前後左右の移動処理
	if (CheckHitKey(KEY_INPUT_W) == 1) {

		dir = VAdd(dir,{ 0.0f, 0.0f, 1.0f });
	}
	if (CheckHitKey(KEY_INPUT_S) == 1) {

		dir = VAdd(dir, VTransform({ 0.0f, 0.0f, -1.0f }, mat));
	}
	if (CheckHitKey(KEY_INPUT_D) == 1) {

		dir = VAdd(dir, VTransform({ 1.0f, 0.0f, 0.0f }, mat));
	}
	if (CheckHitKey(KEY_INPUT_A) == 1) {

		dir = VAdd(dir, VTransform({ -1.0f, 0.0f, 0.0f }, mat));
	}

	if (!AsoUtility::EqualsVZero(dir)) {
	
		//カメラのY軸回転をもらう
		Camera* camera = SceneManager::GetInstance().GetCamera();
		mat = MMult(mat, MGetRotY(camera->GetCameraAngles().y));

		//行列回転を使って向きを帰る
		dir = VNorm(dir);
		moveDir_ = VTransform(dir, mat);
		
		//移動とプレイヤーを回転させる
		pos_ = VAdd(pos_, VScale(moveDir_, speed_));
		angles_.y = atan2f(moveDir_.x, moveDir_.z) + DX_PI_F;
	}
	else {

		//待機モーションに移行
		ChangeWait();
	}
}

void Player::UpdateAttack(void)
{
	//当たり判定の中心
	pos = MV1GetFramePosition(modelId_, 46);

	if (animationController_->IsEnd()) {

		//待機モーションに移行
		ChangeWait();
	}
}

void Player::UpdateEnd(void)
{
}

void Player::DrawStandby(void)
{
}

void Player::DrawDeadReact(void)
{
}

void Player::DrawHitReact(void)
{
}

void Player::DrawEnd(void)
{
}