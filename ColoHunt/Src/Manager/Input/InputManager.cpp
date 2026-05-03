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
	keyMou_->Add(KEY_INPUT_RSHIFT);
	keyMou_->Add(MOUSE_INPUT_RIGHT);
	keyMou_->Add(MOUSE_INPUT_LEFT);
	keyMou_->Add(KEY_INPUT_SPACE);
	keyMou_->Add(MOUSE_INPUT_MIDDLE);
	keyMou_->Add(KEY_INPUT_F);
	keyMou_->Add(KEY_INPUT_Q);
	keyMou_->Add(KEY_INPUT_LEFT);
	keyMou_->Add(KEY_INPUT_E);
	keyMou_->Add(KEY_INPUT_RIGHT);
	keyMou_->Add(KEY_INPUT_W);
	keyMou_->Add(KEY_INPUT_UP);
	keyMou_->Add(KEY_INPUT_S);
	keyMou_->Add(KEY_INPUT_DOWN);

	// パッド取得]
	pads_ = new Controller();
	pads_->Init();
}

// 更新
void InputManager::Update(void){
	
	// キーマウ更新
	keyMou_->Update();

	// 現在の接続しているパッドの数のみ更新(1番から)
	for (int num = static_cast<int>(KEYPAD_NO::PAD1); num < GetJoypadNum(); num++) {

		pads_->Update(num);
	}
}

// リソースの破棄
void InputManager::Release(void){

	// 解放
	keyMou_->Destroy();
	pads_->Destroy();
	delete keyMou_;
	delete pads_;
}

std::map<InputManager::KEYPAD_NO, VECTOR>  InputManager::GetDirectionXZAKeyL(void)
{
	std::map<KEYPAD_NO, VECTOR>  res;

	VECTOR nowPos;

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
	for (int num = static_cast<int>(KEYPAD_NO::PAD1); num < GetJoypadNum(); num++) {

		nowPos = pads_->GetDirectionXZAKey(pads_->GetJPadState(num).AKeyLX, pads_->GetJPadState(num).AKeyLY);
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
	for (int num = static_cast<int>(KEYPAD_NO::PAD1); num < GetJoypadNum(); num++) {

		nowPos = pads_->GetDirectionXZAKey(pads_->GetJPadState(num).AKeyRX, pads_->GetJPadState(num).AKeyRY);
		res[static_cast<KEYPAD_NO>(num)] = nowPos;
	}
	return res;
}

KeyMouse::Info InputManager::GetKeyMouse(COMMAND com)
{
	return KeyMouse::Info();
}

Controller::JOYPAD_IN_STATE InputManager::GetKeyController(COMMAND com)
{
	return Controller::JOYPAD_IN_STATE();
}

InputManager::InputManager(void)
{
}

InputManager::~InputManager(void)
{
}

void InputManager::CommondLoad(void)
{
<<<<<<< HEAD

=======
	// ダッシュキー
	keyCommand_.emplace(COMMAND::RUN, KEY_INPUT_LSHIFT);
	padCommand_.emplace(COMMAND::RUN, Controller::JOYPAD_BTN::R);
	
	// 攻撃キー
	keyCommand_.emplace(COMMAND::ATTACK, MOUSE_INPUT_RIGHT);
	padCommand_.emplace(COMMAND::ATTACK, Controller::JOYPAD_BTN::LEFT);

	// コンボキー
	keyCommand_.emplace(COMMAND::COMBO, MOUSE_INPUT_LEFT);
	padCommand_.emplace(COMMAND::COMBO, Controller::JOYPAD_BTN::TOP);

	// 回避キー
	keyCommand_.emplace(COMMAND::COMBO, KEY_INPUT_SPACE);
	padCommand_.emplace(COMMAND::COMBO, Controller::JOYPAD_BTN::DOWN);

	// ロックオンキー
	keyCommand_.emplace(COMMAND::COMBO, MOUSE_INPUT_MIDDLE);
	padCommand_.emplace(COMMAND::COMBO, Controller::JOYPAD_BTN::L);

	// 使うキー
	keyCommand_.emplace(COMMAND::COMBO, KEY_INPUT_F);
	padCommand_.emplace(COMMAND::COMBO, Controller::JOYPAD_BTN::LEFT);

	// 左キー
	keyCommand_.emplace(COMMAND::COMBO, KEY_INPUT_Q);
	keyCommand_.emplace(COMMAND::COMBO, KEY_INPUT_LEFT);
	padCommand_.emplace(COMMAND::COMBO, Controller::JOYPAD_BTN::LEFT_DPAD);

	// 右キー
	keyCommand_.emplace(COMMAND::COMBO, KEY_INPUT_E);
	keyCommand_.emplace(COMMAND::COMBO, KEY_INPUT_RIGHT);
	padCommand_.emplace(COMMAND::COMBO, Controller::JOYPAD_BTN::RIGHT_DPAD);

	// 上キー
	keyCommand_.emplace(COMMAND::COMBO, KEY_INPUT_W);
	keyCommand_.emplace(COMMAND::COMBO, KEY_INPUT_UP);
	padCommand_.emplace(COMMAND::COMBO, Controller::JOYPAD_BTN::LEFT_DPAD);

	// 下キー
	keyCommand_.emplace(COMMAND::COMBO, KEY_INPUT_S);
	keyCommand_.emplace(COMMAND::COMBO, KEY_INPUT_DOWN);
	padCommand_.emplace(COMMAND::COMBO, Controller::JOYPAD_BTN::RIGHT_DPAD);

	// ポーズキー
	keyCommand_.emplace(COMMAND::COMBO, KEY_INPUT_ESCAPE);
	padCommand_.emplace(COMMAND::COMBO, Controller::JOYPAD_BTN::SELECT);
	padCommand_.emplace(COMMAND::COMBO, Controller::JOYPAD_BTN::START);
>>>>>>> e21b7bcb84e8595c45fabe20210778bf7a280d36
}
