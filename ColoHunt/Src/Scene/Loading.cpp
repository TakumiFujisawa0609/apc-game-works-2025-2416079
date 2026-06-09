#include "Loading.h"
#include <DxLib.h>
#include"../Application.h"
#include"../Utility/Utility.h"

Loading* Loading::instance_ = nullptr;

// コンストラクタ
Loading::Loading() : handle_(), posX_(0), posY_(0), isLoading_(false), loadTimer_(0)
{}

// デストラクタ
Loading::~Loading()
{}

// 初期化
void Loading::Init(void)
{
	loadTimer_ = 0;
	isLoading_ = false;
	posX_ = Application::SCREEN_SIZE_X_HALF;
	posY_ = Application::SCREEN_SIZE_Y_HALF;
}

// 読み込み
void Loading::InitLoad(void)
{
	handle_ = LoadGraph((Application::PATH_IMAGE + "Load.png").c_str());
}

// 更新
void Loading::Update(void)
{
	loadTimer_++;

	// 読込中のものがなくなったら or 最低ロード時間経過
	if (GetASyncLoadNum() == 0 && loadTimer_ >= MIN_LOAD_TIME)
	{
		// ロード終了
		Init();
	}
}

// 描画
void Loading::Draw(void)
{
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	DrawRotaGraph(posX_, posY_, IMAGE_RATE, 0.0, handle_, false);
}

// 解放
void Loading::Release(void)
{
	DeleteGraph(handle_);
}

// 非同期読み込みに切り替える
void Loading::StartAsyncLoad(void)
{
	isLoading_ = true;
	// 非同期読み込み開始
	SetUseASyncLoadFlag(true);
}

// 同期読み込みに切り替える
void Loading::EndAsyncLoad(void)
{
	// 非同期読み込み終了
	SetUseASyncLoadFlag(false);
}
