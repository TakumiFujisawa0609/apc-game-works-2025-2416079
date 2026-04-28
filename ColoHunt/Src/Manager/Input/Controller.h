#pragma once
#include <Dxlib.h>

class Controller {

public:

	// ゲームコントローラータイプ
	// DxLib定数、DX_OTHER等に対応
	enum class JOYPAD_TYPE
	{
		OTHER = 0,

		XBOX_360,
		XBOX_ONE,
		DUAL_SHOCK_3,
		DUAL_SHOCK_4,
		DUAL_SENSE,
		SWITCH_JOY_CON_L,
		SWITCH_JOY_CON_R,
		SWITCH_PRO_CTRL,

		MAX
	};

	// ゲームコントローラーボタン
	enum class JOYPAD_BTN
	{
		LEFT = 0,

		RIGHT,
		TOP,
		DOWN,
		LEFT_DPAD,
		RIGHT_DPAD,
		TOP_DPAD,
		DOWN_DPAD,
		R,
		L,
		R2_TRIGGER,
		L2_TRIGGER,
		START,
		SELECT,
		R3_PUSH,
		L3_PUSH,

		MAX
	};

	// ゲームコントローラーの入力情報
	struct JOYPAD_IN_STATE
	{
		unsigned char ButtonsOld[static_cast<int>(JOYPAD_BTN::MAX)];
		unsigned char ButtonsNew[static_cast<int>(JOYPAD_BTN::MAX)];
		bool IsOld[static_cast<int>(JOYPAD_BTN::MAX)];
		bool IsNew[static_cast<int>(JOYPAD_BTN::MAX)];
		bool IsTrgDown[static_cast<int>(JOYPAD_BTN::MAX)];
		bool IsTrgUp[static_cast<int>(JOYPAD_BTN::MAX)];
		int AKeyLX;
		int AKeyLY;
		int AKeyRX;
		int AKeyRY;
		bool AnyoneBotton;
		bool IsAnyoneDown;
		bool Anyone;
	};

	Controller(void);
	~Controller(void);

	// 初期化
	virtual void Init(void);
	// 更新
	virtual void Update(int i);
	// リソースの破棄
	virtual void Destroy(void);

	// アナログキーの最大値
	static constexpr float AKEY_VAL_MAX = 1000.0f;
	// アナログキーの入力受付しきい値(0.0～1.0)
	static constexpr float THRESHOLD = 0.05f;

	// アナログキーの入力値から方向を取得
	VECTOR GetDirectionXZAKey(int aKeyX, int aKeyY);

	// コントローラの入力情報を取得する
	JOYPAD_IN_STATE GetJPadState(int no) const { return padInfos_[static_cast<int>(no)]; }

	JOYPAD_TYPE GetJPadType(int no) { return GetJPadDType(no); }

private:

	// コントローラ情報
	DINPUT_JOYSTATE joyDInState_;
	// コントローラ情報(XBOX)
	XINPUT_STATE joyXInState_;

	// パッド情報
	JOYPAD_IN_STATE padInfos_[5];

	// 接続されたコントローラの種別を取得する
	JOYPAD_TYPE GetJPadDType(int no);

	// コントローラの入力情報を取得する
	DINPUT_JOYSTATE GetJPadDInputState(int no);

	// コントローラ(XBOX)の入力情報を取得する
	XINPUT_STATE GetJPadXInputState(int no);

	// コントローラの入力情報を更新する
	void SetJPadInState(int jpNo);

	// コントローラの入力情報を取得する
	JOYPAD_IN_STATE GetJPadInputState(int no);
};