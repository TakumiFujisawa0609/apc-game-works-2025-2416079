// FpsControll class
#include "FpsControll.h"
#include <DxLib.h>
#include <cstdlib>
#include <string>


FpsControll::FpsControll(const int targetfps) : TargetFpsValue(targetfps), OneFrameTime(static_cast<int>(1000.0 / targetfps)), ListMax(targetfps * 2), SkipDrawScreen(false){}

// 待ち時間
void FpsControll::Wait(void) {

	if (!TargetFpsIsValid()) {

		return;
	}
	//待ち時間を取得
	int waitTime = 0;

	if (CheckSkipDrawScreen(&waitTime)) {

		//取得した分待つ
		WaitTimer(waitTime);
		//現在の時刻を記録する
		RegisterTime(GetNowCount());
	}
	else {

		//コマ落ち処理をし記録する
		RegisterTime(time_list.back() + OneFrameTime);
		//描画を飛ばさせる
		SkipDrawScreen = true;
	}
}

/// <summary>
/// 描画をスキップするかどうか
/// </summary>
/// <returns>true : スキップする / false : ...しない</returns>
bool FpsControll::SkipDrawScene(void) {

	if (!TargetFpsIsValid()) {

		return false;
	}
	//フラグ立っていたらスキップする
	if (SkipDrawScreen) {

		SkipDrawScreen = false;
		return true;
	}
	return false;
}

/// <summary>
/// 現在の自国の確認
/// </summary>
/// <param name="nowtime">現在の時刻(ミリ秒)</param>
void FpsControll::RegisterTime(const int nowtime) {

	//時間を記録する
	time_list.push_back(nowtime);

	if (time_list.size() > ListMax) {

		//あふれたら削除する
		time_list.pop_front();
	}
}

/// <summary>
/// 待つ時間
/// </summary>
/// <param name="time">待つべき時間(ミリ秒)</param>
/// <returns>false : コマ落ち</returns>
bool FpsControll::CheckSkipDrawScreen(int* time) const {

	//時間の初期化
	(*time) = 0;

	//時刻リストが空ならwait時間は0秒
	if (time_list.empty()) {

		(*time) = 0;
		return true;
	}
	//実際にかかった時間
	int actuallyTime = GetNowCount() - time_list.back();
	//待つべき時間
 	int waitTime = OneFrameTime - actuallyTime;

	if (waitTime >= 0) {

		//待ち時間をそのまま返す
		(*time) = waitTime;
		return true;
	}
	else {

		if (std::abs(waitTime) < OneFrameTime) {

			//1フレーム以上遅れていなければ処理を行う
			return false;
		}
	}
	//どれも引っかからないなら0を返す
	(*time) = 0;
	return true;
}


bool FpsControll::TargetFpsIsValid(void) const {

	//マイナス値は入れない
	if (TargetFpsValue <= 0) {

		return false;
	}
	return true;
}