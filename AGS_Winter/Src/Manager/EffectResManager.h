#pragma once
#include <map>


class EffectResManager
{
public:

	// エフェクトの種類
	enum class TYPE
	{
		BLAST_GROUND,
		BLAST_HIT,
		BEAM,
		FIREWORKS,
	};

	// インスタンスの生成
	static void CreateInstance(void);
	// インスタンスの取得
	static EffectResManager& GetInstance(void);

	// 初期化
	void Init(void);
	// リソースの破棄
	void Destroy(void);
	// エフェクシアのリソースハンドルを取得
	int GetResourceId(TYPE type);

private:

	// 静的インスタンス
	static EffectResManager* instance_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	EffectResManager(void);

	// コピーコンストラクタも同様
	EffectResManager(const EffectResManager& instance) = default;
	// デストラクタも同様
	~EffectResManager(void) = default;

	// リソースのロード
	void Load(void);

	// エフェクシアのリソースハンドル
	std::map<TYPE, int> resourceIds_;
};