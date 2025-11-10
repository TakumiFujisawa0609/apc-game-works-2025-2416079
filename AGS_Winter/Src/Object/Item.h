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

private:

	int itemImg_[static_cast<int>(TYPE::MAX)];

	int itemNum_[static_cast<int>(TYPE::MAX)];

	int itemNo_;
};