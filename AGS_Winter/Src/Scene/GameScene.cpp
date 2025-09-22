#include <EffekseerForDXLib.h>
#include "../Object/Stage.h"
#include "../Object/Enemy/EnemyBase.h"
#include "../Object/Player.h"
#include "../Manager/InputManager.h"
#include "../Manager/EffectResManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Utility/AngleUtility.h"
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

	cameraReset_ = true;
	GameCamera();
	cameraReset_ = true;

	imgGameover_ = LoadGraph((Application::PATH_IMAGE + "Gameover.png").c_str());
	imgGameclear_ = LoadGraph((Application::PATH_IMAGE + "Gameclear.png").c_str());
	imgNextwave_ = LoadGraph((Application::PATH_IMAGE + "NextWave.png").c_str());
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

			hitFlg_ = true;
		}
		else {

			hitFlg_ = false;
		}
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

	//画面の中央の座標
	const int centerX = Application::SCREEN_SIZE_X / 2;
	const int centerY = Application::SCREEN_SIZE_Y / 2;

	int mouseX, mouseY;

	// 初回は基準をセットして抜ける
	if (cameraReset_) {

		// 中心に戻す
		SetMousePoint(centerX, centerY);
		yaw = 0.0f;
		pitch = 0.3f;
		cameraReset_ = false;
	}
	GetMousePoint(&mouseX, &mouseY);

	// 相対移動量を計算
	int deltaX = mouseX - centerX;
	int deltaY = mouseY - centerY;

	// 中心に戻す
	SetMousePoint(centerX, centerY);

	yaw += deltaX * SENSITIVITY;
	pitch += deltaY * SENSITIVITY;

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

	if (hitFlg_) {

		DrawString(0, 0, "あたった", 0xff00ff);
	}

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
