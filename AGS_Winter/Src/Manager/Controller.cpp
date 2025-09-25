#include <cmath>
#include "Controller.h"


Controller* Controller::inst_ = nullptr;

void Controller::CreateInstance(void)
{
	inst_ = new Controller();
}

Controller& Controller::GetInstance(void)
{
	if (inst_ == nullptr) {

		CreateInstance();
	}
	return *inst_;
}

void Controller::Init(void)
{
}

void Controller::Update(void)
{
	// パッド情報
	//SetJPadInState(JOYPAD_NO::KEY_PAD1);
	for (int i = 1; i < GetJoypadNum()+1; i++) {
	
		SetJPadInState(static_cast<Controller::JOYPAD_NO>(i));
	}
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
	// ( しきい値 0.35 の場合は、 0.0 ～ 0.65 / 0.65 になる )
	float scale = (len - THRESHOLD) / (1.0f - THRESHOLD);
	dirX = (dirX / len) * scale;
	dirZ = (dirZ / len) * scale;
	
	// Zは前に倒すとマイナス値が返ってくるので反転
	ret = VNorm({ dirX, 0.0f, -dirZ });
	return ret;
}

Controller::JOYPAD_IN_STATE Controller::GetJPadInputState(JOYPAD_NO no)
{
	JOYPAD_IN_STATE ret = JOYPAD_IN_STATE();

	auto type = GetJPadDType(no);

	switch (type){
	case Controller::JOYPAD_TYPE::OTHER:
		break;

	case Controller::JOYPAD_TYPE::XBOX_360:
	{
	}
	break;

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
		ret.ButtonsNew[idx] = d.Buttons[9];// START

		idx = static_cast<int>(JOYPAD_BTN::L3_PUSH);
		ret.ButtonsNew[idx] = d.Buttons[8];// SELECT

		// 左スティック
		ret.AKeyLX = d.X;
		ret.AKeyLY = d.Y;

		// 右スティック
		ret.AKeyRX = d.Rx;
		ret.AKeyRY = d.Ry;

	}
	break;

	case Controller::JOYPAD_TYPE::DUAL_SHOCK_4:
		break;

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

Controller::JOYPAD_TYPE Controller::GetJPadDType(JOYPAD_NO no)
{
	return static_cast<Controller::JOYPAD_TYPE>(GetJoypadType(static_cast<int>(no)));
}

DINPUT_JOYSTATE Controller::GetJPadDInputState(JOYPAD_NO no)
{
	// コントローラ情報
	GetJoypadDirectInputState(static_cast<int>(no), &joyDInState_);
	return joyDInState_;
}

XINPUT_STATE Controller::GetJPadXInputState(JOYPAD_NO no)
{
	// コントローラ情報
	GetJoypadXInputState(static_cast<int>(no), &joyXInState_);
	return joyXInState_;
}

void Controller::SetJPadInState(JOYPAD_NO jpNo)
{
	int no = static_cast<int>(jpNo);
	auto stateNew = GetJPadInputState(jpNo);
	auto& stateNow = padInfos_[no];

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


		stateNow.AKeyLX = stateNew.AKeyLX;
		stateNow.AKeyLY = stateNew.AKeyLY;
		stateNow.AKeyRX = stateNew.AKeyRX;
		stateNow.AKeyRY = stateNew.AKeyRY;
	}
}
