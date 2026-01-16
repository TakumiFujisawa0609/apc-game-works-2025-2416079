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
	void Draw(void);
	//解放
	void Release(void);

	//何の種類か
	TYPE GetUseType(void) { return useType_; }
	
	//使用状況取得
	void Use(void) { use_ = using_ = true; }
	bool IsUse(void) { return use_; }
	void SetUsing(bool use) { using_ = use; }

private:

	//アイテムのイメージハンドル
	int itemImg_[static_cast<int>(TYPE::MAX)];

	//種類ごとの数
	int itemNum_[static_cast<int>(TYPE::MAX)];

	//種類
	TYPE type_;
	TYPE useType_;
	
	//使用状況
	bool prevUse_;
	bool use_;
	bool using_;

	//デバッグ
	int prevKey_;
	int nowKey_;
};