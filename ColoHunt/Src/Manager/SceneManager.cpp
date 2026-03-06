#include <EffekseerForDxlib.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "Camera.h"
#include "../Utility/Utility.h"
#include "../Object/Grid.h"
#include "../Common/Fader.h"
#include "../Scene/TitleScene.h"
#include "../Scene/GameScene.h"
#include "../Scene/Result/GameOver.h"
#include "../Scene/Result/GameClear.h"
#include "../Application.h"
#include "Input/Controller.h"
#include "Audio/AudioManager.h"
#include "Pause.h"
#include "SceneManager.h"

SceneManager* SceneManager::instance_ = nullptr;

void SceneManager::CreateInstance()
{
	instance_ = new SceneManager();

	instance_->Init();
}

SceneManager& SceneManager::GetInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new SceneManager();
	}
	return *instance_;
}

SceneManager::SceneManager(void):timer_(0), waitSceneId_(SCENE_ID::NONE), time_(0), showFlg_(false), screenImg_(-1)
{
}

void SceneManager::Init(void)
{
	sceneId_ = SCENE_ID::NONE;
	waitSceneId_ = SCENE_ID::TITLE;

	grid_ = new Grid();
	grid_->Init();

	// フェード機能の初期化
	fader_ = new Fader();
	fader_->Init();
	
	Camera::GetInstance()->Init();

	pause_ = new Pause();

	backGround_ = MV1LoadModel((Application::PATH_MODEL + "Sky.mv1").c_str());
	loadImg_ = LoadGraph((Application::PATH_IMAGE + "Load.png").c_str());
	font_ = CreateFontToHandle("アンニャントロマン", 35, 1, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);

	MV1SetPosition(backGround_, Utility::VECTOR_ZERO);
	MV1SetRotationXYZ(backGround_, Utility::VECTOR_ZERO);
	MV1SetScale(backGround_, BACKGROUND_SCR);

	isSceneChanging_ = true;
	isLoad_ = false;
	screenImg_ = MakeGraph(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y);

	// デルタタイム
	preTime_ = std::chrono::system_clock::now();

	damageNum_ = 0;

	// 3D描画の初期化
	Init3D();

	DoChangeScene(waitSceneId_);
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

	if (isSceneChanging_){
		
		// フェード状態の切替処理
		Fade();
	}
	else{
		if (pause_->IsPause()) {

			pause_->Update();

			if (!pause_->IsPause()) {

				AudioManager::GetInstance()->PlayBGM(pauseId_);
			}
		}
		else {
			if (Controller::GetInstance().GetJPadState(Controller::JOYPAD_NO::PAD1).IsTrgDown[static_cast<int>(Controller::JOYPAD_BTN::START)]) {
				if (sceneId_ == SCENE_ID::GAME) {

					AudioManager::GetInstance()->StopSE();
					pauseId_ = AudioManager::GetInstance()->PauseBGM();
					AudioManager::GetInstance()->PlaySE(SoundID::SE_PAUSE);
					pause_->Init();
					return;
				}
			}
			// 各シーンの更新処理
			scene_->Update();
			timer_++;
		}
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
	Camera::GetInstance()->SetBeforeDraw();

	if (!pause_->IsPause()) {

		UpdateEffekseer3D();
	}

	SetUseLighting(false);
	MV1DrawModel(backGround_);
	SetUseLighting(true);

	// 各シーンの描画処理
	if (!isLoad_) {

		scene_->Draw();
	}

	if (pause_->IsPause()) {

		pause_->Draw();
	}

	// 暗転・明転
	fader_->Draw();

	if (isLoad_) {

		DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
		DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, 0.95, 0.0, loadImg_, true);
		DrawFormatStringToHandle(Application::SCREEN_SIZE_X - 270, Application::SCREEN_SIZE_Y - 75, 0xdddddd, font_, "LOADING");
		if (showFlg_[0]) {

			DrawFormatStringToHandle(Application::SCREEN_SIZE_X - 75, Application::SCREEN_SIZE_Y - 75, 0xdddddd, font_, ".");
		}
		if (showFlg_[1]) {

			DrawFormatStringToHandle(Application::SCREEN_SIZE_X - 50, Application::SCREEN_SIZE_Y - 75, 0xdddddd, font_, ".");
		}
		if (showFlg_[2]) {

			DrawFormatStringToHandle(Application::SCREEN_SIZE_X - 25, Application::SCREEN_SIZE_Y - 75, 0xdddddd, font_, ".");
		}
	}
}

void SceneManager::Destroy(void)
{
	grid_->Release();
	delete grid_;

	// シーンの解放
	scene_->Release();
	delete scene_;

	pause_->Release();
	delete pause_;

	// フェード機能の解放
	delete fader_;

	// インスタンスのメモリ解放
	delete instance_;

	DeleteGraph(screenImg_);
}

void SceneManager::ChangeScene(SCENE_ID nextId)
{
	// フェード処理が終わってからシーンを変える場合もあるため、
	// 遷移先シーンをメンバ変数に保持
	waitSceneId_ = nextId;

	// フェードアウト(暗転)を開始する
	timer_ = 0;
	fader_->SetFade(Fader::STATE::FADE_OUT);
	isSceneChanging_ = true;
}

SceneManager::SCENE_ID SceneManager::GetSceneID(void) const
{
	return sceneId_;
}

float SceneManager::GetDeltaTime(void) const
{
	//return 1.0f / 60.0f;
	return deltaTime_;
}

void SceneManager::SetScreenImage(void) const
{
	GetDrawScreenGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, screenImg_);
}

void SceneManager::ResetDeltaTime(void)
{
	deltaTime_ = 1.0f / Application::FPS;
	preTime_ = std::chrono::system_clock::now();
}

void SceneManager::DoChangeScene(SCENE_ID sceneId)
{
	loadCnt_++;

	if (loadCnt_ == 3) {

		showFlg_[0] = true;
	}
	if (loadCnt_ == 6) {

		showFlg_[1] = true;
	}
	if (loadCnt_ == 9) {

		showFlg_[2] = true;
	}
	if (loadCnt_ == 12) {

		showFlg_[0] = false;
		showFlg_[1] = false;
		showFlg_[2] = false;
	}
	if (sceneId_ != sceneId) {
		if (sceneId_ == SCENE_ID::OVER || sceneId_ == SCENE_ID::CLEAR) {

			AudioManager::GetInstance()->StopBGM();
		}
		// シーンを変更する
		sceneId_ = sceneId;

		// 現在のシーンを解放
		if (scene_ != nullptr) {
				
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

		case SCENE_ID::OVER:

			scene_ = new GameOver();
			break;

		case SCENE_ID::CLEAR:

			scene_ = new GameClear();
			SaveTime();
			break;
		}
	}
	// 各シーンの初期化
	if (!isLoad_) {
	
		isLoad_ = true;
		SetUseASyncLoadFlag(true);
		scene_->InitLoad();
		SetUseASyncLoadFlag(false);
	}
	if (GetASyncLoadNum() <= 0) {

		isLoad_ = false;
		loadCnt_ = 0;

		// 初期化
		Camera::GetInstance()->Init();
		scene_->Init();

		ResetDeltaTime();

		waitSceneId_ = SCENE_ID::NONE;
	}
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
			timer_ = 0;
		}
		break;

	case Fader::STATE::FADE_OUT:
		// 暗転中
		if (fader_->IsEnd()){

			// 完全に暗転してからシーン遷移
			DoChangeScene(waitSceneId_);

			if (!isLoad_) {

				// 暗転から明転へ
				fader_->SetFade(Fader::STATE::FADE_IN);
			}
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
	ChangeLightTypeDir({ 0.00f, -1.00f, 0.00f });

	// フォグ設定
	SetFogEnable(true);
	// フォグの色
	SetFogColor(200, 200, 200);
	// フォグを発生させる奥行きの最小、最大距離
	SetFogStartEnd(12000.0f, 14000.0f);
}

std::vector<int> SceneManager::LoadTime(void)
{
	// ファイルの読込
	std::ifstream ifs = std::ifstream(Application::PATH_CSV + "Time.csv");
	std::vector<int> fileTime;

	if (!ifs) {

		// エラーが発生
		fileTime.push_back(-1);
		return fileTime;
	}
	// ファイルを１行ずつ読み込む
	std::string line;
	// 1行の文字情報
	std::vector<std::string> strSplit;

	getline(ifs, line);

	// １行をカンマ区切りで分割
	strSplit = Utility::Split(line, ',');

	for (int i = 0; i < strSplit.size(); i++) {

		fileTime.insert(fileTime.begin() + i, atoi(strSplit.at(i).c_str()));
	}
	ifs.close();

	//昇順にソート
	sort(fileTime.begin(), fileTime.end());

	//サイズオーバーしてるなら最後の数値を消す
	//(例外処理)
	while (fileTime.size() >= 11) {
		fileTime.pop_back();
	}

	return fileTime;
}

void SceneManager::SaveTime(void)
{
	//既存のファイルを開く
	std::vector<int> fileTime = LoadTime();
	std::vector<int> newTime;

	//新しいタイムを入れてソートする
	newTime.push_back(GetTime());
	sort(newTime.begin(), newTime.end());

	//サイズオーバーしてたら最後を消す
	if (newTime.size() == 11) {

		newTime.pop_back();
	}
	// ファイルの書き込み
	std::ofstream ofs = std::ofstream(Application::PATH_CSV + "Time.csv");

	//カンマ区切りで書き込み
	for (int t : newTime) {

		ofs << t;
		ofs << ",";
	}
	
	ofs.close();
}
