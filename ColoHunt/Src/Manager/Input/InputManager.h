#pragma once
#include <Dxlib.h>
#include <map>
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

	// ボタンをすべて取得
	template<class T>
	std::map<InputManager::KEYPAD_NO, T> GetKeyAll(COMMAND com) {
		
		std::map<InputManager::KEYPAD_NO, T> res;

		T value = keyMou_->GetKey(keyCommand_.at(com));
		res.emplace(KEYPAD_NO::KEY, value);

		for (int i = static_cast<int>(KEYPAD_NO::PAD1); i < static_cast<int>(KEYPAD_NO::MAX); i++) {
			value = pads_->GetJPadState(i);
			res.emplace(static_cast<KEYPAD_NO>(i), value);
		}
		return res;
	}

	/// <summary>
	/// ボタンを優先順に指定数取得
	/// </summary>
	/// <typeparam name="T">返り値</typeparam>
	/// <param name="com">コマンド</param>
	/// <param name="num">ほしい数(1以上)</param>
	/// <returns></returns>
	template<class T>
	std::map<InputManager::KEYPAD_NO, T> GetKeyAll(COMMAND com, int num) {

		std::map<InputManager::KEYPAD_NO, T> res;
		T value;

		if (num == 0) return res;

		if (orderOfPriority_.size() == 0) {

			value = pads_->GetJPadState(static_cast<int>(KEYPAD_NO::PAD1));
			res.emplace(static_cast<KEYPAD_NO>(static_cast<int>(KEYPAD_NO::PAD1)), value);
			return res;
		}

		for (int i = static_cast<int>(KEYPAD_NO::PAD1); i < static_cast<int>(KEYPAD_NO::MAX); i++) {
			if (orderOfPriority_.at(i - 1)) {

				value = pads_->GetJPadState(i);
				res.emplace(static_cast<KEYPAD_NO>(i), value);
			}
		}

		if (orderOfPriority_.at(static_cast<int>(KEYPAD_NO::MAX) - 1)) {

			T value = keyMou_->GetKey(keyCommand_.at(com));
			res.emplace(KEYPAD_NO::KEY, value);
		}
		return res;
	}

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
	// パッドコマンド
	std::map<InputManager::COMMAND, int> padCommand_;

	// 最終打ち込みを優先順に入れる
	std::vector<bool> orderOfPriority_;
};