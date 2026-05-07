#pragma once
#include <map>
#include <vector>
#include <Dxlib.h>
#include "InputBase.h"

class Keyboard : public InputBase
{
public:

	// コンストラクタ
	Keyboard(void);
	// デストラクタ
	~Keyboard(void) = default;

	// 初期化
	void Init(void) override;
	// 更新
	void Update(void) override;
	// リソースの破棄
	void Release(void) override;

	// 判定を行うキーをクリア
	void Clear(void) { infos_.clear(); }

private:
};