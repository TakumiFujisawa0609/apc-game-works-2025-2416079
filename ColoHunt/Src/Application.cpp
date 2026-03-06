#include <EffekseerForDXLib.h>
#include "Manager/Input/KeyMouse.h"
#include "Manager/EffectResManager.h"
#include "Manager/SceneManager.h"
#include "Manager/Input/Controller.h"
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

	// 入力制御初期化
	SetUseDirectInputFlag(true);
	KeyMouse::CreateInstance();
	Controller::CreateInstance();

	// エフェクト管理初期化
	EffectResManager::CreateInstance();

	//オーディオ管理初期化
	AudioManager::CreateInstance();
	AudioManager::GetInstance()->Init();

	//カメラの初期化
	Camera::CreateInstance();

	// シーン管理初期化
	SceneManager::CreateInstance();

	fpsControll_ = new FpsControll(FPS);

	isFinish_ = false;
}

void Application::Run(void)
{

	KeyMouse& inputManager = KeyMouse::GetInstance();
	Controller& controller = Controller::GetInstance();
	SceneManager& sceneManager = SceneManager::GetInstance();

	// ゲームループ
	while (ProcessMessage() == 0 && !isFinish_)
	{

		inputManager.Update();
		controller.Update(Controller::JOYPAD_NO::PAD1);
		sceneManager.Update();

		if (!fpsControll_->SkipDrawScene()){
			
			sceneManager.Draw();
			fpsControll_->Wait();
		}
		ScreenFlip();
	}

}

void Application::Destroy(void)
{
	AudioManager::GetInstance()->DeleteAll();

	// シーン管理解放
	SceneManager::GetInstance().Destroy();

	// エフェクト管理解放
	EffectResManager::GetInstance().Destroy();

	// 入力制御解放
	Controller::GetInstance().Destroy();

	// 入力制御解放
	KeyMouse::GetInstance().Destroy();

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
