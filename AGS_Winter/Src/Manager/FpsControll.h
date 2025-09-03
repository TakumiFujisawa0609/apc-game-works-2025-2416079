#pragma once
#include <list>


class FpsControll final {

public:
	
	FpsControll(int targetfps);

	//待ち時間
	void Wait(void);
	//描画をスキップするかどうか
	bool SkipDrawScene(void);

private:

	//現在の時間の確認
	void RegisterTime(int nowtime);
	//待つ時間
	bool CheckSkipDrawScreen(int* waittime) const;

	//FPSが正しいかどうか
	bool TargetFpsIsValid(void) const;
	const int TargetFpsValue;

	//1フレーム当たりの時間
	const int OneFrameTime;
	
	//リストに2秒のフレームの時間を記録し最大サイズを決める
	const int ListMax;
	//1フレームごとにかかった時間を記録するリスト
	std::list<int> time_list;

	//コマ落ち実装フラグ
	bool SkipDrawScreen;
};