#pragma once
#include "SceneManager.h"

class Pause {
public:

	enum class PAUSE_STATE {
		
		CONTINUE = 2,
		SETTING,
		RETURN_TITLE,
		FINISH,
	};

	Pause();
	~Pause();

	void Init(SceneManager::SCENE_ID id);
	void Update();
	void Draw();
	void Release();

	bool IsPause() const { return isPause_; }

private:

	SceneManager::SCENE_ID prevId_;
	bool isPause_;
	PAUSE_STATE state_;
	int image_;
};