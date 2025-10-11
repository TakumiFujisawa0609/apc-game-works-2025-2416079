#include <EffekseerForDxlib.h>
#include "Camera.h"
#include "../Object/Grid.h"
#include "../Common/Fader.h"
#include "../Scene/TitleScene.h"
#include "../Scene/GameScene.h"
#include "../Application.h"
#include "SceneManager.h"

SceneManager* SceneManager::instance_ = nullptr;

void SceneManager::CreateInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new SceneManager();
	}
	instance_->Init();
}

SceneManager& SceneManager::GetInstance(void)
{
	return *instance_;
}

void SceneManager::Init(void)
{
	sceneId_ = SCENE_ID::TITLE;
	waitSceneId_ = SCENE_ID::NONE;

	camera_ = new Camera();
	camera_->Init();

	grid_ = new Grid();
	grid_->Init();

	// フェード機能の初期化
	fader_ = new Fader();
	fader_->Init();

	isSceneChanging_ = false;

	// デルタタイム
	preTime_ = std::chrono::system_clock::now();

	// 3D描画の初期化
	Init3D();

	// 初期シーンの設定
	DoChangeScene(SCENE_ID::TITLE);
}

void SceneManager::Update(void)
{
	if (scene_ == nullptr){

		return;
	}

	// デルタタイム
	auto nowTime = std::chrono::system_clock::now();
	deltaTime_ = static_cast<float>( std::chrono::duration_cast<std::chrono::nanoseconds>(nowTime - preTime_).count() / 1000000000.0);
	preTime_ = nowTime;

	// フェード機能の更新
	fader_->Update();

	// カメラの更新
	camera_->Update();

	if (isSceneChanging_){
		
		// フェード状態の切替処理
		Fade();
	}
	else{
		// 各シーンの更新処理
		scene_->Update();
	}

}

void SceneManager::Draw(void)
{
	
	// 描画先グラフィック領域の指定
	// (３Ｄ描画で使用するカメラの設定などがリセットされる)
	SetDrawScreen(DX_SCREEN_BACK);

	// 画面を初期化
	ClearDrawScreen();

	// 3D描画の初期化
	camera_->SetBeforeDraw();

	UpdateEffekseer3D();

	// 各シーンの描画処理
	scene_->Draw();

	// グリッドの描画
	grid_->Draw();

	// カメラのデバック描画
	//camera_->Draw();

	DrawEffekseer3D();

	// 暗転・明転
	fader_->Draw();

//#pragma region Step1 ポイントライト
//	if (CheckHitKey(KEY_INPUT_T)) { pointLightPos_.z += 3.0f; }
//	if (CheckHitKey(KEY_INPUT_G)) { pointLightPos_.z -= 3.0f; }
//	if (CheckHitKey(KEY_INPUT_R)) { pointLightPos_.y += 3.0f; }
//	if (CheckHitKey(KEY_INPUT_Y)) { pointLightPos_.y -= 3.0f; }
//	if (CheckHitKey(KEY_INPUT_H)) { pointLightPos_.x += 3.0f; }
//	if (CheckHitKey(KEY_INPUT_F)) { pointLightPos_.x -= 3.0f; }
//	SetLightPosition(pointLightPos_);
//#pragma endregion

//#pragma region Step2 スポットライト
//	if (CheckHitKey(KEY_INPUT_T)) { spotLightPos_.z += 3.0f; }
//	if (CheckHitKey(KEY_INPUT_G)) { spotLightPos_.z -= 3.0f; }
//	if (CheckHitKey(KEY_INPUT_R)) { spotLightPos_.y += 3.0f; }
//	if (CheckHitKey(KEY_INPUT_Y)) { spotLightPos_.y -= 3.0f; }
//	if (CheckHitKey(KEY_INPUT_H)) { spotLightPos_.x += 3.0f; }
//	if (CheckHitKey(KEY_INPUT_F)) { spotLightPos_.x -= 3.0f; }
//	SetLightPosition(spotLightPos_);
//	DrawSphere3D(spotLightPos_, 20.0f, 10, 0xff0000, 0xff0000, true);
//#pragma endregion
}

void SceneManager::Destroy(void)
{
	camera_->Release();
	delete camera_;

	grid_->Release();
	delete grid_;

	// シーンの解放
	scene_->Release();
	delete scene_;

	// フェード機能の解放
	delete fader_;

	// インスタンスのメモリ解放
	delete instance_;

	DeleteLightHandle(pointLight1_);
	DeleteLightHandle(pointLight2_);
}

void SceneManager::ChangeScene(SCENE_ID nextId)
{
	// フェード処理が終わってからシーンを変える場合もあるため、
	// 遷移先シーンをメンバ変数に保持
	waitSceneId_ = nextId;

	// フェードアウト(暗転)を開始する
	fader_->SetFade(Fader::STATE::FADE_OUT);
	isSceneChanging_ = true;
}

SceneManager::SCENE_ID SceneManager::GetSceneID(void)
{
	return sceneId_;
}

float SceneManager::GetDeltaTime(void) const
{
	//return 1.0f / 60.0f;
	return deltaTime_;
}

SceneManager::SceneManager(void)
{

	sceneId_ = SCENE_ID::NONE;
	waitSceneId_ = SCENE_ID::NONE;

	scene_ = nullptr;
	fader_ = nullptr;

	isSceneChanging_ = false;

	// デルタタイム
	deltaTime_ = 1.0f / Application::FPS;

}

void SceneManager::ResetDeltaTime(void)
{
	deltaTime_ = 1.0f / Application::FPS;
	preTime_ = std::chrono::system_clock::now();
}

void SceneManager::DoChangeScene(SCENE_ID sceneId)
{
	// シーンを変更する
	sceneId_ = sceneId;

	// 現在のシーンを解放
	if (scene_ != nullptr){

		scene_->Release();
		delete scene_;
	}

	switch (sceneId_) {
	case SCENE_ID::TITLE:

		scene_ = new TitleScene();
		break;

	case SCENE_ID::GAME:

		scene_ = new GameScene();
		break;
	}

	// 各シーンの初期化
	scene_->Init();
	camera_->SetBeforeDraw();

	ResetDeltaTime();

	waitSceneId_ = SCENE_ID::NONE;

}

void SceneManager::Fade(void)
{

	Fader::STATE fState = fader_->GetState();
	switch (fState){
	case Fader::STATE::FADE_IN:
		// 明転中
		if (fader_->IsEnd()){

			// 明転が終了したら、フェード処理終了
			fader_->SetFade(Fader::STATE::NONE);
			isSceneChanging_ = false;
		}
		break;

	case Fader::STATE::FADE_OUT:
		// 暗転中
		if (fader_->IsEnd()){

			// カメラの初期化
			camera_->Init();
			// 完全に暗転してからシーン遷移
			DoChangeScene(waitSceneId_);
			// 暗転から明転へ
			fader_->SetFade(Fader::STATE::FADE_IN);
		}
		break;
	}

}

void SceneManager::Init3D(void)
{
	//	背景色の設定
	SetBackgroundColor(200, 200, 200);

	// Zバッファの設定
	SetUseZBuffer3D(true);
	SetWriteZBuffer3D(true);

	// カリング設定
	SetUseBackCulling(true);

	// ライト設定
	SetUseLighting(true);
	ChangeLightTypeDir({ 0.00f, -100.00f, 0.00f });

	//#pragma region Step1 ポイントライト
	//
	//	pointLightPos_ = { 0.0f, 40.0f, 180.0f };
	//	ChangeLightTypePoint(pointLightPos_, 400.0f, 0.000f, 0.001f, 0.000f);
	//	
	//	// 追加ポイントライト１
	//	pointLight1Pos_ = { -390.0f, 100.0f, 50.0f };
	//	pointLight1_ = CreatePointLightHandle(pointLight1Pos_, 100.0f, 0.000f, 0.002f, 0.000f);
	//
	//	// 追加ポイントライト２
	//	pointLight2Pos_ = { 390.0f, 100.0f, 50.0f };
	//	pointLight2_ = CreatePointLightHandle(pointLight2Pos_, 100.0f, 0.000f, 0.002f, 0.000f);
	//
	//#pragma endregion

	//#pragma region Step2 スポットライト
	//	spotLightPos_ = { 500.0f, 300.0f, 45.0f };
	//	ChangeLightTypeSpot(spotLightPos_,{ 0.0f, 0.0f, 1.0f }, 360.0f * DX_PI_F / 180.0f, 0.0f * DX_PI_F / 180.0f,	200.0f, 0.000f, 0.001f, 0.000f);
	//#pragma endregion

	//// ディフューズカラー
	//SetLightDifColor(GetColorF(0.3f, 0.3f, 0.8f, 1.0f));

	// フォグ設定
	SetFogEnable(true);
	// フォグの色
	SetFogColor(200, 200, 200);
	// フォグを発生させる奥行きの最小、最大距離
	SetFogStartEnd(10000.0f, 12000.0f);
}