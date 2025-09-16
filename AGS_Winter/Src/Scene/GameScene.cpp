#include <EffekseerForDXLib.h>
#include "GameScene.h"
#include "../Object/Stage.h"
#include "../Object/Enemy/EnemyManager.h"
#include "../Object/Enemy/EnemyBase.h"
#include "../Object/Player.h"
#include "../Manager/InputManager.h"
#include "../Manager/EffectResManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Application.h"
#include "../Utility/AsoUtility.h"


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

	enemyManager_ = new EnemyManager();
	enemyManager_->Init();

	Camera* camera = SceneManager::GetInstance().GetCamera();
	prevCameraPos_ = camera->GetCameraPos();
	shakeFlg_ = false;
	cntShake_ = 0;

	player_ = new Player();
	player_->Init();

	cameraReset_ = true;
	GameCamera(player_->GetPos());
	cameraReset_ = true;

	imgGameover_ = LoadGraph((Application::PATH_IMAGE + "Gameover.png").c_str());
	imgGameclear_ = LoadGraph((Application::PATH_IMAGE + "Gameclear.png").c_str());
	imgNextwave_ = LoadGraph((Application::PATH_IMAGE + "NextWave.png").c_str());
	
	gameoverFlg_ = false;
	gameclearFlg_ = false;

	waveCnt_ = 0;
	waveFlg_ = false;
	time_ = 0;
}

void GameScene::Update(void)
{
	VECTOR playerPrevPos = player_->GetPos();
	player_->Update();

	if (!gameoverFlg_ && !gameclearFlg_ && !waveFlg_) {

		Collision();

		// ステージの更新
		stage_->Update();
		enemyManager_->Update(waveCnt_);

		if (shakeFlg_) {

			Camera* camera = SceneManager::GetInstance().GetCamera();
			cntShake_--;

			if (cntShake_ % 4 == 0) {
			
				VECTOR shake = { static_cast<float>(GetRand(30) - 15), static_cast<float>(GetRand(30) - 15), 0.0f };
				camera->SetCameraPos(VAdd(prevCameraPos_, shake));

			}
			if (cntShake_ < 0) {

				camera->SetCameraPos(prevCameraPos_);
				shakeFlg_ = false;
			}
		}

		if (enemyManager_->GetDeadCnt() == EnemyManager::SPAWN_MAX + 1) {
			if (++waveCnt_ < 2) {

				cntShake_ = -1;
				waveFlg_ = true;
				enemyManager_->Reset();
			}
			else {

				gameclearFlg_ = true;
			}
		}
		if (gameclearFlg_) {

			int resId = EffectResManager::GetInstance().GetResourceId(EffectResManager::TYPE::FIREWORKS);
			effectBlastPlayIdR_ = PlayEffekseer3DEffect(resId);

			// エフェクトの大きさ
			float SCALE = 10.0f;
			SetScalePlayingEffekseer3DEffect(effectBlastPlayIdR_, SCALE, SCALE, SCALE);

			// エフェクトの回転
			VECTOR angles = { 0.0f, 0.0f, 0.0f };
			SetRotationPlayingEffekseer3DEffect(effectBlastPlayIdR_, angles.x, angles.y, angles.z);

			// エフェクトの位置
			SetPosPlayingEffekseer3DEffect(effectBlastPlayIdR_, 200.0f, 0.0f, 50.0f);
			
			effectBlastPlayIdL_ = PlayEffekseer3DEffect(resId);

			// エフェクトの大きさ
			SetScalePlayingEffekseer3DEffect(effectBlastPlayIdL_, SCALE, SCALE, SCALE);

			// エフェクトの回転
			SetRotationPlayingEffekseer3DEffect(effectBlastPlayIdL_, angles.x, angles.y, angles.z);

			// エフェクトの位置
			SetPosPlayingEffekseer3DEffect(effectBlastPlayIdL_, -200.0f, 0.0f, 50.0f);
		}
	}
	else if (!waveFlg_){
		if (gameclearFlg_) {

			std::vector<EnemyBase*> enemys = enemyManager_->GetEnemys();

			for (EnemyBase* enemy : enemys) {
				if (enemy->IsDead()) {

					enemy->Update();
				}
			}
		}
		// シーン遷移
		InputManager& ins = InputManager::GetInstance();

		if (ins.IsTrgDown(KEY_INPUT_SPACE)) {

			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);

			if (IsEffekseer3DEffectPlaying(effectBlastPlayIdR_) == 0) {
			
				StopEffekseer3DEffect(effectBlastPlayIdR_);
			}
			if (IsEffekseer3DEffectPlaying(effectBlastPlayIdL_) == 0) {
			
				StopEffekseer3DEffect(effectBlastPlayIdL_);
			}
		}
	}
	if (waveFlg_) {

		std::vector<EnemyBase*> enemys = enemyManager_->GetEnemys();

		for (EnemyBase* enemy : enemys) {
			if (enemy->IsDead()) {

				enemy->Update();
			}
		}
		if (time_++ >= 90) {

			time_ = 0;
			waveFlg_ = false;
		}
	}
	GameCamera(playerPrevPos);
}

void GameScene::Collision(void)
{
	int modelId = stage_->GetModelId();

	// 敵を取得する
	std::vector<EnemyBase*> enemys = enemyManager_->GetEnemys();

	// 弾を取得する
	for (EnemyBase* enemy : enemys) {
		if (!enemy->IsCollisionState()) {

			// 爆発中や処理終了後は、以降の処理は実行しない
			continue;
		}
		// ゲームオーバー判定との衝突判定
		auto i = MV1CollCheck_Capsule(enemy->GetModelId(), -1, GAMEOVER_POINT, VAdd(GAMEOVER_POINT, { 0.0f, 200.0f, 0.0f }), GAMEOVER_RADIUS);
		if (i.HitNum > 0) {

			gameoverFlg_ = true;
		}
	}
}

void GameScene::GameCamera(VECTOR playerPos)
{
	Camera* camera = SceneManager::GetInstance().GetCamera();
	VECTOR headPos = VAdd(playerPos, { 0.0f, 150.0f, 0.0f });

	static bool first = true;

	static float yaw = 0.0f;   // 左右回転
	static float pitch = 0.3f;   // 上下回転（初期は少し見下ろす）

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

	camera->SetAbsCameraPos(newPos);
	camera->SetAbsCameraAngles({ pitch, yaw, 0.0f });

	//カメラの追従
	camera->SetCameraPos(VSub(player_->GetPos(), playerPos));
}

void GameScene::Draw(void)
{
	SceneBase::Draw();
	stage_->Draw();
	enemyManager_->Draw();
	player_->Draw();

	//std::vector<ShotBase*> shots = cannon_->GetShots();
	//for (ShotBase* shot : shots) {
	//	if (shot->GetType() == ShotBase::TYPE::BEAM) {
	//		if (shot->IsCollisionState()) {

	//			DrawCapsule3D(shot->GetPos(), VAdd(shot->GetPos(), VScale(shot->GetDirection(), 500.0f)), shot->GetCollisionRadius(), 16, 0xffffff, 0xffffff, false);
	//		}
	//	}
	//}

	if (gameoverFlg_) {

		DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 - 100, 1.0f, 0.0, imgGameover_, true);
	}
	if (gameclearFlg_) {

		DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 - 100, 1.0f, 0.0, imgGameclear_, true);
	}
	if (waveFlg_) {

		DrawRotaGraph(-342 + time_ * 20, Application::SCREEN_SIZE_Y / 2 - 100, 1.0f, 0.0, imgNextwave_, true);
	}
}

void GameScene::Release(void)
{
	stage_->Release();
	delete stage_;

	enemyManager_->Release();
	delete enemyManager_;

	player_->Release();
	delete player_;

	DeleteGraph(imgGameover_);
	DeleteGraph(imgGameclear_);
	DeleteGraph(imgNextwave_);
}