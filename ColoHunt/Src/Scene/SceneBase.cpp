#include <DxLib.h>
#include <fstream>
#include "SceneBase.h"
#include "../Application.h"
#include "../Utility/Utility.h"


SceneBase::SceneBase(void)
{
}

SceneBase::~SceneBase(void)
{
}

void SceneBase::Init(void)
{
}

void SceneBase::Update(void)
{
}

void SceneBase::Draw(void)
{
}

void SceneBase::Release(void)
{
}

std::vector<int> SceneBase::LoadTime(void)
{
	// ファイルの読込
	std::ifstream ifs = std::ifstream(Application::PATH_CSV + "Time.csv");

	if (!ifs) {

		// エラーが発生
		std::vector<int> error{};
		return error;
	}
	// ファイルを１行ずつ読み込む
	std::string line;
	// 1行の文字情報
	std::vector<std::string> strSplit;
	// それぞれのタイム
	std::vector<int> time;

	getline(ifs, line);
		
	// １行をカンマ区切りで分割
	strSplit = Utility::Split(line, ',');

	for (int i = 0; i < strSplit.size(); i++) {

		time.push_back(atoi(strSplit.at(i).c_str()));
	}
	return time;

	ifs.close();
}
