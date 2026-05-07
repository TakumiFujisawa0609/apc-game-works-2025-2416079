#pragma once
#include <Dxlib.h>
#include "InputBase.h"


class Controller : public InputBase
{

public:

	Controller(int num);
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

	// アナログキーの入力値\を取得
	VECTOR GetDirectionXZAKeyL(void);
	VECTOR GetDirectionXZAKeyR(void);

	JOYPAD_TYPE GetJPadType(void) { return GetJPadDType(); }

	// コントローラータイプ
	JOYPAD_TYPE type_;

	// コントローラーの番号
	const int CONTROLLER_NUM;

	// コントローラ情報
	DINPUT_JOYSTATE joyDInState_;
	// コントローラ情報(XBOX)
	XINPUT_STATE joyXInState_;

	// 接続されたコントローラの種別を取得する
	JOYPAD_TYPE GetJPadDType(void);

	// コントローラの入力情報を取得する
	DINPUT_JOYSTATE GetJPadDInputState(void);

	// コントローラ(XBOX)の入力情報を取得する
	XINPUT_STATE GetJPadXInputState(void);

	// コントローラの入力情報を取得する
	bool GetJPadInputState(int key);

	// アナログキーの入力値から方向を取得
	VECTOR GetDirectionXZAKey(int aKeyX, int aKeyY);

	// 左スティック
	int analogKeyLX;
	int analogKeyLY;

	// 右スティック
	int analogKeyRX;
	int analogKeyRY;
};