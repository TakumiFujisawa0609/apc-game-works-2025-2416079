#pragma once
#include "EnemyBase.h"


class EnemyBee : public EnemyBase
{
public:

	EnemyBee();
	~EnemyBee();

protected:

	float speedY_;

	void SetParam(void) override;

	void ChangeStandby(void) override;
	void ChangeHitReact(void) override;

	void Move(void) override;
};