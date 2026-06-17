#pragma once


class Item
{
public:

	enum class TYPE {

		NON = -1,
		HP,
		HP_MAX,
		STAMINA,
		MAX,
	};

	// コンストラクタ
	Item(void);
	// デストラクタ
	~Item(void);

	//ロード
	void InitLoad(void);
	//その他の初期化
	void Init(void);
	//更新
	void Update(void);
	//描画
	void Draw(void) const;
	//解放
	void Release(void) const;

	//何の種類か
	TYPE GetUseType(void) { return useType_; }
	
	//使用状況取得
	void Use(void) { use_ = true; }
	
	//アイテムの数
	int GetItemNum(void) { return useNum_; }

private:

	// アイテムの個数
	static constexpr int ITEM_NUM[static_cast<int>(TYPE::MAX)] = {

		10,
		2,
		5
	};

	// アイテムのボックスの色
	static constexpr unsigned int BOX_COLOR = 0x222255;
	static constexpr unsigned int LINE_COLOR = 0xeeee33;
	static constexpr unsigned int FONT_COLOR = 0xffffff;

	// アイテムボックスの相対座標
	static constexpr int LEFT_POS = -40;
	static constexpr int RIGHT_POS = 40;
	static constexpr int UP_POS = LEFT_POS;
	static constexpr int DOWN_POS = RIGHT_POS;
	static constexpr int LINE_WIDTH = 4;
	static constexpr float CIRCLE_POS = 40.0f;
	static constexpr float CIRCLE_RAD = 15.0f;
	static constexpr float CIRCLE_DIV_NUM = 32;
	static constexpr int FONT_WIDTH = RIGHT_POS;
	static constexpr int FONT_HEIGHT = 30;

	//アイテムのイメージハンドル
	int itemImg_[static_cast<int>(TYPE::MAX)];

	//種類ごとの数
	int itemNum_[static_cast<int>(TYPE::MAX)];

	//種類
	TYPE type_;
	TYPE useType_;
	
	//使用状況
	bool use_;
	int useNum_;

	//デバッグ
	int prevKey_;
	int nowKey_;
};