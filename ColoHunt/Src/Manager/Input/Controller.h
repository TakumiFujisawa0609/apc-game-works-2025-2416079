#pragma once
#include <Dxlib.h>
#include "InputBase.h"


class Controller : public InputBase
{
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

	Controller(int no);
	~Controller(void);

	// 初期化
	void Init(void) override;
	// 更新
	void Update(void) override;
	// リソースの破棄
	void Release(void) override;

	// アナログキーの最大値
	static constexpr float AKEY_VAL_MAX = 1000.0f;
	// アナログキーの入力受付しきい値(0.0～1.0)
	static constexpr float THRESHOLD = 0.05f;

	// Lアナログキーの入力値から方向を取得
	VECTOR GetDirectionXZAKeyL(void);
	// Rアナログキーの入力値から方向を取得
	VECTOR GetDirectionXZAKeyR(void);

	// コントローラータイプ
	JOYPAD_TYPE GetJPadType(void) { return type_; }

private:

	// 判定感度
	static constexpr int INPUT_SENSI = 128;

	// パッドの番号
	const int PAD_NUMBER;

	// コントローラータイプ
	JOYPAD_TYPE type_;

	// コントローラ情報
	DINPUT_JOYSTATE joyDInState_;
	// コントローラ情報(XBOX)
	XINPUT_STATE joyXInState_;
	
	// アナログキーの入力情報
	int analogKeyLX;
	int analogKeyLY;
	int analogKeyRX;
	int analogKeyRY;

	// コントローラの入力情報を取得する
	DINPUT_JOYSTATE GetJPadDInputState(void);

	// コントローラ(XBOX)の入力情報を取得する
	XINPUT_STATE GetJPadXInputState(void);

	// コントローラの入力情報を更新する
	void SetJPadInState(void);

	// コントローラの入力情報を取得する
	bool GetJPadInputState(int idx);

	// アナログキーの入力値から方向を取得
	VECTOR GetDirectionXZAKey(int aKeyX, int aKeyY);
};