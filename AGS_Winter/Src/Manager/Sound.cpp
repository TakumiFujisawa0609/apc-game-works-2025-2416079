#include <DxLib.h>
#include "Sound.h"
#include "../Application.h"


Sound* Sound::sound_ = nullptr;

void Sound::CreateInstance(void)
{
	sound_ = new Sound();

	sound_->SystemInit();
}

Sound& Sound::GetInstance(void)
{
	if (sound_ == nullptr) {

		sound_->CreateInstance();
	}
	return *sound_;
}

bool Sound::SystemInit(void)
{
	//常時シーン

	soundMem.mem = LoadSoundMem("Sounds/hitKey.mp3");
	soundMem.type = DX_PLAYTYPE_BACK;
	soundMem.volume = 125;
	se_[SE_TYPE::E_SE_HITKEY] = soundMem;
	
	soundMem.mem = LoadSoundMem("Sounds/cursor.mp3");
	soundMem.type = DX_PLAYTYPE_BACK;
	soundMem.volume = 125;
	se_[SE_TYPE::E_SE_CURSOR] = soundMem;

	soundMem.mem = LoadSoundMem("Sounds/pause.mp3");
	soundMem.type = DX_PLAYTYPE_BACK;
	soundMem.volume = 125;
	se_[SE_TYPE::E_SE_PAUSE] = soundMem;

	soundMem.mem = LoadSoundMem("Sounds/fade.mp3");
	soundMem.type = DX_PLAYTYPE_BACK;
	soundMem.volume = 125;
	se_[SE_TYPE::E_SE_FADE] = soundMem;
	
	//タイトルシーン

	soundMem.mem = LoadSoundMem("Sounds/title.mp3");
	soundMem.type = DX_PLAYTYPE_LOOP;
	soundMem.volume = 150;
	bgm_[BGM_TYPE::E_BGM_TITLE] = soundMem;

	//ゲームシーン

	soundMem.mem = LoadSoundMem((Application::PATH_SOUND + "Slash_1.mp3").c_str());
	soundMem.type = DX_PLAYTYPE_BACK;
	soundMem.volume = 125;
	se_[SE_TYPE::E_SE_SLASH_1] = soundMem;
	
	soundMem.mem = LoadSoundMem("Sounds/ready.mp3");
	soundMem.type = DX_PLAYTYPE_BACK;
	soundMem.volume = 125;
	se_[SE_TYPE::E_SE_READY] = soundMem;

	soundMem.mem = LoadSoundMem("Sounds/go.mp3");
	soundMem.type = DX_PLAYTYPE_BACK;
	soundMem.volume = 125;
	se_[SE_TYPE::E_SE_GO] = soundMem;

	soundMem.mem = LoadSoundMem("Sounds/jump1.mp3");
	soundMem.type = DX_PLAYTYPE_BACK;
	soundMem.volume = 90;
	se_[SE_TYPE::E_SE_JUMP] = soundMem;

	soundMem.mem = LoadSoundMem("Sounds/jump2.mp3");
	soundMem.type = DX_PLAYTYPE_BACK;
	soundMem.volume = 90;
	se_[SE_TYPE::E_SE_JUMP_SEC] = soundMem;

	soundMem.mem = LoadSoundMem("Sounds/damage.mp3");
	soundMem.type = DX_PLAYTYPE_BACK;
	soundMem.volume = 90;
	se_[SE_TYPE::E_SE_DAMAGED] = soundMem;

	soundMem.mem = LoadSoundMem("Sounds/run.mp3");
	soundMem.type = DX_PLAYTYPE_LOOP;
	soundMem.volume = 60;
	se_[SE_TYPE::E_SE_RUN] = soundMem;

	soundMem.mem = LoadSoundMem("Sounds/slide.mp3");
	soundMem.type = DX_PLAYTYPE_LOOP;
	soundMem.volume = 70;
	se_[SE_TYPE::E_SE_SLIDE] = soundMem;

	soundMem.mem = LoadSoundMem("Sounds/fall.mp3");
	soundMem.type = DX_PLAYTYPE_BACK;
	soundMem.volume = 125;
	se_[SE_TYPE::E_SE_FALL] = soundMem;

	soundMem.mem = LoadSoundMem("Sounds/scream.mp3");
	soundMem.type = DX_PLAYTYPE_BACK;
	soundMem.volume = 125;
	se_[SE_TYPE::E_SE_SCREAM] = soundMem;

	soundMem.mem = LoadSoundMem("Sounds/game.mp3");
	soundMem.type = DX_PLAYTYPE_BACK;
	soundMem.volume = 80;
	bgm_[BGM_TYPE::E_BGM_GAME] = soundMem;

	//敗北

	soundMem.mem = LoadSoundMem("Sounds/lose.mp3");
	soundMem.type = DX_PLAYTYPE_BACK;
	soundMem.volume = 80;
	bgm_[BGM_TYPE::E_BGM_GAMEOVER] = soundMem;

	//勝利

	soundMem.mem = LoadSoundMem("Sounds/clear.mp3");
	soundMem.type = DX_PLAYTYPE_BACK;
	soundMem.volume = 100;
	bgm_[BGM_TYPE::E_BGM_GAMECLEAR] = soundMem;

	return true;
}

void Sound::Play(SE_TYPE type)
{
	ChangeVolumeSoundMem(se_[type].volume, se_[type].mem);
	PlaySoundMem(se_[type].mem, se_[type].type, true);
}

void Sound::Play(BGM_TYPE type)
{
	ChangeVolumeSoundMem(bgm_[type].volume, bgm_[type].mem);
	PlaySoundMem(bgm_[type].mem, bgm_[type].type, true);
}

void Sound::PlayMiddle(SE_TYPE type)
{
	ChangeVolumeSoundMem(se_[type].volume, se_[type].mem);
	PlaySoundMem(se_[type].mem, se_[type].type, false);
}

void Sound::PlayMiddle(BGM_TYPE type)
{
	ChangeVolumeSoundMem(bgm_[type].volume, bgm_[type].mem);
	PlaySoundMem(bgm_[type].mem, bgm_[type].type, false);
}

void Sound::Update(void)
{
	for (int i = 0; i < static_cast<int>(SE_TYPE::E_SE_MAX); i++) {
		
		SE_TYPE type = static_cast<SE_TYPE>(i);

		if (IsStopSe(type)) {
			
			StopSoundMem(se_[type].mem);
		}
	}
	for (int i = 0; i < static_cast<int>(BGM_TYPE::E_BGM_MAX); i++) {
		
		BGM_TYPE type = static_cast<BGM_TYPE>(i);

		if (IsStopBgm(type)) {

			StopSoundMem(bgm_[type].mem);
		}
	}
}

bool Sound::IsStopSe(SE_TYPE type)
{
	if (se_[type].type != DX_PLAYTYPE_LOOP) {
		if (CheckSoundMem(se_[type].mem) == 0) {

			return true;
		}
	}
	return false;
}

bool Sound::IsStopBgm(BGM_TYPE type)
{
	if (bgm_[type].type != DX_PLAYTYPE_LOOP) {
		if (CheckSoundMem(bgm_[type].mem) == 0) {

			return true;
		}
	}
	return false;
}

void Sound::Stop(void)
{
	for (int i = 0; i < static_cast<int>(SE_TYPE::E_SE_MAX); i++) {

		SE_TYPE type = static_cast<SE_TYPE>(i);

		if (!IsStopSe(type)) {
		
			StopSoundMem(se_[type].mem);
		}
	}
	for (int i = 0; i < static_cast<int>(BGM_TYPE::E_BGM_MAX); i++) {

		BGM_TYPE type = static_cast<BGM_TYPE>(i);
		
		if (!IsStopBgm(type)) {
		
			StopSoundMem(bgm_[type].mem);
		}
	}
}

void Sound::StopSeAll(void)
{
	for (int i = 0; i < static_cast<int>(SE_TYPE::E_SE_MAX); i++) {

		SE_TYPE type = static_cast<SE_TYPE>(i);
		
		if (!IsStopSe(type)) {
		
			StopSoundMem(se_[type].mem);
		}
	}
}

void Sound::StopSe(SE_TYPE type)
{
	if (!IsStopSe(type)) {

		StopSoundMem(se_[type].mem);
	}
}

void Sound::StopBgm(BGM_TYPE type)
{
	if (!IsStopBgm(type)) {

		StopSoundMem(bgm_[type].mem);
	}
}

bool Sound::Release(void)
{
	for (int i = 0; i < static_cast<int>(SE_TYPE::E_SE_MAX); i++) {

		SE_TYPE type = static_cast<SE_TYPE>(i);
		
		if (!IsStopSe(type)) {
		
			StopSoundMem(se_[type].mem);
		}
	}
	for (int i = 0; i < static_cast<int>(BGM_TYPE::E_BGM_MAX); i++) {

		BGM_TYPE type = static_cast<BGM_TYPE>(i);
		
		if (IsStopBgm(type)) {
		
			StopSoundMem(bgm_[type].mem);
		}
	}
	InitSoundMem();
	return true;
}
