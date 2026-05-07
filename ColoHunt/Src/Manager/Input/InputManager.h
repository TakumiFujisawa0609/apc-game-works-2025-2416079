#pragma once
#include <Dxlib.h>
#include <map>
#include <list>
#include <string>
#include "Controller.h"
#include "Keyboard.h"
#include "Mouse.h"


class Controller;
class Keyboard;
class Mouse;

class InputManager{

public:

	// キーボード,ゲームコントローラー認識番号
	// DxLib定数、DX_INPUT_PAD1等に対応
	enum class KEYPAD_NO
		{
		NON = -1,

		PAD1,				// パッド１入力
		PAD2,				// パッド２入力
		PAD3,				// パッド３入力
		PAD4,				// パッド４入力
		KEY,				// キー入力
		MOUSE,				// マウス入力

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
		DECIDE,
		CANCEL,

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

	// 特定のボタンを取得
	InputBase::Info GetKey(COMMAND com, KEYPAD_NO no);

	// 優先順に一つ取得
	InputBase::Info GetPriorityKey(COMMAND com);
	// 指定数(優先機種を0から)
	std::vector<InputBase::Info> GetPriorityKey(COMMAND com, int num);

	// 最優先されたタイプを取得
	KEYPAD_NO GetMostPriority(void) { return mostPriority_; }
	Controller::JOYPAD_TYPE GetMostPriorityType(void) { return mostPriorityNo_; }

	// (今のフレームで)最優先の物が何か押しているか
	bool GetPriorityAnyoneTrg(void);
	// (今のフレームで)最優先の物が今何か押したか
	bool GetPriorityAnyoneTrgDown(void);

private:

	InputManager(void);
	~InputManager(void);

	// コピー・ムーブ操作を禁止
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	InputManager(InputManager&&) = delete;
	InputManager& operator=(InputManager&&) = delete;

	// コマンドロード
	void CommandLoad(void);

	// コマンドの複数の押下判定の合算
	InputBase::Info GetKeyInfo(std::vector<InputBase::Info> infos);

	//インスタンス
	static InputManager* inst_;

	// パッド情報
	std::vector<Controller*> pads_;

	// キーマウ情報
	Keyboard* keyboard_;
	Mouse* mouse_;

	// キーマウコマンド
	std::map<InputManager::COMMAND, std::vector<int>> keyCommand_;
	std::map<InputManager::COMMAND, std::vector<int>> mouseCommand_;
	//パッドコマンド
	std::map<InputManager::COMMAND, std::vector<int>> padCommand_;

	// 優先
	std::vector<bool> orderOfPriority_;
	KEYPAD_NO mostPriority_;
	Controller::JOYPAD_TYPE mostPriorityNo_;
};