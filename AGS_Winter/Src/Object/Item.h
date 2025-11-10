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

<<<<<<< HEAD
=======
	TYPE GetType(void) { return type_; }
	bool IsUse(void) { return use_; }
	void SetUsing(bool use) { using_ = use; }

>>>>>>> dc1217fbb6e215f6d79dfc738caff453d3bc6f4c
private:

	int itemImg_[static_cast<int>(TYPE::MAX)];

	int itemNum_[static_cast<int>(TYPE::MAX)];

<<<<<<< HEAD
	int itemNo_;
=======
	TYPE type_;
	bool use_;
	bool using_;

	//デバッグ
	int prevKey_;
	int nowKey_;
>>>>>>> dc1217fbb6e215f6d79dfc738caff453d3bc6f4c
};