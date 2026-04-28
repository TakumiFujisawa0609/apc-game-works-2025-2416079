#pragma once
#include <map>
#include <vector>
#include <Dxlib.h>

class KeyMouse
{
public:

	// キー情報
	struct Info
	{
		int key;			// キーID
		bool keyOld;		// 1フレーム前の押下状態
		bool keyNew;		// 現フレームの押下状態
		bool keyTrgDown;	// 現フレームでボタンが押されたか
		bool keyTrgUp;		// 現フレームでボタンが離されたか
	};

	// インスタンスを明示的に生成
	static void CreateInstance(void);
	// インスタンスの取得
	static KeyMouse& GetInstance(void);
	
	// コンストラクタ
	KeyMouse(void);
	// デストラクタ
	~KeyMouse(void) = default;

	// 初期化
	virtual void Init(void);
	// 更新
	virtual void Update(void);
	// リソースの破棄
	virtual void Destroy(void);

	// 判定を行うキーを追加
	void Add(int key);

	// 判定を行うキーをクリア
	void Clear(void) { infos_.clear(); }

	// キーの状態を取得
	KeyMouse::Info GetKey(int key) const { return infos_.at(key); }

	// マウス座標の取得
	VECTOR GetMousePos(void) const { return mousePos_; }

private:

	// キー情報
	std::map<int, KeyMouse::Info> infos_;
	KeyMouse::Info infoEmpty_;

	// マウスカーソルの位置
	VECTOR mousePos_;
	
	// マウスボタンの入力状態
	int mouseInput_;

	// 配列の中からキー情報を取得する
	const KeyMouse::Info& Find(int key) const;
};