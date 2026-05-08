#include <fstream>
#include <vector>
#include "InputManager.h"
#include "Keyboard.h"
#include "../../Application.h"
#include "../../Utility/Utility.h"
#include "../../Utility/VectorUtility.h"


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

	// キーコマンドのロード
	CommandLoad();

	// キーマウ取得
	keyboard_ = new Keyboard();
	keyboard_->Init();
	mouse_ = new Mouse();
	mouse_->Init();
	
	// マウスの登録
	mouse_->Add(MOUSE_INPUT_LEFT);
	mouse_->Add(MOUSE_INPUT_RIGHT);
	mouse_->Add(MOUSE_INPUT_MIDDLE);
	
	// キーの登録
	keyboard_->Add(KEY_INPUT_W);
	keyboard_->Add(KEY_INPUT_A);
	keyboard_->Add(KEY_INPUT_S);
	keyboard_->Add(KEY_INPUT_D);
	keyboard_->Add(KEY_INPUT_LSHIFT);
	keyboard_->Add(KEY_INPUT_ESCAPE);
	keyboard_->Add(KEY_INPUT_SPACE);
	keyboard_->Add(KEY_INPUT_RETURN);
	keyboard_->Add(KEY_INPUT_RCONTROL);
	keyboard_->Add(KEY_INPUT_TAB);
	keyboard_->Add(KEY_INPUT_F);
	keyboard_->Add(KEY_INPUT_Q);
	keyboard_->Add(KEY_INPUT_E);
	keyboard_->Add(KEY_INPUT_LEFT);
	keyboard_->Add(KEY_INPUT_RIGHT);
	keyboard_->Add(KEY_INPUT_UP);
	keyboard_->Add(KEY_INPUT_DOWN);

	// パッド取得
	for (int num = static_cast<int>(KEYPAD_NO::PAD1); num < static_cast<int>(KEYPAD_NO::MAX); num++) {
		
		pads_.emplace_back(new Controller(num + 1));
		pads_.back()->Init();

		pads_.back()->Add(XINPUT_BUTTON_A);
		pads_.back()->Add(XINPUT_BUTTON_B);
		pads_.back()->Add(XINPUT_BUTTON_X);
		pads_.back()->Add(XINPUT_BUTTON_Y);
		pads_.back()->Add(XINPUT_BUTTON_DPAD_UP);
		pads_.back()->Add(XINPUT_BUTTON_DPAD_DOWN);
		pads_.back()->Add(XINPUT_BUTTON_DPAD_LEFT);
		pads_.back()->Add(XINPUT_BUTTON_DPAD_RIGHT);
		pads_.back()->Add(PAD_INPUT_L);
		pads_.back()->Add(PAD_INPUT_R);
		pads_.back()->Add(XINPUT_BUTTON_BACK);
		pads_.back()->Add(XINPUT_BUTTON_START);
	}
}

// 更新
void InputManager::Update(void){

	// 優先を初期化
	orderOfPriority_.clear();
	KEYPAD_NO tmp = mostPriority_;
	Controller::JOYPAD_TYPE tmpNo = mostPriorityNo_;
	mostPriority_ = KEYPAD_NO::NON;
	mostPriorityNo_ = Controller::JOYPAD_TYPE::NON;

	// 現在の接続しているパッドの数のみ更新(1番から)
	for (int num = static_cast<int>(KEYPAD_NO::PAD1); num < GetJoypadNum(); num++) {

		pads_.at(num)->Update();
		// 優先順に動いているか
		orderOfPriority_.emplace_back(pads_.at(num)->GetAnyone());
		// 最も優先的に動いているもの
		if (orderOfPriority_.back() && mostPriority_ == KEYPAD_NO::NON) {
			mostPriority_ = static_cast<KEYPAD_NO>(num);
			mostPriorityNo_ = pads_.at(num)->GetJPadType();
		}
	}

	// キーマウ更新
	keyboard_->Update();
	mouse_->Update();
	orderOfPriority_.emplace_back(keyboard_->GetAnyone() || mouse_->GetAnyone());

	// 最も優先的に動いているもの
	if (mostPriority_ == KEYPAD_NO::NON) {
		if (orderOfPriority_.back()) {
			mostPriority_ = KEYPAD_NO::KEY;
			mostPriorityNo_ = Controller::JOYPAD_TYPE::NON;
		}
		// どれも動いてないときは直近のものを取るようにする
		// 直近がないならPAD1
		else {
			if (tmp == KEYPAD_NO::NON) {
				orderOfPriority_.at(static_cast<int>(KEYPAD_NO::PAD1)) = true;
				mostPriority_ = KEYPAD_NO::PAD1;
				mostPriorityNo_ = pads_.at(static_cast<int>(KEYPAD_NO::PAD1))->GetJPadType();
				if (mostPriorityNo_ == Controller::JOYPAD_TYPE::NON) {
					mostPriorityNo_ = Controller::JOYPAD_TYPE::XBOX_360;
				}
			}
			else {
				mostPriority_ = tmp;
				mostPriorityNo_ = tmpNo;
			}
		}
	}
}

// リソースの破棄
void InputManager::Release(void){

	// 解放
	keyboard_->Release();
	mouse_->Release();
	for (auto p : pads_) {

		p->Release();
		delete p;
	}
	delete keyboard_;
	delete mouse_;
	pads_.clear();
}

std::map<InputManager::KEYPAD_NO, VECTOR>  InputManager::GetDirectionXZAKeyL(void)
{
	std::map<KEYPAD_NO, VECTOR>  res;

	VECTOR nowPos = Utility::VECTOR_ZERO;

	// WASDをアナログに変換
	if (keyboard_->GetKey(KEY_INPUT_W).keyNew) {
		nowPos.z += 1.0f;
	}
	if (keyboard_->GetKey(KEY_INPUT_S).keyNew) {
		nowPos.z -= 1.0f;
	}
	if (keyboard_->GetKey(KEY_INPUT_D).keyNew) {
		nowPos.x += 1.0f;
	}
	if (keyboard_->GetKey(KEY_INPUT_A).keyNew) {
		nowPos.x -= 1.0f;
	}
	if (!VectorUtility::EqualsVZero(nowPos)) {
		nowPos = VNorm(nowPos);
	}
	res[KEYPAD_NO::KEY] = nowPos;

	// 現在の接続しているパッドの数のみ取得(1番から)
	for (int num = static_cast<int>(KEYPAD_NO::PAD1); num < GetJoypadNum(); num++) {

		nowPos = pads_.at(num)->GetDirectionXZAKeyL();
		res[static_cast<KEYPAD_NO>(num)] = nowPos;
	}
	return res;
}

std::map<InputManager::KEYPAD_NO, VECTOR>  InputManager::GetDirectionXZAKeyR(void)
{
	std::map<KEYPAD_NO, VECTOR>  res;
	
	//　中心からの差分をとる
	VECTOR nowPos = mouse_->GetMousePos();
	VECTOR diffPos = VGet(nowPos.x - Application::SCREEN_SIZE_X_HALF, 0.0f, nowPos.y - Application::SCREEN_SIZE_Y_HALF);
	res[KEYPAD_NO::KEY] = VScale(diffPos, Mouse::MOUSE_SENSI);

	// 現在の接続しているパッドの数のみ取得(1番から)
	for (int num = static_cast<int>(KEYPAD_NO::PAD1); num < GetJoypadNum(); num++) {

		nowPos = pads_.at(num)->GetDirectionXZAKeyR();
		res[static_cast<KEYPAD_NO>(num)] = VScale(nowPos, Controller::CameraSensi);
	}
	return res;
}

InputBase::Info InputManager::GetKey(COMMAND com, KEYPAD_NO no)
{
	std::vector<InputBase::Info> infos;
	if (no == KEYPAD_NO::KEY) {
		if (keyCommand_.contains(com)) {
			for (int i : keyCommand_.at(com)) {

				infos.emplace_back(keyboard_->GetKey(i));
			}
		}
		if (mouseCommand_.contains(com)) {
			for (int i : mouseCommand_.at(com)) {

				infos.emplace_back(mouse_->GetKey(i));
			}
		}
	}
	else {
		for (auto i : padCommand_.at(com)) {

			infos.emplace_back(pads_.at(static_cast<int>(no))->GetKey(i));
		}
	}
	return GetKeyInfo(infos);
}

InputBase::Info InputManager::GetPriorityKey(COMMAND com)
{
	return GetKey(com, mostPriority_);
}

std::vector<InputBase::Info> InputManager::GetPriorityKey(COMMAND com, int num)
{
	std::vector<InputBase::Info> temp;
	for (int i = 0; i < static_cast<int>(KEYPAD_NO::MAX); i++) {
		if (orderOfPriority_.at(i)) {
			switch (i)
			{
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
			case static_cast<int>(KEYPAD_NO::KEY):
				temp.emplace_back(GetKey(com, KEYPAD_NO::KEY));
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

bool InputManager::GetPriorityAnyoneTrg(void)
{
	switch (mostPriority_)
	{
	case KEYPAD_NO::PAD1:
		return pads_.at(static_cast<int>(KEYPAD_NO::PAD1))->GetAnyoneTrg();
		break;
	case KEYPAD_NO::PAD2:
		return pads_.at(static_cast<int>(KEYPAD_NO::PAD2))->GetAnyoneTrg();
		break;
	case KEYPAD_NO::PAD3:
		return pads_.at(static_cast<int>(KEYPAD_NO::PAD3))->GetAnyoneTrg();
		break;
	case KEYPAD_NO::PAD4:
		return pads_.at(static_cast<int>(KEYPAD_NO::PAD4))->GetAnyoneTrg();
		break;
	case KEYPAD_NO::KEY:
		return keyboard_->GetAnyoneTrg() || mouse_->GetAnyoneTrg();
		break;
	default:
		return false;
		break;
	}
}

bool InputManager::GetPriorityAnyoneTrgDown(void)
{
	switch (mostPriority_)
	{
	case KEYPAD_NO::PAD1:
		return pads_.at(static_cast<int>(KEYPAD_NO::PAD1))->GetAnyoneTrgDown();
		break;
	case KEYPAD_NO::PAD2:
		return pads_.at(static_cast<int>(KEYPAD_NO::PAD2))->GetAnyoneTrgDown();
		break;
	case KEYPAD_NO::PAD3:
		return pads_.at(static_cast<int>(KEYPAD_NO::PAD3))->GetAnyoneTrgDown();
		break;
	case KEYPAD_NO::PAD4:
		return pads_.at(static_cast<int>(KEYPAD_NO::PAD4))->GetAnyoneTrgDown();
		break;
	case KEYPAD_NO::KEY:
		return keyboard_->GetAnyoneTrgDown() || mouse_->GetAnyoneTrgDown();
		break;
	default:
		return false;
		break;
	}
}

InputManager::InputManager(void)
{
}

InputManager::~InputManager(void)
{
}

void InputManager::CommandLoad(void)
{
	// ダッシュキー
	keyCommand_[COMMAND::RUN].emplace_back(KEY_INPUT_LSHIFT);
	padCommand_[COMMAND::RUN].emplace_back(PAD_INPUT_R);
	
	// コンボキー
	mouseCommand_[COMMAND::COMBO].emplace_back(MOUSE_INPUT_RIGHT);
	padCommand_[COMMAND::COMBO].emplace_back(XINPUT_BUTTON_Y);

	// 攻撃キー
	mouseCommand_[COMMAND::ATTACK].emplace_back(MOUSE_INPUT_LEFT);
	padCommand_[COMMAND::ATTACK].emplace_back(XINPUT_BUTTON_B);

	// 回避キー
	keyCommand_[COMMAND::DODGE].emplace_back(KEY_INPUT_SPACE);
	padCommand_[COMMAND::DODGE].emplace_back(XINPUT_BUTTON_A);

	// ロックオンキー
	mouseCommand_[COMMAND::LOCK_ON].emplace_back(MOUSE_INPUT_MIDDLE);
	keyCommand_[COMMAND::LOCK_ON].emplace_back(KEY_INPUT_RCONTROL);
	keyCommand_[COMMAND::LOCK_ON].emplace_back(KEY_INPUT_TAB);
	padCommand_[COMMAND::LOCK_ON].emplace_back(PAD_INPUT_L);

	// 使うキー
	keyCommand_[COMMAND::USE].emplace_back(KEY_INPUT_F);
	padCommand_[COMMAND::USE].emplace_back(XINPUT_BUTTON_X);

	// 左キー
	keyCommand_[COMMAND::LEFT].emplace_back(KEY_INPUT_Q);
	keyCommand_[COMMAND::LEFT].emplace_back(KEY_INPUT_LEFT);
	padCommand_[COMMAND::LEFT].emplace_back(XINPUT_BUTTON_DPAD_LEFT);

	// 右キー
	keyCommand_[COMMAND::RIGHT].emplace_back(KEY_INPUT_E);
	keyCommand_[COMMAND::RIGHT].emplace_back(KEY_INPUT_RIGHT);
	padCommand_[COMMAND::RIGHT].emplace_back(XINPUT_BUTTON_DPAD_RIGHT);

	// 上キー
	keyCommand_[COMMAND::UP].emplace_back(KEY_INPUT_W);
	keyCommand_[COMMAND::UP].emplace_back(KEY_INPUT_UP);
	padCommand_[COMMAND::UP].emplace_back(XINPUT_BUTTON_DPAD_UP);

	// 下キー
	keyCommand_[COMMAND::DOWN].emplace_back(KEY_INPUT_S);
	keyCommand_[COMMAND::DOWN].emplace_back(KEY_INPUT_DOWN);
	padCommand_[COMMAND::DOWN].emplace_back(XINPUT_BUTTON_DPAD_DOWN);

	// ポーズキー
	keyCommand_[COMMAND::PAUSE].emplace_back(KEY_INPUT_ESCAPE);
	padCommand_[COMMAND::PAUSE].emplace_back(XINPUT_BUTTON_BACK);
	padCommand_[COMMAND::PAUSE].emplace_back(XINPUT_BUTTON_START);

	// 決定キー
	mouseCommand_[COMMAND::DECIDE].emplace_back(MOUSE_INPUT_LEFT);
	keyCommand_[COMMAND::DECIDE].emplace_back(KEY_INPUT_SPACE);
	keyCommand_[COMMAND::DECIDE].emplace_back(KEY_INPUT_RETURN);
	padCommand_[COMMAND::DECIDE].emplace_back(XINPUT_BUTTON_A);

	// キャンセルキー
	mouseCommand_[COMMAND::CANCEL].emplace_back(MOUSE_INPUT_RIGHT);
	padCommand_[COMMAND::CANCEL].emplace_back(XINPUT_BUTTON_B);
}

InputBase::Info InputManager::GetKeyInfo(std::vector<InputBase::Info> infos)
{
	InputBase::Info res;

	for (InputBase::Info info : infos) {

		res.keyNew = info.keyNew == true ? true : res.keyNew;
		res.keyOld = info.keyOld == true ? true : res.keyOld;
		res.keyTrgDown = info.keyTrgDown == true ? true : res.keyTrgDown;
		res.keyTrgUp = info.keyTrgUp == true ? true : res.keyTrgUp;
	}
	return res;
}
