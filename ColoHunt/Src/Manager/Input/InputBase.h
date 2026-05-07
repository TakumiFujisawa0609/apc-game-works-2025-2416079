#pragma once
#include <map>
#include <vector>
#include <Dxlib.h>

class InputBase
{
public:

	// キーボード,ゲームコントローラー認識番号
	// DxLib定数、DX_INPUT_PAD1等に対応
	enum class KEYPAD_NO
	{
		NON = -1,

		KEY,				// キー入力
		MOUSE,				// マウス入力
		PAD1,				// パッド１入力
		PAD2,				// パッド２入力
		PAD3,				// パッド３入力
		PAD4,				// パッド４入力

		MAX,
	};

	// キー情報
	struct Info
	{
		int key;			// キーID
		KEYPAD_NO type;		// 情報のタイプ
		bool keyOld;		// 1フレーム前の押下状態
		bool keyNew;		// 現フレームの押下状態
		bool keyTrgDown;	// 現フレームでボタンが押されたか
		bool keyTrgUp;		// 現フレームでボタンが離されたか
	};

	// コンストラクタ
	InputBase(void);
	// デストラクタ
	~InputBase(void) = default;

	// 初期化
	virtual void Init(void) {};
	// 更新
	virtual void Update(void) {};
	void Update(std::pair<const int, InputBase::Info>& infos);
	// リソースの破棄
	virtual void Release(void) {};

	// 判定を行うキーを追加
	void Add(int key, KEYPAD_NO type);

	// 判定を行うキーをクリア
	void Clear(void) { infos_.clear(); }

	// キーの状態を取得
	InputBase::Info GetKey(int key) const { return infos_.at(key); }

	// 今何か押されているか
	bool GetIsAnyoneDown(void) { return isAnyoneDown_; }

	// 何か押されているか
	bool GetAnyoneKey(void) { return anyoneKey_; }

	// 何か動いているか
	bool GetAnyone(void) { return anyone_; }

protected:

	// キー情報
	std::map<int, InputBase::Info> infos_;

	// 操作判定
	bool isAnyoneDown_;
	bool anyone_;
	bool anyoneKey_;
};