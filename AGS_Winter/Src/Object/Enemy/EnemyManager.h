#pragma once
#include <vector>


class EnemyBase;

class EnemyManager
{
public:

	// 敵の出現間隔
	static constexpr int SPAWN_INTERVAL = 225;

	static constexpr int SPAWN_MAX = 3;

	// コンストラクタ
	EnemyManager(void);
	// デストラクタ
	~EnemyManager(void);

	void Init(void);
	void Update(int wave);
	void Draw(void);
	void Release(void);

	// エネミーの取得
	const std::vector<EnemyBase*>& GetEnemys(void) const{ return enemys_; }

	void Reset(void);
	void SetDeadCnt(void) { deadCnt_++; }
	int GetDeadCnt(void) { return deadCnt_; }

private:

	// エネミー用のモデルハンドルID
	std::vector<int> enemyModelIds_;
	// エネミー
	std::vector<EnemyBase*> enemys_;

	// 出現間隔
	int cntSpawn_;
	int spawnNum_;
	int deadCnt_;
};