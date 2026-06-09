#pragma once
#include <DxLib.h>
#include <vector>
#include <string>


class Utility
{
public:

	static constexpr VECTOR VECTOR_ZERO = { 0.0f, 0.0f, 0.0f };
	static constexpr VECTOR VECTOR_ONE = { 1.0f, 1.0f, 1.0f };
	static constexpr float ONE_CIRCLE = 2.0f * DX_PI_F;

	static constexpr int MAX = 255;
	static constexpr int HALF = MAX / 2;

	// ‰ñ“]Ž²
	static constexpr VECTOR AXIS_X = { 1.0f, 0.0f, 0.0f };
	static constexpr VECTOR AXIS_Y = { 0.0f, 1.0f, 0.0f };
	static constexpr VECTOR AXIS_Z = { 0.0f, 0.0f, 1.0f };

	// •ûŒü
	static constexpr VECTOR DIR_F = { 0.0f, 0.0f, 1.0f };
	static constexpr VECTOR DIR_B = { 0.0f, 0.0f, -1.0f };
	static constexpr VECTOR DIR_R = { 1.0f, 0.0f, 0.0f };
	static constexpr VECTOR DIR_L = { -1.0f, 0.0f, 0.0f };
	static constexpr VECTOR DIR_U = { 0.0f, 1.0f, 0.0f };
	static constexpr VECTOR DIR_D = { 0.0f, -1.0f, 0.0f };

	// ŽlŽÌŒÜ“ü
	static int Round(float v);

	// •¶Žš—ñ‚Ì•ªŠ„
	static std::vector <std::string> Split(std::string& line, char delimiter);
};