#define STR(var) #var
#include <fstream>
#include <vector>
#include "InputManager.h"
#include "Controller.h"
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

	// パッド取得
	pads_ = new Controller();
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

InputManager::InputManager(void)
{
}

InputManager::~InputManager(void)
{
}

void InputManager::CommondLoad(void)
{

}
