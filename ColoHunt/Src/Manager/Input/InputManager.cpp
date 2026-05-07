#include <fstream>
#include <vector>
#include "InputManager.h"
#include "KeyMouse.h"
#include "../../Application.h"
#include "../../Utility/Utility.h"


InputManager* InputManager::inst_ = nullptr;

void InputManager::CreateInstance(void)
{
	inst_ = new InputManager();
}

InputManager& InputManager::GetInstance(void)
{
	if (inst_ == nullptr) {

		CreateInstance();
	}
	return *inst_;
}

// 初期化
void InputManager::Init(void){

	// キーマウ取得
	keyMou_ = new KeyMouse();
	keyMou_->Init();
	
	// キーの登録
	keyMou_->Add(MOUSE_INPUT_LEFT, InputBase::TYPE::MOUSE);
	keyMou_->Add(MOUSE_INPUT_RIGHT, InputBase::TYPE::MOUSE);
	keyMou_->Add(MOUSE_INPUT_MIDDLE, InputBase::TYPE::MOUSE);
	keyMou_->Add(KEY_INPUT_W, InputBase::TYPE::KEY);
	keyMou_->Add(KEY_INPUT_A, InputBase::TYPE::KEY);
	keyMou_->Add(KEY_INPUT_S, InputBase::TYPE::KEY);
	keyMou_->Add(KEY_INPUT_D, InputBase::TYPE::KEY);
	keyMou_->Add(KEY_INPUT_RSHIFT, InputBase::TYPE::KEY);
	keyMou_->Add(KEY_INPUT_SPACE, InputBase::TYPE::KEY);
	keyMou_->Add(KEY_INPUT_F, InputBase::TYPE::KEY);
	keyMou_->Add(KEY_INPUT_Q, InputBase::TYPE::KEY);
	keyMou_->Add(KEY_INPUT_E, InputBase::TYPE::KEY);
	keyMou_->Add(KEY_INPUT_LEFT, InputBase::TYPE::KEY);
	keyMou_->Add(KEY_INPUT_RIGHT, InputBase::TYPE::KEY);
	keyMou_->Add(KEY_INPUT_UP, InputBase::TYPE::KEY);
	keyMou_->Add(KEY_INPUT_DOWN, InputBase::TYPE::KEY);

	// パッド取得
	for (int num = static_cast<int>(KEYPAD_NO::PAD1); num < static_cast<int>(KEYPAD_NO::MAX); num++) {
		
		pads_.emplace_back(new Controller(num));
		pads_.back()->Init();

		pads_.back()->Add(XINPUT_BUTTON_A, InputBase::TYPE::PAD);
		pads_.back()->Add(XINPUT_BUTTON_B, InputBase::TYPE::PAD);
		pads_.back()->Add(XINPUT_BUTTON_X, InputBase::TYPE::PAD);
		pads_.back()->Add(XINPUT_BUTTON_Y, InputBase::TYPE::PAD);
		pads_.back()->Add(XINPUT_BUTTON_DPAD_UP, InputBase::TYPE::PAD);
		pads_.back()->Add(XINPUT_BUTTON_DPAD_DOWN, InputBase::TYPE::PAD);
		pads_.back()->Add(XINPUT_BUTTON_DPAD_LEFT, InputBase::TYPE::PAD);
		pads_.back()->Add(XINPUT_BUTTON_DPAD_RIGHT, InputBase::TYPE::PAD);
		pads_.back()->Add(PAD_INPUT_L, InputBase::TYPE::PAD);
		pads_.back()->Add(PAD_INPUT_R, InputBase::TYPE::PAD);
		pads_.back()->Add(XINPUT_BUTTON_BACK, InputBase::TYPE::PAD);
		pads_.back()->Add(XINPUT_BUTTON_START, InputBase::TYPE::PAD);
	}
}

// 更新
void InputManager::Update(void){

	// 優先を初期化
	orderOfPriority_.clear();
	mostPriority_ = KEYPAD_NO::NON;
	mostPriorityType_ = Controller::JOYPAD_TYPE::NON;

	// 現在の接続しているパッドの数のみ更新(1番から)
	for (int num = static_cast<int>(KEYPAD_NO::PAD1) - 1; num < GetJoypadNum(); num++) {

		pads_.at(num)->Update();
		// 優先順に動いているか
		orderOfPriority_.emplace_back(pads_.at(num)->GetAnyone());
		// 最も優先的に動いているもの
		if (orderOfPriority_.back() && mostPriority_ == KEYPAD_NO::NON) {
			mostPriority_ = static_cast<KEYPAD_NO>(num + 1);
			mostPriorityType_ = pads_.at(num)->GetJPadType();
		}
	}

	// キーマウ更新
	keyMou_->Update();
	orderOfPriority_.emplace_back(keyMou_->GetAnyone());

	// 最も優先的に動いているもの
	if (mostPriority_ == KEYPAD_NO::NON) {
		if (orderOfPriority_.back()) {
			mostPriority_ = KEYPAD_NO::KEY;
			mostPriorityType_ = Controller::JOYPAD_TYPE::NON;
		}
		// どれも動いてないときはPAD1を取るようにする
		else {
			orderOfPriority_.emplace_back(KEYPAD_NO::PAD1, true);
			mostPriority_ = KEYPAD_NO::PAD1;
			mostPriorityType_ = pads_.at(static_cast<int>(KEYPAD_NO::PAD1) - 1)->GetJPadType();
			if (mostPriorityType_ == Controller::JOYPAD_TYPE::NON) {
				mostPriorityType_ = Controller::JOYPAD_TYPE::XBOX_360;
			}
		}
	}
}

// リソースの破棄
void InputManager::Release(void){

	// 解放
	keyMou_->Release();
	for (auto p : pads_) {

		p->Release();
		delete p;
	}
	delete keyMou_;
	pads_.clear();
}

std::map<InputManager::KEYPAD_NO, VECTOR>  InputManager::GetDirectionXZAKeyL(void)
{
	std::map<KEYPAD_NO, VECTOR>  res;

	VECTOR nowPos = Utility::VECTOR_ZERO;

	// WASDをアナログに変換
	if (keyMou_->GetKey(KEY_INPUT_W).keyNew) {
		nowPos.z += 1.0f;
	}
	if (keyMou_->GetKey(KEY_INPUT_S).keyNew) {
		nowPos.z -= 1.0f;
	}
	if (keyMou_->GetKey(KEY_INPUT_D).keyNew) {
		nowPos.x += 1.0f;
	}
	if (keyMou_->GetKey(KEY_INPUT_A).keyNew) {
		nowPos.x = 1.0f;
	}
	res[KEYPAD_NO::KEY] = VNorm(nowPos);

	// 現在の接続しているパッドの数のみ取得(1番から)
	for (int num = static_cast<int>(KEYPAD_NO::PAD1) - 1; num < GetJoypadNum(); num++) {

		nowPos = pads_.at(num)->GetDirectionXZAKeyL();
		res[static_cast<KEYPAD_NO>(num)] = nowPos;
	}
	return res;
}

std::map<InputManager::KEYPAD_NO, VECTOR>  InputManager::GetDirectionXZAKeyR(void)
{
	std::map<KEYPAD_NO, VECTOR>  res;
	
	//　中心からの差分をとる
	VECTOR nowPos = keyMou_->GetMousePos();
	VECTOR diffPos = VGet(Application::SCREEN_SIZE_X_HALF - nowPos.x, 0.0f, Application::SCREEN_SIZE_Y - nowPos.y);
	res[KEYPAD_NO::KEY] = diffPos;

	// 現在の接続しているパッドの数のみ取得(1番から)
	for (int num = static_cast<int>(KEYPAD_NO::PAD1) - 1; num < GetJoypadNum(); num++) {

		nowPos = pads_.at(num)->GetDirectionXZAKeyR();
		res[static_cast<KEYPAD_NO>(num)] = nowPos;
	}
	return res;
}

InputBase::Info InputManager::GetKey(COMMAND com, KEYPAD_NO no)
{
	if (no == KEYPAD_NO::KEY) {
		return keyMou_->GetKey(keyCommand_.at(com));
	}
	return pads_.at(static_cast<int>(no) - 1)->GetKey(padCommand_.at(com));
}

InputBase::Info InputManager::GetPriorityKey(COMMAND com)
{
	switch (mostPriority_)
	{
		case KEYPAD_NO::KEY:
			return GetKey(com, KEYPAD_NO::KEY);
			break;
		case KEYPAD_NO::PAD1:
			return GetKey(com, KEYPAD_NO::PAD1);
			break;
		case KEYPAD_NO::PAD2:
			return GetKey(com, KEYPAD_NO::PAD2);
			break;
		case KEYPAD_NO::PAD3:
			return GetKey(com, KEYPAD_NO::PAD3);
			break;
		case KEYPAD_NO::PAD4:
			return GetKey(com, KEYPAD_NO::PAD4);
			break;
		default:
		break;
	}
}

std::vector<InputBase::Info> InputManager::GetPriorityKey(COMMAND com, int num)
{
	std::vector<InputBase::Info> temp;
	for (int i = 0; i < static_cast<int>(KEYPAD_NO::MAX); i++) {
		if (orderOfPriority_.at(i)) {
			switch (i)
			{
			case static_cast<int>(KEYPAD_NO::KEY):
				temp.emplace_back(GetKey(com, KEYPAD_NO::KEY));
				break;
			case static_cast<int>(KEYPAD_NO::PAD1):
				temp.emplace_back(GetKey(com, KEYPAD_NO::PAD1));
				break;
			case static_cast<int>(KEYPAD_NO::PAD2):
				temp.emplace_back(GetKey(com, KEYPAD_NO::PAD2));
				break;
			case static_cast<int>(KEYPAD_NO::PAD3):
				temp.emplace_back(GetKey(com, KEYPAD_NO::PAD3));
				break;
			case static_cast<int>(KEYPAD_NO::PAD4):
				temp.emplace_back(GetKey(com, KEYPAD_NO::PAD4));
				break;
			default:
				break;
			}
		}
	}
	// 多いなら減らす 
	while (temp.size() > num) {

		temp.pop_back();
	}
	// 少なくてもそのまま
	return temp;
}

InputManager::InputManager(void)
{
}

InputManager::~InputManager(void)
{
}

void InputManager::CommondLoad(void)
{

	// ダッシュキー
	keyCommand_.emplace(COMMAND::RUN, KEY_INPUT_LSHIFT);
	padCommand_.emplace(COMMAND::RUN, PAD_INPUT_R);
	
	// 攻撃キー
	keyCommand_.emplace(COMMAND::ATTACK, MOUSE_INPUT_RIGHT);
	padCommand_.emplace(COMMAND::ATTACK, XINPUT_BUTTON_B);

	// コンボキー
	keyCommand_.emplace(COMMAND::COMBO, MOUSE_INPUT_LEFT);
	padCommand_.emplace(COMMAND::COMBO, XINPUT_BUTTON_Y);

	// 回避キー
	keyCommand_.emplace(COMMAND::DODGE, KEY_INPUT_SPACE);
	padCommand_.emplace(COMMAND::DODGE, XINPUT_BUTTON_A);

	// ロックオンキー
	keyCommand_.emplace(COMMAND::LOCK_ON, MOUSE_INPUT_MIDDLE);
	padCommand_.emplace(COMMAND::LOCK_ON, PAD_INPUT_L);

	// 使うキー
	keyCommand_.emplace(COMMAND::USE, KEY_INPUT_F);
	padCommand_.emplace(COMMAND::USE, XINPUT_BUTTON_X);

	// 左キー
	keyCommand_.emplace(COMMAND::LEFT, KEY_INPUT_Q);
	keyCommand_.emplace(COMMAND::LEFT, KEY_INPUT_LEFT);
	padCommand_.emplace(COMMAND::LEFT, XINPUT_BUTTON_DPAD_LEFT);

	// 右キー
	keyCommand_.emplace(COMMAND::RIGHT, KEY_INPUT_E);
	keyCommand_.emplace(COMMAND::RIGHT, KEY_INPUT_RIGHT);
	padCommand_.emplace(COMMAND::RIGHT, XINPUT_BUTTON_DPAD_RIGHT);

	// 上キー
	keyCommand_.emplace(COMMAND::UP, KEY_INPUT_W);
	keyCommand_.emplace(COMMAND::UP, KEY_INPUT_UP);
	padCommand_.emplace(COMMAND::UP, XINPUT_BUTTON_DPAD_UP);

	// 下キー
	keyCommand_.emplace(COMMAND::DOWN, KEY_INPUT_S);
	keyCommand_.emplace(COMMAND::DOWN, KEY_INPUT_DOWN);
	padCommand_.emplace(COMMAND::DOWN, XINPUT_BUTTON_DPAD_DOWN);

	// ポーズキー
	keyCommand_.emplace(COMMAND::PAUSE, KEY_INPUT_ESCAPE);
	padCommand_.emplace(COMMAND::PAUSE, XINPUT_BUTTON_BACK);
	padCommand_.emplace(COMMAND::PAUSE, XINPUT_BUTTON_START);

	// 決定キー
	keyCommand_.emplace(COMMAND::DECIDE, MOUSE_INPUT_LEFT);
	padCommand_.emplace(COMMAND::DECIDE, XINPUT_BUTTON_A);

	// キャンセルキー
	keyCommand_.emplace(COMMAND::CANCEL, MOUSE_INPUT_RIGHT);
	keyCommand_.emplace(COMMAND::CANCEL, KEY_INPUT_ESCAPE);
	padCommand_.emplace(COMMAND::CANCEL, XINPUT_BUTTON_B);
}
