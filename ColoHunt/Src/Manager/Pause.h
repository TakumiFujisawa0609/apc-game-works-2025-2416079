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

	void Init(void);
	void Update();
	void Draw();
	void Release();

	bool IsPause() const { return isPause_; }

private:

	bool isPause_;
	PAUSE_STATE state_;

	VECTOR pos_;
};