#pragma once
#include <map>
#include <vector>
#include <Dxlib.h>
#include "InputBase.h"

class KeyMouse : public InputBase
{
public:

	// コンストラクタ
	KeyMouse(int sensi);
	// デストラクタ
	~KeyMouse(void) = default;

	// 初期化
	void Init(void) override;
	// 更新
	void Update(void) override;
	// リソースの破棄
	void Release(void) override;

	// 判定を行うキーをクリア
	void Clear(void) { infos_.clear(); }

	// マウス座標の取得
	VECTOR GetMousePos(void) const { return mousePos_; }

private:

	// マウスカーソルの位置
	VECTOR mousePos_;
	
	// マウスボタンの入力状態
	int mouseInput_;

	// マウスの入力取得
	bool CheckMouse(int key);
};