#pragma once
#include <string>
#include <unordered_map>

// サウンドID
enum class SoundID
{
	BGM_TITLE,
	BGM_BATTLE,
	BGM_CLEAR,
	BGM_GAMEOVER,
	SE_WALK,
	SE_RUN,
	SE_ATTACK,
	SE_DAMAGE,
	SE_HEAVY_DAMAGE,
	SE_LIGHT_DAMAGE,
	SE_DODGE,
	SE_HEAL,
	SE_WOLF_ATTACK,
	SE_WOLF_RUN,
	SE_DESIDE,
	SE_CANCEL,
	SE_PAUSE,
	SE_CURSOR
};

// 読み込むシーン
enum class LoadScene
{
	SYSTEM,		// システム
	TITLE,		// タイトル
	GAME,		// ゲーム
	RESULT,		// リザルト
};

// サウンド属性
struct SoundData
{
	std::string path;	// サウンドのパス
};

// サウンドテーブル（名称被りや拡張性の為にクラス化)
class SoundTable
{
public:
	// サウンドテーブルのマップ
	// サウンドID から サウンドパスを取得
	static const std::unordered_map<SoundID, SoundData> Table;
};
