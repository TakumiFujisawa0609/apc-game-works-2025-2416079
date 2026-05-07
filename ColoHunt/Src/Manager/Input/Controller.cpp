#include <cmath>
#include "Controller.h"


Controller::Controller(int no) : PAD_NUMBER(no)
{
}

Controller::~Controller(void)
{
}

void Controller::Init(void)
{
	type_ = static_cast<Controller::JOYPAD_TYPE>(GetJoypadType(PAD_NUMBER));
}

void Controller::Update(void)
{
	// パッド情報
	SetJPadInState();
}

void Controller::Release(void)
{
}

VECTOR Controller::GetDirectionXZAKeyL(void)
{
	return GetDirectionXZAKey(analogKeyLX, analogKeyLY);
}

VECTOR Controller::GetDirectionXZAKeyR(void)
{
	return GetDirectionXZAKey(analogKeyRX, analogKeyRY);
}

bool Controller::GetJPadInputState(int idx)
{
	bool res = false;

	switch (type_){
	case Controller::JOYPAD_TYPE::OTHER:
		break;

	case Controller::JOYPAD_TYPE::XBOX_360:
	case Controller::JOYPAD_TYPE::XBOX_ONE:
	{
		auto d = GetJPadDInputState();
		auto x = GetJPadXInputState();

		//   Y
		// X   B
		//   A

		switch (idx)
		{
		case XINPUT_BUTTON_DPAD_UP:
			res = x.Buttons[0] == 1;// UP
			break;
		case XINPUT_BUTTON_DPAD_DOWN:
			res = x.Buttons[1] == 1;// DOWN
			break;
		case XINPUT_BUTTON_DPAD_LEFT:
			res = x.Buttons[2] == 1;// LEFT
			break;
		case XINPUT_BUTTON_DPAD_RIGHT:
			res = x.Buttons[3] == 1;// RIGHT
			break;
		case XINPUT_BUTTON_Y:
			res = d.Buttons[3] == INPUT_SENSI;// Y
			break;
		case XINPUT_BUTTON_X:
			res = d.Buttons[2] == INPUT_SENSI;// X
			break;
		case XINPUT_BUTTON_B:
			res = d.Buttons[1] == INPUT_SENSI;// B
			break;
		case XINPUT_BUTTON_A:
			res = d.Buttons[0] == INPUT_SENSI;// A
			break;
		case PAD_INPUT_L:
			res = d.Buttons[4] == INPUT_SENSI;// L
			break;
		case PAD_INPUT_R:
			res = d.Buttons[5] == INPUT_SENSI;// R
			break;
		case XINPUT_BUTTON_LEFT_SHOULDER:
			res = x.LeftTrigger >= INPUT_SENSI; // L2_TRIGGER
			break;
		case XINPUT_BUTTON_RIGHT_SHOULDER:
			res = x.RightTrigger >= INPUT_SENSI;// R2_TRIGGER
			break;
		case XINPUT_BUTTON_START:
			res = d.Buttons[7] == INPUT_SENSI;// START
			break;
		case XINPUT_BUTTON_BACK:
			res = d.Buttons[6] == INPUT_SENSI;// SELECT
			break;
		case XINPUT_BUTTON_LEFT_THUMB:
			res = d.Buttons[9] == INPUT_SENSI;// R3
			break;
		case XINPUT_BUTTON_RIGHT_THUMB:
			res = d.Buttons[8] == INPUT_SENSI;// L3
			break;
		default:

			// 左スティック
			analogKeyLX = d.X;
			analogKeyLY = d.Y;

			// 右スティック
			analogKeyRX = d.Rx;
			analogKeyRY = d.Ry;

			break;
		}
	}
	break;

	case Controller::JOYPAD_TYPE::DUAL_SHOCK_3:
	case Controller::JOYPAD_TYPE::DUAL_SHOCK_4:
	case Controller::JOYPAD_TYPE::DUAL_SENSE:
	{
		auto d = GetJPadDInputState();

		//   △
		// □  〇
		//   ×
		switch (idx)
		{
		case XINPUT_BUTTON_DPAD_UP:
			if ((int)d.POV[0] == 0 || (int)d.POV[0] == 4500 || (int)d.POV[0] == 31500) {
				res = true;// UP
			}
			else {
				res = false;
			};
			break;
		case XINPUT_BUTTON_DPAD_DOWN:
			if ((int)d.POV[0] == 13500 || (int)d.POV[0] == 18000 || (int)d.POV[0] == 22500) {
				res = true;// DOWN
			}
			else {
				res = false;
			}
			break;
		case XINPUT_BUTTON_DPAD_LEFT:
			if ((int)d.POV[0] == 22500 || (int)d.POV[0] == 27000 || (int)d.POV[0] == 31500) {
				res = true;// LEFT
			}
			else {
				res = false;
			}
			break;
		case XINPUT_BUTTON_DPAD_RIGHT:
			if ((int)d.POV[0] == 4500 || (int)d.POV[0] == 9000 || (int)d.POV[0] == 13500) {
				res = true;// RIGHT
			}
			else {
				res = false;
			}
			break;
		case XINPUT_BUTTON_Y:
			res = d.Buttons[3] == INPUT_SENSI;// △
			break;
		case XINPUT_BUTTON_X:
			res = d.Buttons[0] == INPUT_SENSI;// □
			break;
		case XINPUT_BUTTON_B:
			res = d.Buttons[2] == INPUT_SENSI;// ×
			break;
		case XINPUT_BUTTON_A:
			res = d.Buttons[1] == INPUT_SENSI;// 〇
			break;
		case PAD_INPUT_L:
			res = d.Buttons[4] == INPUT_SENSI;// L
			break;
		case PAD_INPUT_R:
			res = d.Buttons[5] == INPUT_SENSI;// R
			break;
		case XINPUT_BUTTON_LEFT_SHOULDER:
			res = d.Buttons[6] == INPUT_SENSI;// L2
			break;
		case XINPUT_BUTTON_RIGHT_SHOULDER:
			res = d.Buttons[7] == INPUT_SENSI;// R2
			break;
		case XINPUT_BUTTON_START:
			res = d.Buttons[9] == INPUT_SENSI;// START
			break;
		case XINPUT_BUTTON_BACK:
			if (d.Buttons[8] == 1 || d.Buttons[13] == 1) {
				res = true;// SELECT
			}
			else {
				res = false;
			}
			break;
		case XINPUT_BUTTON_LEFT_THUMB:
			res = d.Buttons[10] == INPUT_SENSI;// L3
		break; 
		case XINPUT_BUTTON_RIGHT_THUMB:
			res = d.Buttons[11] == INPUT_SENSI;// R3
			break;
		default:

			// 左スティック
			analogKeyLX = d.X;
			analogKeyLY = d.Y;

			// 右スティック
			analogKeyRX = d.Z;
			analogKeyRY = d.Rz;
			break;
		}
	}
	break;

	case Controller::JOYPAD_TYPE::SWITCH_JOY_CON_L:
		break;

	case Controller::JOYPAD_TYPE::SWITCH_JOY_CON_R:
		break;

	case Controller::JOYPAD_TYPE::SWITCH_PRO_CTRL:
		break;

	case Controller::JOYPAD_TYPE::MAX:
		break;
	}
	return res;
}

DINPUT_JOYSTATE Controller::GetJPadDInputState(void)
{
	// コントローラ情報
	GetJoypadDirectInputState(PAD_NUMBER, &joyDInState_);
	return joyDInState_;
}

XINPUT_STATE Controller::GetJPadXInputState(void)
{
	// コントローラ情報
	GetJoypadXInputState(PAD_NUMBER, &joyXInState_);
	return joyXInState_;
}

void Controller::SetJPadInState(void)
{
	anyone_ = anyoneKey_ = isAnyoneDown_ = false;

	// キーボード検知
	for (auto& p : infos_)
	{
		// コントローラー以外は更新しない
		if (p.second.type == KEYPAD_NO::KEY || p.second.type == KEYPAD_NO::MOUSE) {

			continue;
		}
		p.second.keyOld = p.second.keyNew;

		// コントローラーの取得
		p.second.keyNew = GetJPadInputState(p.second.key);

		InputBase::Update(p);
	}


}

VECTOR Controller::GetDirectionXZAKey(int aKeyX, int aKeyY)
{
	VECTOR ret = { 0.0f, 0.0f, 0.0f };

	// スティックの個々の入力値は、
	// -1000.0f ～ 1000.0f の範囲で返ってくるが、
	// X:1000.0f、Y:1000.0fになることは無い(1000と500くらいが最大)
	// スティックの入力値を -1.0 ～ 1.0 に正規化
	float dirX = static_cast<float>(aKeyX) / AKEY_VAL_MAX;
	float dirZ = static_cast<float>(aKeyY) / AKEY_VAL_MAX;

	// ピタゴラスの定理でニュートラル状態からの長さベクトルにする
	// ( 円形のデッドゾーンになる )
	// 平方根により、おおよその最大値が1.0となる
	float len = sqrtf(dirX * dirX + dirZ * dirZ);

	if (len < THRESHOLD) {
		// (0.0f, 0.0f, 0.0f)
		return ret;
	}

	// デッドゾーン境界からに再スケーリング(可変デッドゾーン)
	float scale = (len - THRESHOLD) / (1.0f - THRESHOLD);
	dirX = (dirX / len) * scale;
	dirZ = (dirZ / len) * scale;

	// Zは前に倒すとマイナス値が返ってくるので反転
	ret = VNorm({ dirX, 0.0f, -dirZ });
	return ret;
}
