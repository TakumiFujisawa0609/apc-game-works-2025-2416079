#pragma once
#include <map>
#include <string>
#include <DxLib.h>


class Parameter {

public:

	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new Parameter(); } }
	static Parameter* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

	// csvのロード
	void LoadParameter(void);

	// 保存した内容の呼び出し
	float GetFloatParameter(std::string name) { return floatData_.at(name); }
	int GetIntParameter(std::string name) { return intData_.at(name); }
	VECTOR GetVectorParameter(std::string name) { return vectorData_.at(name); }

private:

	// 静的インスタンス
	static Parameter* instance_;

	// ロードした内容の保存
	std::map<std::string, float> floatData_;
	std::map<std::string, int> intData_;
	std::map<std::string, VECTOR> vectorData_;

	Parameter();		// コンストラクタ
	~Parameter();	// デストラクタ

	// コピー・ムーブ操作を禁止
	Parameter(const Parameter&) = delete;
	Parameter& operator=(const Parameter&) = delete;
	Parameter(Parameter&&) = delete;
	Parameter& operator=(Parameter&&) = delete;
};