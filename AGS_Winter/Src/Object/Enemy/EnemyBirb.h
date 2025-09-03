#pragma once
#include "EnemyBase.h"


class EnemyBirb : public EnemyBase 
{
public:

	EnemyBirb();
	~EnemyBirb();

protected:

	void SetParam(void) override;

	void ChangeStandby(void) override;
	void ChangeHitReact(void) override;

	void Move(void) override;
};