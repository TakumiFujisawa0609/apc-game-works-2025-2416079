#include <cmath>
#include "Controller.h"


Controller::Controller(void)
{
}

Controller::~Controller(void)
{
}

void Controller::Init(void)
{
}

void Controller::Update(int i)
{
	// パッド情報
	SetJPadInState(i);
}

void Controller::Destroy(void)
{
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

Controller::JOYPAD_IN_STATE Controller::GetJPadInputState(int no)
{
	JOYPAD_IN_STATE ret = JOYPAD_IN_STATE();

	auto type = GetJPadDType(no);

	switch (type){
	case Controller::JOYPAD_TYPE::OTHER:
		break;

	case Controller::JOYPAD_TYPE::XBOX_360:
	case Controller::JOYPAD_TYPE::XBOX_ONE:
	{

		auto d = GetJPadDInputState(no);
		auto x = GetJPadXInputState(no);

		int idx;

		//   Y
		// X   B
		//   A

		idx = static_cast<int>(JOYPAD_BTN::TOP);
		ret.ButtonsNew[idx] = d.Buttons[3];// Y

		idx = static_cast<int>(JOYPAD_BTN::LEFT);
		ret.ButtonsNew[idx] = d.Buttons[2];// X

		idx = static_cast<int>(JOYPAD_BTN::RIGHT);
		ret.ButtonsNew[idx] = d.Buttons[1];// B

		idx = static_cast<int>(JOYPAD_BTN::DOWN);
		ret.ButtonsNew[idx] = d.Buttons[0];// A
		
		idx = static_cast<int>(JOYPAD_BTN::TOP_DPAD);
		ret.ButtonsNew[idx] = x.Buttons[0];// UP

		idx = static_cast<int>(JOYPAD_BTN::LEFT_DPAD);
		ret.ButtonsNew[idx] = x.Buttons[2];// LEFT

		idx = static_cast<int>(JOYPAD_BTN::RIGHT_DPAD);
		ret.ButtonsNew[idx] = x.Buttons[3];// RIGHT

		idx = static_cast<int>(JOYPAD_BTN::DOWN_DPAD);
		ret.ButtonsNew[idx] = x.Buttons[1];// DOWN

		idx = static_cast<int>(JOYPAD_BTN::R);
		ret.ButtonsNew[idx] = d.Buttons[5];// R

		idx = static_cast<int>(JOYPAD_BTN::L);
		ret.ButtonsNew[idx] = d.Buttons[4];// L

		idx = static_cast<int>(JOYPAD_BTN::R2_TRIGGER);
		ret.ButtonsNew[idx] = x.RightTrigger;// R2_TRIGGER

		idx = static_cast<int>(JOYPAD_BTN::L2_TRIGGER);
		ret.ButtonsNew[idx] = x.LeftTrigger; // L2_TRIGGER

		idx = static_cast<int>(JOYPAD_BTN::START);
		ret.ButtonsNew[idx] = d.Buttons[7];// START

		idx = static_cast<int>(JOYPAD_BTN::SELECT);
		ret.ButtonsNew[idx] = d.Buttons[6];// SELECT
		
		idx = static_cast<int>(JOYPAD_BTN::R3_PUSH);
		ret.ButtonsNew[idx] = d.Buttons[9];// R3

		idx = static_cast<int>(JOYPAD_BTN::L3_PUSH);
		ret.ButtonsNew[idx] = d.Buttons[8];// L3

		// 左スティック
		ret.AKeyLX = d.X;
		ret.AKeyLY = d.Y;

		// 右スティック
		ret.AKeyRX = d.Rx;
		ret.AKeyRY = d.Ry;

	}
	break;

	case Controller::JOYPAD_TYPE::DUAL_SHOCK_3:
	case Controller::JOYPAD_TYPE::DUAL_SHOCK_4:
	case Controller::JOYPAD_TYPE::DUAL_SENSE:
	{

		auto d = GetJPadDInputState(no);

		int idx;

		//   △
		// □  〇
		//   ×

		idx = static_cast<int>(JOYPAD_BTN::TOP);
		ret.ButtonsNew[idx] = d.Buttons[3];// △

		idx = static_cast<int>(JOYPAD_BTN::LEFT);
		ret.ButtonsNew[idx] = d.Buttons[0];// □

		idx = static_cast<int>(JOYPAD_BTN::RIGHT);
		ret.ButtonsNew[idx] = d.Buttons[2];// 〇

		idx = static_cast<int>(JOYPAD_BTN::DOWN);
		ret.ButtonsNew[idx] = d.Buttons[1];// ×

		idx = static_cast<int>(JOYPAD_BTN::TOP_DPAD);
		if ((int)d.POV[0] == 0 || (int)d.POV[0] == 4500 || (int)d.POV[0] == 31500) {
		
			ret.ButtonsNew[idx] = 128;// TOP
		}
		else {

			ret.ButtonsNew[idx] = 0;
		}

		idx = static_cast<int>(JOYPAD_BTN::LEFT_DPAD);
		if ((int)d.POV[0] == 22500 || (int)d.POV[0] == 27000 || (int)d.POV[0] == 31500) {

			ret.ButtonsNew[idx] = 128;// TOP
		}
		else {

			ret.ButtonsNew[idx] = 0;
		}

		idx = static_cast<int>(JOYPAD_BTN::RIGHT_DPAD);
		if ((int)d.POV[0] == 4500 || (int)d.POV[0] == 9000 || (int)d.POV[0] == 13500) {

			ret.ButtonsNew[idx] = 128;// TOP
		}
		else {

			ret.ButtonsNew[idx] = 0;
		}

		idx = static_cast<int>(JOYPAD_BTN::DOWN_DPAD);
		if ((int)d.POV[0] == 13500 || (int)d.POV[0] == 18000 || (int)d.POV[0] == 22500) {

			ret.ButtonsNew[idx] = 128;// TOP
		}
		else {

			ret.ButtonsNew[idx] = 0;
		}

		idx = static_cast<int>(JOYPAD_BTN::R);
		ret.ButtonsNew[idx] = d.Buttons[5];// R

		idx = static_cast<int>(JOYPAD_BTN::L);
		ret.ButtonsNew[idx] = d.Buttons[4];// L

		idx = static_cast<int>(JOYPAD_BTN::R2_TRIGGER);
		ret.ButtonsNew[idx] = d.Buttons[7];// R2

		idx = static_cast<int>(JOYPAD_BTN::L2_TRIGGER);
		ret.ButtonsNew[idx] = d.Buttons[6];// L2

		idx = static_cast<int>(JOYPAD_BTN::START);
		ret.ButtonsNew[idx] = d.Buttons[9];// START

		idx = static_cast<int>(JOYPAD_BTN::SELECT);
		if (d.Buttons[8] >= 1 || d.Buttons[13] >= 1) {

			ret.ButtonsNew[idx] = 128;// SELECT
		}
		else {
			ret.ButtonsNew[idx] = 0;
		}

		idx = static_cast<int>(JOYPAD_BTN::R3_PUSH);
		ret.ButtonsNew[idx] = d.Buttons[11];// R3

		idx = static_cast<int>(JOYPAD_BTN::L3_PUSH);
		ret.ButtonsNew[idx] = d.Buttons[10];// L3

		// 左スティック
		ret.AKeyLX = d.X;
		ret.AKeyLY = d.Y;

		// 右スティック
		ret.AKeyRX = d.Z;
		ret.AKeyRY = d.Rz;

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
	return ret;
}

Controller::JOYPAD_TYPE Controller::GetJPadDType(int no)
{
	return static_cast<Controller::JOYPAD_TYPE>(GetJoypadType(no));
}

DINPUT_JOYSTATE Controller::GetJPadDInputState(int no)
{
	// コントローラ情報
	GetJoypadDirectInputState(no, &joyDInState_);
	return joyDInState_;
}

XINPUT_STATE Controller::GetJPadXInputState(int no)
{
	// コントローラ情報
	GetJoypadXInputState(no, &joyXInState_);
	return joyXInState_;
}

void Controller::SetJPadInState(int jpNo)
{
	auto stateNew = GetJPadInputState(jpNo);
	auto& stateNow = padInfos_[jpNo];

	stateNow.AnyoneBotton = stateNow.Anyone = stateNow.IsAnyoneDown = false;

	int max = static_cast<int>(JOYPAD_BTN::MAX);
	for (int i = 0; i < max; i++)
	{
		stateNow.ButtonsOld[i] = stateNow.ButtonsNew[i];
		stateNow.ButtonsNew[i] = stateNew.ButtonsNew[i];

		stateNow.IsOld[i] = stateNow.IsNew[i];
		//stateNow.IsNew[i] = stateNow.ButtonsNew[i] == 128 || stateNow.ButtonsNew[i] == 255;
		stateNow.IsNew[i] = stateNow.ButtonsNew[i] > 0;

		stateNow.IsTrgDown[i] = stateNow.IsNew[i] && !stateNow.IsOld[i];
		stateNow.IsTrgUp[i] = !stateNow.IsNew[i] && stateNow.IsOld[i];

		if (!stateNow.AnyoneBotton) {
			
			stateNow.AnyoneBotton = stateNow.IsTrgDown[i];
			stateNow.Anyone = stateNow.IsTrgDown[i];
		}
		if (!stateNow.IsAnyoneDown) {

			stateNow.IsAnyoneDown = stateNow.IsNew[i] && !stateNow.IsOld[i];
		}
	}
	stateNow.AKeyLX = stateNew.AKeyLX;
	stateNow.AKeyLY = stateNew.AKeyLY;
	stateNow.AKeyRX = stateNew.AKeyRX;
	stateNow.AKeyRY = stateNew.AKeyRY;

	if (!stateNow.Anyone) {
		if (stateNow.AKeyLX != 0 || stateNow.AKeyLY != 0 || stateNow.AKeyRX != 0 || stateNow.AKeyRY != 0) {

			stateNow.Anyone = true;
		}
	}
}
