#include <EffekseerForDXLib.h>
#include "../Object/Stage.h"
#include "../Object/Enemy/EnemyBase.h"
#include "../Object/Player.h"
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

	GameCamera();

	imgGameover_ = LoadGraph((Application::PATH_IMAGE + "Gameover.png").c_str());
	imgGameclear_ = LoadGraph((Application::PATH_IMAGE + "Gameclear.png").c_str());
	imgNextwave_ = LoadGraph((Application::PATH_IMAGE + "NextWave.png").c_str());

	hitFlgE_ = false;
	hitFlgP_ = false;
}

void GameScene::Update(void)
{
	player_->Update();
	enemyBase_->Update();
	GameCamera();

	// ステージの更新
	stage_->Update();

	Collision();
}

void GameScene::Collision(void)
{
	MV1_COLL_RESULT_POLY_DIM info{};

	if (player_->IsAttack()) {
		
		info = MV1CollCheck_Capsule(enemyBase_->GetModelId(), -1, player_->GetAttackStartPos(), player_->GetAttackEndPos(), 10.0f);

		if (info.HitNum > 0) {
			if (!hitFlgE_) {
			
				hitFlgE_ = true;
				enemyBase_->Damage(player_->GetPower());

				if (player_->GetPower() > 1) {

					player_->SetPower(1);
				}
			}
			if (enemyBase_->ClearFlg() == true) {

				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
			}
		}
		else {

			hitFlgE_ = false;
		}
	}
	if (enemyBase_->IsAttack() == true) {

		info = MV1CollCheck_Capsule(player_->GetModelId(), -1, enemyBase_->GetAttackStartPos(), enemyBase_->GetAttackEndPos(), 20.0f);

		if (info.HitNum > 0) {
			if (player_->IsDodge() == false) {
				if (!hitFlgP_) {

					hitFlgP_ = true;
					player_->Damage(1);
				}
			}
			else {
				if (player_->DodgeCount() <= 10) {
				
					player_->SetPower(3);
				}
				else if (player_->DodgeCount() <= 20) {
					
					player_->SetPower(2);
				}
			}
			if (player_->OverFlg() == true) {

				SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
			}
		}
	}
	else {

		hitFlgP_ = false;
	}
}

void GameScene::GameCamera(void)
{
	//カメラのインスタンスとプレイヤーの頭の位置を取る
	Camera* camera = SceneManager::GetInstance().GetCamera();
	VECTOR headPos = VAdd(player_->GetPos(), {0.0f, 150.0f, 0.0f});

	// 上下左右回転
	static float yaw = 0.0f; 
	static float pitch = 0.3f;

	static bool isFirst = false;

	if (!isFirst) {

		// カメラの位置を計算
		VECTOR newPos;
		newPos.x = headPos.x - CAMERA_TO_PLAYER * cosf(pitch) * sinf(yaw);
		newPos.y = headPos.y + CAMERA_TO_PLAYER * sinf(pitch);
		newPos.z = headPos.z - CAMERA_TO_PLAYER * cosf(pitch) * cosf(yaw);

		//カメラの位置の設定
		camera->SetAbsCameraPos(newPos);
		camera->SetAbsCameraAngles({ pitch, yaw, 0.0f });

		isFirst = true;

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

	//		yaw += 0.1f;
	//	}
	//	if (CheckHitKey(KEY_INPUT_LEFT) == 1) {

	//		yaw -= 0.1f;
	//	}
	//}
	//else {
	//}

	Controller& ctrl = Controller::GetInstance();
	//ゲームパッドの情報を取得
	Controller::JOYPAD_IN_STATE padState = ctrl.GetJPadInputState(Controller::JOYPAD_NO::PAD1);
	//方向の取得
	pitch += padState.AKeyRY / 25000.0f;
	yaw += padState.AKeyRX / 12000.0f;

	if (padState.ButtonsNew[9]) {

		VECTOR dir = VSub(VAdd(enemyBase_->GetPos(), { 0.0f, 50.0f, 0.0f }), headPos);
		VECTOR newPos = Utility::VECTOR_ZERO;

		pitch = -VNorm(dir).y;
		yaw = atan2f(VNorm(dir).x, VNorm(dir).z);
	}

	// ピッチに制限（真上と床下を防ぐ）
	if (pitch > DX_PI_F / 2.0f - 0.1f) {

		pitch = DX_PI_F / 2.0f - 0.1f;
	}
	if (pitch < -DX_PI_F / 18.0f) {

		pitch = -DX_PI_F / 18.0f;
	}

	// カメラの位置を計算
	VECTOR newPos;
	newPos.x = headPos.x - CAMERA_TO_PLAYER * cosf(pitch) * sinf(yaw);
	newPos.y = headPos.y + CAMERA_TO_PLAYER * sinf(pitch);
	newPos.z = headPos.z - CAMERA_TO_PLAYER * cosf(pitch) * cosf(yaw);

	//カメラの位置の設定
	camera->SetAbsCameraPos(newPos);
	camera->SetAbsCameraAngles({ pitch, yaw, 0.0f });
}

void GameScene::Draw(void)
{
	SceneBase::Draw();
	stage_->Draw();
	player_->Draw();
	enemyBase_->Draw();

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
