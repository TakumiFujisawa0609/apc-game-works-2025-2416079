#include <string>
#include "../Application.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "TitleScene.h"


TitleScene::TitleScene(void)
{
	imgTitle_ = -1;
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	imgTitle_ = LoadGraph((Application::PATH_IMAGE + "Title.png").c_str());
	imgBg_ = LoadGraph((Application::PATH_IMAGE + "Background.png").c_str());
}

void TitleScene::Update(void)
{
	// シーン遷移
	InputManager& ins = InputManager::GetInstance();

	if (ins.IsTrgDown(KEY_INPUT_SPACE)){

		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}
}

void TitleScene::Draw(void)
{
	SceneBase::Draw();

	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, 1.0f, 0.0, imgBg_, true);
	DrawRotaGraph( Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 - 100, 1.0f, 0.0, imgTitle_, true);

	//VECTOR pos;
	//pos = { 0.0f, 0.0f, 0.0f };
	//DrawSphere3D( pos, 60.0f, 10, 0xff0000, GetColor(255, 0, 0), true );

	/*SetUseLighting(false);
	pos = { 0.0f, 0.0f, 0.0f };
	DrawSphere3D(pos, 60.0f, 10, 0xff0000, 0xff0000, true);
	SetUseLighting(true);*/

	//VECTOR lightDir = GetLightDirection();
	//float pow = 0.05f;
	//if (CheckHitKey(KEY_INPUT_A)) { lightDir.x -= pow; }
	//if (CheckHitKey(KEY_INPUT_D)) { lightDir.x += pow; }
	//if (CheckHitKey(KEY_INPUT_W)) { lightDir.y -= pow; }
	//if (CheckHitKey(KEY_INPUT_S)) { lightDir.y += pow; }
	//SetLightDirection(lightDir);
	//// 真ん中の球体
	//pos = { 0.0f, 0.0f, 0.0f };
	//DrawSphere3D({100.0f, 100.0f, 100.0f}, 60.0f, 10, 0xff0000, 0xff0000, true);
	//// 光の反対方向
	//VECTOR revLightDir = VScale(lightDir, -1.0f);
	//// 光の位置
	//pos = VAdd(pos, VScale(revLightDir, 300.0f));
	//DrawSphere3D(pos, 30.0f, 10, 0xaaaa00, 0xaaaa00, true);
	//DrawLine3D(pos, VAdd(pos, VScale(lightDir, 150.0f)), 0xaaaa00);
	////光の方向：
	//DrawFormatString(20, 20, 0x000000,"% .2f, % .2f, % .2f", lightDir.x, lightDir.y, lightDir.z);

	//SetUseZBuffer3D(false);
	//SetWriteZBuffer3D(false);
	//pos = { 0.0f, 0.0f, 0.0f };
	//DrawSphere3D(pos, 60.0f, 10, 0xff0000, 0xff0000, true);
	//pos = { 0.0f, 0.0f, 100.0f };
	//DrawSphere3D(pos, 100.0f, 10, 0x00ff00, 0x00ff00, true);
	//SetWriteZBuffer3D(true);
	//SetUseZBuffer3D(true);

	//// バックカリングを無効にする
	//SetUseBackCulling(false);
	//// カメラ位置
	//pos = { 0.0f, 500.0f, -500.0f };
	//DrawSphere3D(pos, 300.0f, 100, 0xff0000, 0xff0000, true);
	//SetUseBackCulling(true);
	//// ライトの位置変更
	//VECTOR lightDir = GetLightDirection();
	//float pow = 0.05f;
	//if (CheckHitKey(KEY_INPUT_A)) { lightDir.x -= pow; }
	//if (CheckHitKey(KEY_INPUT_D)) { lightDir.x += pow; }
	//if (CheckHitKey(KEY_INPUT_W)) { lightDir.y -= pow; }
	//if (CheckHitKey(KEY_INPUT_S)) { lightDir.y += pow; }
	//SetLightDirection(lightDir);
	//
	//DrawFormatString(20, 20, 0xffffff,"光の方向：(% .2f, % .2f, % .2f)", lightDir.x, lightDir.y, lightDir.z);
}

void TitleScene::Release(void)
{
	DeleteGraph(imgTitle_);
	DeleteGraph(imgBg_);
}
