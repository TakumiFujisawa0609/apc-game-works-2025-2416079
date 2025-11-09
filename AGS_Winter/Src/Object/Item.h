#pragma once


class Item
{
public:

	enum class TYPE {

		HP,
		HP_MAX,
		STAMINA,
		MAX,
	};

	// コンストラクタ
	Item(void);
	// デストラクタ
	~Item(void);

	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	TYPE GetType(void) { return type_; }
	bool IsUse(void) { return use_; }
	void SetUsing(bool use) { using_ = use; }

private:

	int itemImg_[static_cast<int>(TYPE::MAX)];

	int itemNum_[static_cast<int>(TYPE::MAX)];

	TYPE type_;
	bool use_;
	bool using_;

	//デバッグ
	int prevKey_;
	int nowKey_;
};