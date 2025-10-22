#include <EffekseerForDXLib.h>
#include <cmath>
#include "../Object/Stage.h"
#include "../Object/Enemy/EnemyBase.h"
#include "../Object/Player.h"
#include "../Manager/Sound.h"
#include "../Manager/EffectResManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/Controller.h"
#include "../Application.h"
#include "../Utility/Utility.h"
#include "../Utility/AsoUtility.h"
#include "../Utility/AngleUtility.h"
#include "../Utility/VectorUtility.h"
#include "GameScene.h"


GameScene::GameScene(void)
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	// ステージの初期化
	stage_ = new Stage();
	stage_->Init();

	player_ = new Player();
	player_->Init();

	enemyBase_ = new EnemyBase(player_);
	enemyBase_->Init();

	pitch_ = 0.3f;
	yaw_ = 0.0f;
	isFirst_ = false;
	isLockon_ = false;
		
	GameCamera();

	imgGameover_ = LoadGraph((Application::PATH_IMAGE + "Gameover.png").c_str());
	imgGameclear_ = LoadGraph((Application::PATH_IMAGE + "Gameclear.png").c_str());
	imgNextwave_ = LoadGraph((Application::PATH_IMAGE + "NextWave.png").c_str());

	hitFlgE_ = false;
	hitFlgP_ = false;

	shadowMap_ = MakeShadowMap(8192, 8192);

	SetShadowMapLightDirection(shadowMap_, { 0.2f, -0.8f, 0.0f });
	SetShadowMapDrawArea(shadowMap_, { -5000.0f, 0.0f, -5000.0f }, { 5000.0f, 0.0f, 5000.0f });
}

void GameScene::Update(void)
{
	player_->Update();
	enemyBase_->Update();
	GameCamera();

	// ステージの更新
	stage_->Update();

	Collision();

	if (enemyBase_->ClearFlg() == true) {

		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CLEAR);
	}
	if (player_->OverFlg() == true) {

		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::OVER);
	}
}

void GameScene::Collision(void)
{
	Sound& sound = Sound::GetInstance();
	MV1_COLL_RESULT_POLY_DIM info{};

	if (player_->IsAttack()) {
		
		info = MV1CollCheck_Capsule(enemyBase_->GetModelId(), -1, player_->GetAttackStartPos(), player_->GetAttackEndPos(), 10.0f);

		if (!hitFlgE_) {
			if (info.HitNum > 0) {
			
				sound.Play(Sound::SE_TYPE::E_SE_SLASH_1);
				Effect(info.Dim[info.HitNum - 1]);

				hitFlgE_ = true;
				enemyBase_->Damage(player_->GetPower());

				if (player_->GetPower() > 1) {

					player_->SetPower(1);
				}
			}
		}
	}
	else {

		hitFlgE_ = false;
	}
	if (enemyBase_->IsAttackA()) {

		info = MV1CollCheck_Sphere(player_->GetModelId(), -1, enemyBase_->GetAttackStartPos(), EnemyBase::ATTACK_RADIUS);

		if (info.HitNum > 0) {
			if (!hitFlgP_) {
				if (!player_->IsDodge()) {

					hitFlgP_ = true;
					player_->Damage(1, enemyBase_->GetAngle().y);
				}
				else {
					if (player_->DodgeCount() <= 10) {

						player_->SetPower(3);
					}
					else if (player_->DodgeCount() <= 20) {

						player_->SetPower(2);
					}
				}
			}
		}
	}
	if (enemyBase_->IsAttackB()) {

		info = MV1CollCheck_Capsule(player_->GetModelId(), -1, enemyBase_->GetAttackStartPos(),enemyBase_->GetAttackEndPos(), EnemyBase::ATTACK_RADIUS);

		if (info.HitNum > 0) {
			if (!hitFlgP_) {
				if (!player_->IsDodge()) {

					hitFlgP_ = true;
					player_->Damage(1, enemyBase_->GetAngle().y);
				}
				else {
					if (player_->DodgeCount() <= 10) {

						player_->SetPower(3);
					}
					else if (player_->DodgeCount() <= 20) {

						player_->SetPower(2);
					}
				}
			}
		}
	}
	if (enemyBase_->IsAttackC()) {

		info = MV1CollCheck_Capsule(player_->GetModelId(), -1, enemyBase_->GetAttackStartPos(),enemyBase_->GetAttackEndPos(), EnemyBase::ATTACK_RADIUS * 2);

		if (info.HitNum > 0) {
			if (!hitFlgP_) {
				if (!player_->IsDodge()) {

					hitFlgP_ = true;
					player_->Damage(2, enemyBase_->GetAngle().y);
				}
				else {
					if (player_->DodgeCount() <= 10) {

						player_->SetPower(3);
					}
					else if (player_->DodgeCount() <= 20) {

						player_->SetPower(2);
					}
				}
			}
		}
	}
	if (!enemyBase_->IsAttack()) {

		hitFlgP_ = false;
	}
}

void GameScene::GameCamera(void)
{
	//カメラのインスタンスとプレイヤーの頭の位置を取る
	Camera* camera = SceneManager::GetInstance().GetCamera();
	VECTOR headPos = VAdd(player_->GetPos(), { 0.0f, 150.0f, 0.0f });

	// 上下左右回転

	if (!isFirst_) {

		// カメラの位置を計算
		VECTOR newPos;
		newPos.x = headPos.x - CAMERA_TO_PLAYER * cosf(pitch_) * sinf(yaw_);
		newPos.y = headPos.y + CAMERA_TO_PLAYER * sinf(pitch_);
		newPos.z = headPos.z - CAMERA_TO_PLAYER * cosf(pitch_) * cosf(yaw_);

		//カメラの位置の設定
		camera->SetAbsCameraPos(newPos);
		camera->SetAbsCameraAngles({ pitch_, yaw_, 0.0f });

		isFirst_ = true;

		return;
	}

	////前後左右の移動処理
	//if (GetJoypadNum() == 0) {

	//	if (CheckHitKey(KEY_INPUT_UP) == 1) {

	//		pitch += 0.1f;
	//	}
	//	if (CheckHitKey(KEY_INPUT_DOWN) == 1) {

	//		pitch -= 0.1f;
	//	}
	//	if (CheckHitKey(KEY_INPUT_RIGHT) == 1) {

	//		yaw_ += 0.1f;
	//	}
	//	if (CheckHitKey(KEY_INPUT_LEFT) == 1) {

	//		yaw_ -= 0.1f;
	//	}
	//}
	//else {
	//}

	Controller& ctrl = Controller::GetInstance();
	//ゲームパッドの情報を取得
	Controller::JOYPAD_IN_STATE padState = ctrl.GetJPadState(Controller::JOYPAD_NO::PAD1);

	if (!isLockon_) {
	
		//方向の取得
		pitch_ += padState.AKeyRY / 25000.0f;
		yaw_ += padState.AKeyRX / 12000.0f;

		if (padState.IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::L)] || CheckHitKey(KEY_INPUT_O)) {

			isLockon_ = true;
		}
	}
	else {

		VECTOR dir = VSub(VAdd(enemyBase_->GetPos(), { 0.0f, 50.0f, 0.0f }), headPos);

		float prevPitch = pitch_;
		float prevYaw = yaw_;

		pitch_ = -VNorm(dir).y;
		yaw_ = atan2f(VNorm(dir).x, VNorm(dir).z);

		pitch_ = AngleUtility::LerpAngle(prevPitch, pitch_, 0.8f);
		yaw_ = AngleUtility::LerpAngle(prevYaw, yaw_, 0.8f);

		if (((std::abs(prevPitch - pitch_) < 0.1f && std::abs(prevYaw - yaw_) < 0.1f)) || yaw_ < 0.2f) {

			isLockon_ = false;
		}
	}

	// ピッチに制限（真上と床下を防ぐ）
	if (pitch_ > DX_PI_F / 2.0f - 0.1f) {

		pitch_ = DX_PI_F / 2.0f - 0.1f;
	}
	if (pitch_ < -DX_PI_F / 18.0f) {

		pitch_ = -DX_PI_F / 18.0f;
	}

	// カメラの位置を計算
	VECTOR newPos;
	newPos.x = headPos.x - CAMERA_TO_PLAYER * cosf(pitch_) * sinf(yaw_);
	newPos.y = headPos.y + CAMERA_TO_PLAYER * sinf(pitch_);
	newPos.z = headPos.z - CAMERA_TO_PLAYER * cosf(pitch_) * cosf(yaw_);

	//カメラの位置の設定
	camera->SetAbsCameraPos(newPos);
	camera->SetAbsCameraAngles({ pitch_, yaw_, 0.0f });
}

void GameScene::Effect(MV1_COLL_RESULT_POLY dim)
{
	//リソースを得る
	int resource = EffectResManager::GetInstance().GetResourceId(EffectResManager::TYPE::ENEMY_HIT);
	
	//エフェクトの再生
	int effect = PlayEffekseer3DEffect(resource);

	//位置等々の設定
	VECTOR pos = dim.Position[0];
	SetPosPlayingEffekseer3DEffect(effect, pos.x, pos.y, pos.z);
	SetScalePlayingEffekseer3DEffect(effect, 12.0f, 12.0f, 12.0f);
	SetRotationPlayingEffekseer3DEffect(effect, 0.0f, 0.0f, 0.0f);
}

void GameScene::Draw(void)
{
	ShadowMap_DrawSetup(shadowMap_);

	MV1DrawModel(player_->GetModelId());
	MV1DrawModel(enemyBase_->GetModelId());

	ShadowMap_DrawEnd();

	SetUseShadowMap(0, shadowMap_);

	SceneBase::Draw();
	stage_->Draw();
	player_->Draw();
	enemyBase_->Draw();

	SetUseShadowMap(0, -1);

	//if (hitFlgP_) {

	//	DrawString(0, 0, "あたった", 0xff00ff);
	//}

	//std::vector<ShotBase*> shots = cannon_->GetShots();
	//for (ShotBase* shot : shots) {
	//	if (shot->GetType() == ShotBase::TYPE::BEAM) {
	//		if (shot->IsCollisionState()) {

	//			DrawCapsule3D(shot->GetPos(), VAdd(shot->GetPos(), VScale(shot->GetDirection(), 500.0f)), shot->GetCollisionRadius(), 16, 0xffffff, 0xffffff, false);
	//		}
	//	}
	//}
}

void GameScene::Release(void)
{
	DeleteShadowMap(shadowMap_);

	stage_->Release();
	delete stage_;

	player_->Release();
	delete player_;

	enemyBase_->Release();
	delete enemyBase_;

	DeleteGraph(imgGameover_);
	DeleteGraph(imgGameclear_);
	DeleteGraph(imgNextwave_);
}

void GameScene::EnemyToPlayer(void)
{
	MATRIX angles = AngleUtility::GetMatrixRotateXYZ(VSub(player_->GetPos(), enemyBase_->GetPos()));
	enemyBase_->SetAngle(VTransform(enemyBase_->GetAngle(), angles));
}
