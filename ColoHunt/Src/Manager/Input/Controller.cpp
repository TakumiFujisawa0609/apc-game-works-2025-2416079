#include <cmath>
#include "Controller.h"


Controller::Controller(int num) : CONTROLLER_NUM(num)
{
}

Controller::~Controller(void)
{
}

void Controller::Init(void)
{
	anyoneTrgDown_ = anyoneTrg_ = anyone_ = false;
}

void Controller::Update(void)
{
	// 初期化
	anyoneTrgDown_ = anyoneTrg_ = anyone_ = false;

	// パッド検知
	for (auto& p : infos_)
	{
		p.second.keyOld = p.second.keyNew;
		if (p.second.type == TYPE::PAD) {
			p.second.keyNew = GetJPadInputState(p.second.key);
		}
		InputBase::Update(p);
	}
	// スティックの更新
	bool stick = GetJPadInputState(-1);
	if (!anyone_) {
		anyone_ = stick;
	}
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

	if (len < THRESHOLD){
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

bool Controller::GetJPadInputState(int key)
{
	auto type = GetJPadDType();

	auto d = GetJPadDInputState();
	auto x = GetJPadXInputState();

	switch (type){
	case Controller::JOYPAD_TYPE::OTHER:
		break;

	case Controller::JOYPAD_TYPE::XBOX_360:
	case Controller::JOYPAD_TYPE::XBOX_ONE:

		//   Y
		// X   B
		//   A

		switch (key)
		{
		case XINPUT_BUTTON_Y:
			return  d.Buttons[3] == 128;
			break;
		case XINPUT_BUTTON_X:
			return  d.Buttons[2] == 128;
			break;
		case XINPUT_BUTTON_B:
			return  d.Buttons[1] == 128;
			break;
		case XINPUT_BUTTON_A:
			return  d.Buttons[0] == 128;
			break;
		case XINPUT_BUTTON_DPAD_UP:
			return x.Buttons[0];
			break;
		case XINPUT_BUTTON_DPAD_DOWN:
			return x.Buttons[1];
			break;
		case XINPUT_BUTTON_DPAD_LEFT:
			return x.Buttons[2];
			break;
		case XINPUT_BUTTON_DPAD_RIGHT:
			return x.Buttons[3];
			break;
		case PAD_INPUT_L:
			return  d.Buttons[4] == 128;
			break;
		case PAD_INPUT_R:
			return  d.Buttons[5] == 128;
			break;
		case XINPUT_BUTTON_LEFT_SHOULDER:
			return x.LeftTrigger;
			break;
		case XINPUT_BUTTON_RIGHT_SHOULDER:
			return x.RightTrigger;
			break;
		case XINPUT_BUTTON_LEFT_THUMB:
			return  d.Buttons[8] == 128;
			break;
		case XINPUT_BUTTON_RIGHT_THUMB:
			return  d.Buttons[9] == 128;
			break;
		case XINPUT_BUTTON_START:
			return  d.Buttons[7] == 128;
			break;
		case XINPUT_BUTTON_BACK:
			return  d.Buttons[6] == 128;
			break;
		case -1:
			// 左スティック
			analogKeyLX = d.X;
			analogKeyLY = d.Y;
			// 右スティック
			analogKeyRX = d.Rx;
			analogKeyRY = d.Ry;
			if (analogKeyLX == analogKeyLY == analogKeyRX == analogKeyRY == 0) {
				return false;
			}
			return true;
			break;
		}
	break;

	case Controller::JOYPAD_TYPE::DUAL_SHOCK_3:
	case Controller::JOYPAD_TYPE::DUAL_SHOCK_4:
	case Controller::JOYPAD_TYPE::DUAL_SENSE:

		//   △
		// □  〇
		//   ×

		switch (key)
		{
		case XINPUT_BUTTON_Y:
			return  d.Buttons[3] == 128;
			break;
		case XINPUT_BUTTON_X:
			return  d.Buttons[0] == 128;
			break;
		case XINPUT_BUTTON_B:
			return  d.Buttons[2] == 128;
			break;
		case XINPUT_BUTTON_A:
			return  d.Buttons[1] == 128;
			break;
		case XINPUT_BUTTON_DPAD_UP:
			if ((int)d.POV[0] == 0 || (int)d.POV[0] == 4500 || (int)d.POV[0] == 31500) {
				return true;
			}
			else {
				return false;
			}
			break;
		case XINPUT_BUTTON_DPAD_DOWN:
			if ((int)d.POV[0] == 13500 || (int)d.POV[0] == 18000 || (int)d.POV[0] == 22500) {
				return true;
			}
			else {
				return false;
			}
			break;
		case XINPUT_BUTTON_DPAD_LEFT:
			if ((int)d.POV[0] == 22500 || (int)d.POV[0] == 27000 || (int)d.POV[0] == 31500) {
				return true;
			}
			else {
				return false;
			}
			break;
		case XINPUT_BUTTON_DPAD_RIGHT:
			if ((int)d.POV[0] == 4500 || (int)d.POV[0] == 9000 || (int)d.POV[0] == 13500) {
				return true;
			}
			else {
				return false;
			}
			break;
		case PAD_INPUT_L:
			return  d.Buttons[4] == 128;
			break;
		case PAD_INPUT_R:
			return  d.Buttons[5] == 128;
			break;
		case XINPUT_BUTTON_LEFT_SHOULDER:
			return  d.Buttons[6] == 128;
			break;
		case XINPUT_BUTTON_RIGHT_SHOULDER:
			return  d.Buttons[7] == 128;
			break;
		case XINPUT_BUTTON_LEFT_THUMB:
			return  d.Buttons[10] == 128;
			break;
		case XINPUT_BUTTON_RIGHT_THUMB:
			return  d.Buttons[11] == 128;
			break;
		case XINPUT_BUTTON_START:
			return  d.Buttons[9] == 128;
			break;
		case XINPUT_BUTTON_BACK:
			return  d.Buttons[8] == 128;
			break;
		case -1:
			// 左スティック
			analogKeyLX = d.X;
			analogKeyLY = d.Y;
			// 右スティック
			analogKeyRX = d.Z;
			analogKeyRY = d.Rz;
			if (analogKeyLX == analogKeyLY == analogKeyRX == analogKeyRY == 0) {
				return false;
			}
			return true;
			break;
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
	return false;
}

Controller::JOYPAD_TYPE Controller::GetJPadDType(void)
{
	return static_cast<Controller::JOYPAD_TYPE>(GetJoypadType(CONTROLLER_NUM));
}

DINPUT_JOYSTATE Controller::GetJPadDInputState(void)
{
	// コントローラ情報
	GetJoypadDirectInputState(CONTROLLER_NUM, &joyDInState_);
	return joyDInState_;
}

XINPUT_STATE Controller::GetJPadXInputState(void)
{
	// コントローラ情報
	GetJoypadXInputState(CONTROLLER_NUM, &joyXInState_);
	return joyXInState_;
}
