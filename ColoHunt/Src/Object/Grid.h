#pragma once
class Grid
{
public:
	static constexpr float LEN = 5000.0f;			// 線の長さ
	static constexpr float HLEN = LEN / 2.0f;			// 線の長さの半分

	static constexpr float TERM = 100.0f;			// 線の間隔
	
	static const int NUM = static_cast<int>(LEN / TERM);			// 線の数
	static const int HNUM = NUM / 2;			// 線の数の半分

	static constexpr float SIZE = 20.0f;			// 球の大きさ
	static constexpr int ROUGH = 16;			// 球の大きさ
	
	// コンストラクタ
	Grid(void);
	// デストラクタ
	~Grid(void);
	
	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);
};