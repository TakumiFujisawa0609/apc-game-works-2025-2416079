#pragma once
#include <Dxlib.h>
#include <map>
#include <list>
#include <string>
#include "Controller.h"
#include "KeyMouse.h"


class Controller;
class KeyMouse;

class InputManager{

public:

	// コマンド
	enum class COMMAND {

		NON = -1,

		RUN,
		ATTACK,
		COMBO,
		DODGE,
		LOCK_ON,
		USE,
		LEFT,
		RIGHT,
		UP,
		DOWN,
		PAUSE,
		DECIDE,
		CANCEL,

		MAX,
	};

	// マウス感度
	static constexpr int MOUSE_SENSITIVITY = 30;

	// インスタンスを明示的に生成
	static void CreateInstance(void);
	// インスタンスの取得
	static InputManager& GetInstance(void);

	// 初期化
	void Init(void);
	// 更新
	void Update(void);
	// リソースの破棄
	void Release(void);

	// 方向を取得
	std::map<InputManager::KEYPAD_NO, VECTOR> GetDirectionXZAKeyL(void);
	std::map<InputManager::KEYPAD_NO, VECTOR>  GetDirectionXZAKeyR(void);

	// 特定のボタンを取得
	InputBase::Info GetKey(COMMAND com, KEYPAD_NO no);

	// 優先順に一つ取得
	InputBase::Info GetPriorityKey(COMMAND com);
	// 指定数(優先機種を0から)
	std::vector<InputBase::Info> GetPriorityKey(COMMAND com, int num);

	// 最優先されたタイプを取得
	Controller::JOYPAD_TYPE GetMostPriorityType(void) { return mostPriorityType_; }

private:

	InputManager(void);
	~InputManager(void);

	// コピー・ムーブ操作を禁止
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	InputManager(InputManager&&) = delete;
	InputManager& operator=(InputManager&&) = delete;

	// コマンドロード
	void CommondLoad(void);

	//インスタンス
	static InputManager* inst_;

	// パッド情報
	std::vector<Controller*> pads_;

	// キーマウ情報
	KeyMouse* keyMou_;

	// キーマウコマンド
	std::map<InputManager::COMMAND, int> keyCommand_;
	//パッドコマンド
	std::map<InputManager::COMMAND, int> padCommand_;

	// 優先
	std::vector<bool> orderOfPriority_;
	KEYPAD_NO mostPriority_;
	Controller::JOYPAD_TYPE mostPriorityType_;
};