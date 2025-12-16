#include "SoundTable.h"
#include "../../Application.h"

// サウンドID から サウンドパスを取得
// システム用
namespace SoundTable_System 
{
	static const std::unordered_map<SoundID, std::string> Table = 
	{
	};
}

// タイトル画面用
namespace SoundTable_Title
{
	static const std::unordered_map<SoundID, std::string> Table =
	{
		{ SoundID::BGM_TITLE, (Application::PATH_SOUND + "Title.mp3").c_str() },
	};
}

// ゲーム画面用
namespace SoundTable_Game 
{
	static const std::unordered_map<SoundID, std::string> Table = 
	{
		{ SoundID::BGM_BATTLE, (Application::PATH_SOUND + "Battle.mp3").c_str() },
		{ SoundID::SE_WALK, (Application::PATH_SOUND + "Walk.mp3").c_str() },
		{ SoundID::SE_RUN,	(Application::PATH_SOUND + "Run.mp3").c_str() },
		{ SoundID::SE_ATTACK, (Application::PATH_SOUND + "Slash.mp3").c_str() },
		{ SoundID::SE_HEAVY_DAMAGE,	(Application::PATH_SOUND + "HeavyDamage.mp3").c_str() },
		{ SoundID::SE_LIGHT_DAMAGE, (Application::PATH_SOUND + "LightDamage.mp3").c_str() },
		{ SoundID::SE_DODGE, (Application::PATH_SOUND + "GreatDodge.mp3").c_str() },
		{ SoundID::SE_HEAL, (Application::PATH_SOUND + "Heal.mp3").c_str() },
	};
}


// リザルト画面用
namespace SoundTable_Result
{
	static const std::unordered_map<SoundID, std::string> Table =
	{
		{ SoundID::BGM_CLEAR, (Application::PATH_SOUND + "Clear.mp3").c_str() },
		{ SoundID::BGM_GAMEOVER, (Application::PATH_SOUND + "Failed.mp3").c_str() },
	};
}