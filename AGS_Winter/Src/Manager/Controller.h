#pragma once
#include <Dxlib.h>

class Controller {

public:
	// ゲームコントローラーの認識番号
// DxLib定数、DX_INPUT_PAD1等に対応
	enum class JOYPAD_NO
	{
		KEY_PAD1,			// キー入力とパッド１入力
		PAD1,				// パッド１入力
		PAD2,				// パッド２入力
		PAD3,				// パッド３入力
		PAD4,				// パッド４入力
		INPUT_KEY = 4096	// キー入力
	};

	// ゲームコントローラータイプ
	// DxLib定数、DX_OTHER等に対応
	enum class JOYPAD_TYPE
	{
		OTHER = 0,
		XBOX_360,
		XBOX_ONE,
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
		LEFT_CROSS,
		RIGHT_CROSS,
		TOP_CROSS,
		DOWN_CROSS,
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
	};

	// インスタンスを明示的に生成
	static void CreateInstance(void);
	// インスタンスの取得
	static Controller& GetInstance(void);

	// 初期化
	void Init(void);
	// 更新
	void Update(void);
	// リソースの破棄
	void Destroy(void);

	// アナログキーの最大値
	static constexpr float AKEY_VAL_MAX = 1000.0f;
	// アナログキーの入力受付しきい値(0.0～1.0)
	static constexpr float THRESHOLD = 0.1f;

	// アナログキーの入力値から方向を取得
	VECTOR GetDirectionXZAKey(int aKeyX, int aKeyY);

	// コントローラの入力情報を取得する
	JOYPAD_IN_STATE GetJPadInputState(JOYPAD_NO no);

	JOYPAD_TYPE GetJPadType(JOYPAD_NO no) { return GetJPadDType(no); }

private:

	//インスタンス
	static Controller* inst_;

	// コントローラ情報
	DINPUT_JOYSTATE joyDInState_;
	// コントローラ情報(XBOX)
	XINPUT_STATE joyXInState_;

	// パッド情報
	JOYPAD_IN_STATE padInfos_[5];

	// 接続されたコントローラの種別を取得する
	JOYPAD_TYPE GetJPadDType(JOYPAD_NO no);

	// コントローラの入力情報を取得する
	DINPUT_JOYSTATE GetJPadDInputState(JOYPAD_NO no);

	// コントローラ(XBOX)の入力情報を取得する
	XINPUT_STATE GetJPadXInputState(JOYPAD_NO no);

	// コントローラの入力情報を更新する
	void SetJPadInState(JOYPAD_NO jpNo);

};