#include <DxLib.h>
#include "Grid.h"

Grid::Grid(void)
{
}

Grid::~Grid(void)
{
}

void Grid::Init(void)
{
}

void Grid::Update(void)
{
}

void Grid::Draw(void)
{
	for (int z = -HNUM; z < HNUM; z++) {
	
		VECTOR sPos = { -HLEN, 0.0f, static_cast<float>(z * TERM)};
		VECTOR ePos = { HLEN, 0.0f, static_cast<float>(z * TERM) };

		DrawLine3D(sPos, ePos, 0xff0000);
		DrawSphere3D(ePos, SIZE, ROUGH, 0xff0000, 0xffffff, true);
	}

	for (int x = -HNUM; x < HNUM; x++) {

		VECTOR sPos = { static_cast<float>(x * TERM), 0.0f, -HLEN };
		VECTOR ePos = { static_cast<float>(x * TERM), 0.0f, HLEN };

		DrawLine3D(sPos, ePos, 0x0000ff);
		DrawSphere3D(ePos, SIZE, ROUGH, 0x0000ff, 0xffffff, true);
	}
	VECTOR sPos = { 0.0f, 0.0f, 0.0f };
	VECTOR ePos = { 0.0f, LEN, 0.0f };

	DrawLine3D(sPos, ePos, 0x00ff00);
	DrawSphere3D(ePos, SIZE, ROUGH, 0x00ff00, 0xffffff, true);
}

void Grid::Release(void)
{
}
