#include <fstream>
#include "Parameter.h"
#include "../../Application.h"
#include "../../Utility/Utility.h"


Parameter* Parameter::instance_ = nullptr;

void Parameter::LoadParameter(void)
{
	// ファイルの読込
	std::ifstream ifs = std::ifstream(Application::PATH_CSV + "Parameter.csv");

	if (!ifs) {

		// エラーが発生
		return;
	}
	// ファイルを１行ずつ読み込む
	std::string line;
	// 1行の文字情報
	std::vector<std::string> strSplit;

	while (getline(ifs, line)) {

		// １行をカンマ区切りで分割
		strSplit = Utility::Split(line, ',');

		if (strSplit.size() == 4) {

			vectorData_.emplace(strSplit.at(0), VGet(std::stof(strSplit.at(1).c_str()), std::stof(strSplit.at(2).c_str()), std::stof(strSplit.at(3).c_str())));
		}
		else if (strSplit.at(1).contains("f")) {

			floatData_.emplace(strSplit.at(0), std::stof(strSplit.at(1)));
		}
		else {

			intData_.emplace(strSplit.at(0), std::stoi(strSplit.at(1)));
		}
	}
	ifs.close();
}

Parameter::Parameter()
{
}

Parameter::~Parameter()
{
}
