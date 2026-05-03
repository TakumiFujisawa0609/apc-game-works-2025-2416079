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

	// キーボード,ゲームコントローラー認識番号
	// DxLib定数、DX_INPUT_PAD1等に対応
	enum class KEYPAD_NO
		{
		NON = -1,

		KEY,				// キー入力
		PAD1,				// パッド１入力
		PAD2,				// パッド２入力
		PAD3,				// パッド３入力
		PAD4,				// パッド４入力

		MAX,
	};

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

		MAX,
	};

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

<<<<<<< HEAD
=======
	// ボタンを取得
	KeyMouse::Info GetKeyMouse(COMMAND com);
	Controller::JOYPAD_IN_STATE GetKeyController(COMMAND com);


>>>>>>> e21b7bcb84e8595c45fabe20210778bf7a280d36
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
	Controller* pads_;

	// キーマウ情報
	KeyMouse* keyMou_;

	// キーマウコマンド
	std::map<InputManager::COMMAND, int> keyCommand_;
	//パッドコマンド
	std::map<InputManager::COMMAND, Controller::JOYPAD_BTN> padCommand_;
};