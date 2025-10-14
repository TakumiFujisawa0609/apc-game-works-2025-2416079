#pragma once
#include <map>


class Sound {

public:

	struct SoundMem {

		int mem = -1;
		int type = -1;
		int volume = -1;
	};

	enum class SE_TYPE {

		E_SE_HITKEY,
		E_SE_CURSOR,
		E_SE_PAUSE,
		E_SE_FADE,

		E_SE_READY,
		E_SE_GO,
		
		E_SE_SLASH_1,
		E_SE_JUMP,
		E_SE_JUMP_SEC,
		E_SE_DAMAGED,
		E_SE_SLIDE,
		E_SE_RUN,
		E_SE_FALL,
		E_SE_SCREAM,

		E_SE_MAX,
	};

	enum class BGM_TYPE {

		E_BGM_TITLE,
		E_BGM_GAME,
		E_BGM_GAMEOVER,
		E_BGM_GAMECLEAR,

		E_BGM_MAX,
	};

	// インスタンスを明示的に生成
	static void CreateInstance(void);
	// インスタンスの取得
	static Sound& GetInstance(void);

	bool SystemInit(void);							//初期化処理
	void Play(SE_TYPE type);
	void Play(BGM_TYPE type);
	void PlayMiddle(SE_TYPE type);
	void PlayMiddle(BGM_TYPE type);
	void Update(void);								//更新処理
	void Stop(void);
	void StopSeAll(void);
	void StopSe(SE_TYPE type);
	void StopBgm(BGM_TYPE type);
	bool Release(void);						//解放処理   

	bool IsStopSe(SE_TYPE type);
	bool IsStopBgm(BGM_TYPE type);

private:

	static Sound* sound_;

	SoundMem soundMem;

	SE_TYPE prevSeType_;
	BGM_TYPE prevBgmType_;

	std::map<SE_TYPE, SoundMem> se_;
	std::map<BGM_TYPE, SoundMem> bgm_;
};