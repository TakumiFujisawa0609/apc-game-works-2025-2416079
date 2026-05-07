#pragma once
#include <map>
#include <vector>
#include <Dxlib.h>


class InputBase
{
public:

	// 情報を得る形
	enum class TYPE
	{
		NON = -1,

		KEY,
		MOUSE,
		PAD,

		MAX
	};

	// キー情報
	struct Info
	{
		int key;			// キーID
		TYPE type;			// 情報を得る形
		bool keyOld;		// 1フレーム前の押下状態
		bool keyNew;		// 現フレームの押下状態
		bool keyTrgDown;	// 現フレームでボタンが押されたか
		bool keyTrgUp;		// 現フレームでボタンが離されたか
	};

	// ゲームコントローラータイプ
	// DxLib定数、DX_OTHER等に対応
	enum class JOYPAD_TYPE
	{
		NON = -1,

		OTHER,
		XBOX_360,
		XBOX_ONE,
		DUAL_SHOCK_3,
		DUAL_SHOCK_4,
		DUAL_SENSE,
		SWITCH_JOY_CON_L,
		SWITCH_JOY_CON_R,
		SWITCH_PRO_CTRL,

		MAX
	};

	// コンストラクタ
	InputBase(void);
	// デストラクタ
	~InputBase(void) = default;

	// 初期化
	virtual void Init(void) = 0;
	// 更新
	virtual void Update(void) = 0;
	void Update(std::pair<const int, Info> p);
	// リソースの破棄
	virtual void Release(void) = 0;

	// 判定を行うキーを追加
	void Add(int key, TYPE type);

	// 判定を行うキーをクリア
	void Clear(void) { infos_.clear(); }

	// キーの状態を取得
	Info GetKey(int key) const { return infos_.at(key); }

	// 何かしているか
	bool GetAnyone(void) { return anyone_; }

protected:

	// キー情報
	std::map<int, Info> infos_;

	// 現フレームで何か押されたか
	bool anyoneTrgDown_;
	// 何か押されているか
	bool anyoneTrg_;
	// 何かしているか
	bool anyone_;
};