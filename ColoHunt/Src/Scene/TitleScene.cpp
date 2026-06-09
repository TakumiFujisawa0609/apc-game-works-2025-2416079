#include <string>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Audio/AudioManager.h"
#include "../Manager/Audio/SoundTable.h"
#include "../Manager/Input/InputManager.h"
#include "../Common/Fader.h"
#include "TitleScene.h"


TitleScene::TitleScene(void) : SceneBase(), cnt_(0), show_(true)
{
	imgTitle_ = -1;
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::InitLoad(void)
{
	imgTitle_ = LoadGraph((Application::PATH_IMAGE + "Title.png").c_str());
	imgBg_ = LoadGraph((Application::PATH_IMAGE + "Title.jpg").c_str());
	video_ = LoadGraph((Application::PATH_VIDEO + "PV.mp4").c_str());
	AudioManager::GetInstance()->LoadSceneSound(LoadScene::TITLE);
}

void TitleScene::Init(void)
{
	state_ = STATE::LOGO;
	select_ = SELECT::PLAY;
	fader_ = new Fader();
	times_ = LoadTime();

	font_ = CreateFontToHandle("アンニャントロマン", 45, 1, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	creatorFont_ = CreateFontToHandle("MS 明朝", 15, -1, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	selectFont_ = CreateFontToHandle("MSPゴシック", 75, 8, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	timeFont_ = CreateFontToHandle("Monserhunterfonts Xtype", 75, 3, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
}

void TitleScene::Update(void)
{
	switch (state_)
	{
	case TitleScene::STATE::LOGO:
	
		UpdateLogo();
		break;

	case TitleScene::STATE::SELECT:
		
		UpdateSelect();
		break;

	case TitleScene::STATE::RANKING:
		
		UpdateRanking();
		break;

	case TitleScene::STATE::PV:
		
		UpdatePv();
		break;
	}
}

void TitleScene::Draw(void)
{
	switch (state_)
	{
	case TitleScene::STATE::LOGO:
		
		DrawLogo();
		break;
	
	case TitleScene::STATE::SELECT:
	
		DrawSelect();
		break;

	case TitleScene::STATE::RANKING:
		
		DrawRanking();
		break;

	case TitleScene::STATE::PV:
		
		DrawPv();
		break;
	}
	fader_->Draw();
}

void TitleScene::Release(void)
{
	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::TITLE);
	
	DeleteGraph(imgTitle_);
	DeleteFontToHandle(font_);
	DeleteFontToHandle(creatorFont_);
	DeleteFontToHandle(timeFont_);
	
	delete fader_;
	
	//DeleteGraph(imgBg_);
}

void TitleScene::ChangeState(STATE sta) 
{
	state_ = sta;

	switch (state_)
	{
	case TitleScene::STATE::LOGO:
	
		ChangeLogo();
		break;

	case TitleScene::STATE::SELECT:
		
		ChangeSelect();
		break;

	case TitleScene::STATE::RANKING:
		
		ChangeRanking();
		break;

	case TitleScene::STATE::PV:
		
		ChangePv();
		break;
	}
}

void TitleScene::UpdateLogo(void)
{
	fader_->Update();

	if (fader_->IsEnd()) {
		if (fader_->GetState() == Fader::STATE::FADE_OUT) {

			fader_->SetFade(Fader::STATE::FADE_IN);
		}
		else if (fader_->GetState() == Fader::STATE::FADE_IN) {

			fader_->SetFade(Fader::STATE::NONE);
		}
	}
	if (fader_->GetState() == Fader::STATE::NONE) {
		
		cnt_++;

		if (cnt_ % CHANGE_CNT == 0) {

			show_ = !show_;
		}
		if (cnt_ >= CHANGE_CNT * (CHANGE_CNT / 3) - 1) {

			cnt_ = 0;
			ChangeState(STATE::PV);
			return;
		}

		// シーン遷移
		if (InputManager::GetInstance().GetPriorityAnyoneTrgDown()) {

			cnt_ = 0;
			AudioManager::GetInstance()->PlayBGM(SoundID::BGM_TITLE);
			AudioManager::GetInstance()->PlaySE(SoundID::SE_DESIDE);
			state_ = STATE::SELECT;
		}
	}
}

void TitleScene::UpdateSelect(void)
{
	// キャンセルしたら戻る
	if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::CANCEL).keyTrgDown) {
	
		AudioManager::GetInstance()->PlaySE(SoundID::SE_CANCEL);
		ChangeState(STATE::LOGO);
		return;
	}
	// 今の選択状態をintに変換
	int nowSelect = static_cast<int>(select_);
	
	if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::UP).keyTrgDown) {
		
		AudioManager::GetInstance()->PlaySE(SoundID::SE_CURSOR);
		
		// 上にずらす
		nowSelect--;

		// 一番上の場合そのまま
		if (nowSelect < static_cast<int>(SELECT::PLAY)) {

			nowSelect = static_cast<int>(SELECT::PLAY);
		}
	}
	else if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::DOWN).keyTrgDown) {
		
		AudioManager::GetInstance()->PlaySE(SoundID::SE_CURSOR);

		// 下にずらす
		nowSelect--;

		// 一番下の場合そのまま
		if (nowSelect > static_cast<int>(SELECT::END)) {

			nowSelect = static_cast<int>(SELECT::END);
		}
	}

	// intをenumへ
	select_ = static_cast<SELECT>(nowSelect);

	if (InputManager::GetInstance().GetPriorityKey(InputManager::COMMAND::DECIDE).keyTrgDown) {
		
		AudioManager::GetInstance()->PlaySE(SoundID::SE_DESIDE);
		
		switch (select_)
		{
		case TitleScene::SELECT::PLAY:

			SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
			break;

		case TitleScene::SELECT::RANKING:

			ChangeState(STATE::RANKING);
			break;

		case TitleScene::SELECT::END:

			Application::GetInstance().FinishGame();
			break;
		}
	}
}

void TitleScene::UpdateRanking(void)
{
	// シーン遷移
	if (InputManager::GetInstance().GetPriorityAnyoneTrgDown()) {

		AudioManager::GetInstance()->PlaySE(SoundID::SE_CANCEL);
		state_ = STATE::SELECT;
	}
}

void TitleScene::UpdatePv(void)
{
	fader_->Update();

	if (fader_->IsEnd()) {
		if (fader_->GetState() == Fader::STATE::FADE_OUT) {

			SeekMovieToGraphToFrame(video_, 0);
			SetMovieVolumeToGraph(video_, 1);
			PlayMovieToGraph(video_);
			fader_->SetFade(Fader::STATE::FADE_IN);
		}
		else if (fader_->GetState() == Fader::STATE::FADE_IN) {
			
			fader_->SetFade(Fader::STATE::NONE);
		}
	}
	if (fader_->GetState() == Fader::STATE::NONE) {

		// シーン遷移
		if (InputManager::GetInstance().GetPriorityAnyoneTrgDown() || GetMovieStateToGraph(video_) == 0) {

			fader_->SetFade(Fader::STATE::FADE_OUT);
			SceneManager::GetInstance().SetScreenImage();
			state_ = STATE::LOGO;
			PauseMovieToGraph(video_);
		}
	}
}

void TitleScene::ChangeLogo(void)
{
	AudioManager::GetInstance()->StopBGM();
}

void TitleScene::ChangeSelect(void)
{
}

void TitleScene::ChangeRanking(void)
{
}

void TitleScene::ChangePv(void)
{
	fader_->SetFade(Fader::STATE::FADE_OUT);
}

void TitleScene::DrawLogo(void)
{
	if (fader_->GetState() != Fader::STATE::FADE_OUT) {
		
		DrawGraph(0, 0, imgBg_, true);
		int widthA = GetDrawFormatStringWidthToHandle(font_, "Press Any Botton");

		if (show_) {

			DrawFormatStringToHandle((Application::SCREEN_SIZE_X - widthA) / 2, Application::SCREEN_SIZE_Y - 175, 0xdd2d2d, font_, "Press Any Botton");
		}

		int widthB = GetDrawFormatStringWidthToHandle(creatorFont_, "HIRO 2025,2026 CREATED");

		DrawFormatStringToHandle((Application::SCREEN_SIZE_X - widthB) / 2, Application::SCREEN_SIZE_Y - 100, 0xffffff, creatorFont_, "HIRO 2025,2026 CREATED");

		//DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, 1.0f, 0.0, imgBg_, true);
		DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 - 100, 0.75f, 0.0, imgTitle_, true);
	}
	else {

		DrawGraph(0, 0, SceneManager::GetInstance().GetScreenImage(), false);
	}
}

void TitleScene::DrawSelect(void)
{
	//背景
	DrawGraph(0, 0, imgBg_, true);

	//黒背景
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 125);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	
	//描画
	DrawRotaGraph(1400, 300, 0.6, 0.0, imgTitle_, true);

	int dx = Application::SCREEN_SIZE_X / 50;
	int dy = Application::SCREEN_SIZE_Y / 20;

	DrawBox(dx, dy, dx * 20, dy * 19, 0x000000, true);
	DrawLineBox(dx, dy, dx * 20, dy * 19, 0xcaaa00, 5);

	dx += Application::SCREEN_SIZE_X * 3 / 16;
	dy += Application::SCREEN_SIZE_Y / 8;

	int width = GetDrawFormatStringWidthToHandle(selectFont_, "ハント開始") / 2;

	DrawFormatStringToHandle(dx - width, dy, 0xffffff, selectFont_, "ハント開始");

	if (select_ == SELECT::PLAY) {

		DrawTriangle(dx - 275, dy + 65, dx - 225, dy + 35, dx - 275, dy + 5, 0xffffff, true);
	}

	dy += Application::SCREEN_SIZE_Y / 4;
	width = GetDrawFormatStringWidthToHandle(selectFont_, "ランキング") / 2;

	DrawFormatStringToHandle(dx - width, dy, 0xffffff, selectFont_, "ランキング");

	if (select_ == SELECT::RANKING) {

		DrawTriangle(dx - 275, dy + 65, dx - 225, dy + 35, dx - 275, dy + 5, 0xffffff, true);
	}

	dy += Application::SCREEN_SIZE_Y / 4;
	width = GetDrawFormatStringWidthToHandle(selectFont_, "終　了") / 2;

	DrawFormatStringToHandle(dx - width, dy, 0xffffff, selectFont_, "終　了");

	if (select_ == SELECT::END) {

		DrawTriangle(dx - 275, dy + 65, dx - 225, dy + 35, dx - 275, dy + 5, 0xffffff, true);
	}

	dx = Application::SCREEN_SIZE_X / 2;
	dy = Application::SCREEN_SIZE_Y * 2 / 3;

	DrawBox(dx, dy, dx * 2 - 50, dy  * 11 / 8, 0x000000, true);
	DrawLineBox(dx, dy, dx * 2 - 50, dy * 11 / 8, 0xcaaa00, 5);

	dx += Application::SCREEN_SIZE_X * 61 / 256;
	dy += 100;

	switch (select_)
	{
	case TitleScene::SELECT::PLAY:
		
		width = GetDrawFormatStringWidthToHandle(selectFont_, "ハントを開始します") / 2;
		DrawFormatStringToHandle(dx - width, dy, 0xffffff, selectFont_, "ハントを開始します");
		break;

	case TitleScene::SELECT::RANKING:
		
		width = GetDrawFormatStringWidthToHandle(selectFont_, "ランキングを表示します") / 2;
		DrawFormatStringToHandle(dx - width, dy, 0xffffff, selectFont_, "ランキングを表示します");
		break;

	case TitleScene::SELECT::END:
		
		width = GetDrawFormatStringWidthToHandle(selectFont_, "ゲームを終了します") / 2;
		DrawFormatStringToHandle(dx - width, dy, 0xffffff, selectFont_, "ゲームを終了します");
		break;
	}

	//文字サイズを得る
	int fontSize = GetFontSize();
	//サイズをあげる
	SetFontSize(20);

	//表示
	DrawBox(0, Application::SCREEN_SIZE_Y - 50, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_X, 0x000000, true);
	DrawString(100, Application::SCREEN_SIZE_Y - 40, "↑↓　選択", 0xffffff);

	//コントローラータイプに応じて表示を変える
	if (InputManager::GetInstance().GetMostPriorityType() == InputBase::JOYPAD_TYPE::NON) {

		DrawString(250, Application::SCREEN_SIZE_Y - 40, "Lｸﾘｯｸ　決定", 0xffffff);
		DrawString(400, Application::SCREEN_SIZE_Y - 40, "Rｸﾘｯｸ　戻る", 0xffffff);
	}
	else if (InputManager::GetInstance().GetMostPriorityType() <= InputBase::JOYPAD_TYPE::XBOX_ONE) {

		DrawString(250, Application::SCREEN_SIZE_Y - 40, "Ａ　決定", 0xffffff);
		DrawString(400, Application::SCREEN_SIZE_Y - 40, "Ｂ　戻る", 0xffffff);
	}
	else if (InputManager::GetInstance().GetMostPriorityType() <= InputBase::JOYPAD_TYPE::DUAL_SENSE) {

		DrawString(250, Application::SCREEN_SIZE_Y - 40, "×　決定", 0xffffff);
		DrawString(400, Application::SCREEN_SIZE_Y - 40, "○　戻る", 0xffffff);
	}
	else {

		DrawString(250, Application::SCREEN_SIZE_Y - 40, "Ｂ　決定", 0xffffff);
		DrawString(400, Application::SCREEN_SIZE_Y - 40, "Ａ　戻る", 0xffffff);
	}

	//サイズを戻す
	SetFontSize(fontSize);
}

void TitleScene::DrawRanking(void)
{
	DrawGraph(0, 0, imgBg_, true);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 125);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	DrawRotaGraph(1400, 300, 0.6, 0.0, imgTitle_, true);

	int dx = Application::SCREEN_SIZE_X / 50;
	int dy = Application::SCREEN_SIZE_Y / 20;

	DrawBox(dx, dy, dx * 49, dy * 19, 0x000000, true);
	DrawLineBox(dx, dy, dx * 49, dy * 19, 0xcaaa00, 5);

	dx += Application::SCREEN_SIZE_X / 7; 
	dy *= 2;

	DrawFormatStringToHandle(dx, dy, 0xffffff, selectFont_, "トップ10のタイムを表示しています");

	for (int i = 0; i < 10; i++) {

		int time = 0;
		if (i < times_.size()) {
		
			time = times_.at(i);
		}
		if (i < 5) {

			dy = Application::SCREEN_SIZE_Y / 10 + (Application::SCREEN_SIZE_Y * 16 / 112 * (i + 1));
		}
		else {

			dx = Application::SCREEN_SIZE_X / 2 + Application::SCREEN_SIZE_X / 11;
			dy = Application::SCREEN_SIZE_Y / 10 + (Application::SCREEN_SIZE_Y * 16 / 112 * (i - 4));
		}
		if (time != 0) {

			DrawFormatStringToHandle(dx, dy + 15, 0xffffff, font_, "%d.", i + 1);
			DrawFormatStringToHandle(dx + 70, dy, 0xffffff, timeFont_, "%d:%02d", time / 60, time % 60);
		}
		else {

			DrawFormatStringToHandle(dx, dy + 15, 0xffffff, font_, "%d.", i + 1);
			DrawFormatStringToHandle(dx + 70, dy, 0xffffff, timeFont_, "--:--");
		}
	}

	//文字サイズを得る
	int fontSize = GetFontSize();
	//サイズをあげる
	SetFontSize(20);

	//表示
	DrawBox(0, Application::SCREEN_SIZE_Y - 50, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_X, 0x000000, true);
	DrawString(100, Application::SCREEN_SIZE_Y - 40, "ANY BUTTON　戻る", 0xffffff);
	
	//サイズを戻す
	SetFontSize(fontSize);
}

void TitleScene::DrawPv(void)
{
	if (fader_->GetState() != Fader::STATE::FADE_OUT) {
	
		DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, video_, FALSE);
	}
	else {
		
		DrawGraph(0, 0, imgBg_, false);
		int widthA = GetDrawFormatStringWidthToHandle(font_, "Press Any Button");

		if (show_) {

			DrawFormatStringToHandle((Application::SCREEN_SIZE_X - widthA) / 2, Application::SCREEN_SIZE_Y - 175, 0xdd2d2d, font_, "Press Any Button");
		}

		int widthB = GetDrawFormatStringWidthToHandle(creatorFont_, "HIRO 2025,2026 CREATED");

		DrawFormatStringToHandle((Application::SCREEN_SIZE_X - widthB) / 2, Application::SCREEN_SIZE_Y - 100, 0xffffff, creatorFont_, "HIRO 2025,2026 CREATED");

		DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 - 100, 0.75f, 0.0, imgTitle_, true);
	}
}
