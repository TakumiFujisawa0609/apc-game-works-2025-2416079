#include <EffekseerForDXLib.h>
#include "Manager/EffectResManager.h"
#include "Manager/SceneManager.h"
#include "Manager/Input/InputManager.h"
#include "Manager/FpsControll.h"
#include "Manager/Camera.h"
#include "Manager/Audio/AudioManager.h"
#include "Application.h"

Application* Application::instance_ = nullptr;

const std::string Application::PATH_DATA = "Data/";
const std::string Application::PATH_IMAGE = PATH_DATA + "Image/";
const std::string Application::PATH_MODEL = PATH_DATA + "Model/";
const std::string Application::PATH_ANIMATION = PATH_DATA + "Animation/";
const std::string Application::PATH_EFFECT = PATH_DATA + "Effect/";
const std::string Application::PATH_SOUND = PATH_DATA + "Sounds/";
const std::string Application::PATH_FONT = PATH_DATA + "Font/";
const std::string Application::PATH_CSV = PATH_DATA + "Csv/";
const std::string Application::PATH_VIDEO = PATH_DATA + "Video/";
const std::string Application::PATH_SHADER = PATH_DATA + "Shader/";

void Application::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new Application();
	}
	instance_->Init();
}

Application& Application::GetInstance(void)
{
	return *instance_;
}

void Application::Init(void)
{

	// アプリケーションの初期設定
	SetWindowText("Colo Hunt");
	SetWindowIconID(317);

	// ウィンドウサイズ
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 32);
	int sizeX, sizeY;
	GetDefaultState(&sizeX, &sizeY, NULL);		// デフォルトの画面サイズを取得
	SetWindowSizeExtendRate(1.0);
	SetWindowSize(sizeX, sizeY);	// ウィンドウのサイズを設定
	ChangeWindowMode(true);

	// DxLibの初期化
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	isInitFail_ = false;
	if (DxLib_Init() == -1)
	{
		isInitFail_ = true;
		return;
	}
	SetMouseDispFlag(false);
	AddFontResource((Application::PATH_FONT + "toroman.ttf").c_str());
	AddFontResource((Application::PATH_FONT + "MonserhunterfontsXtype-Regular.ttf").c_str());

	InitEffekseer();

	// 乱数のシード値を設定する
	DATEDATA date;

	// 現在時刻を取得する
	GetDateTime(&date);

	// 乱数の初期値を設定する
	// 設定する数値によって、ランダムの出方が変わる
	SRand(date.Year + date.Mon + date.Day + date.Hour + date.Min + date.Sec);

	// Zバッファを有効にする
	SetUseZBuffer3D(true);

	// Zバッファへの書き込みを有効にする
	SetWriteZBuffer3D(true);

	// バックカリングを有効にする
	SetUseBackCulling(true);

	// ライトの設定
	SetUseLighting(true);

	// ライトの設定
	ChangeLightTypeDir({ 0.00f, -1.00f, 0.00f });

	// フォグ設定
	SetFogEnable(true);
	// フォグの色
	SetFogColor(200, 200, 200);
	// フォグを発生させる奥行きの最小、最大距離
	SetFogStartEnd(12000.0f, 14000.0f);

	// 入力制御初期化
	SetUseDirectInputFlag(true);
	InputManager::CreateInstance();
	InputManager::GetInstance().Init();

	// エフェクト管理初期化
	EffectResManager::CreateInstance();

	//オーディオ管理初期化
	AudioManager::CreateInstance();
	AudioManager::GetInstance()->Init();

	//カメラの初期化
	Camera::CreateInstance();

	// シーン管理初期化
	SceneManager::CreateInstance();
	SceneManager::GetInstance().Init();

	fpsControll_ = new FpsControll(FPS);

	isFinish_ = false;
}

void Application::Run(void)
{
	// ゲームループ
	while (ProcessMessage() == 0 && !isFinish_)
	{
		InputManager::GetInstance().Update();
		SceneManager::GetInstance().Update();

		if (!fpsControll_->SkipDrawScene()){
			
			SceneManager::GetInstance().Draw();
			fpsControll_->Wait();
		}
		ScreenFlip();
	}

}

void Application::Destroy(void)
{
	AudioManager::GetInstance()->DeleteAll();

	// シーン管理解放
	SceneManager::GetInstance().Release();

	// エフェクト管理解放
	EffectResManager::GetInstance().Destroy();

	// 入力制御解放
	InputManager::GetInstance().Release();

	//カメラの開放
	Camera::DeleteInstance();

	Effkseer_End();

	// DxLib終了
	if (DxLib_End() == -1)
	{
		isReleaseFail_ = true;
	}

	// インスタンスのメモリ解放
	delete instance_;

}

bool Application::IsInitFail(void) const
{
	return isInitFail_;
}

bool Application::IsReleaseFail(void) const
{
	return isReleaseFail_;
}

Application::Application(void):isFinish_(false), isInitFail_(false), isReleaseFail_(false), fpsControll_(nullptr)
{
}

void Application::InitEffekseer(void)
{
	if (Effekseer_Init(8000) == -1) {

		DxLib_End();
	}
	SetChangeScreenModeGraphicsSystemResetFlag(false);
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
}
