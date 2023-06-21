#pragma once

class IUpdateListener
{
public:
	virtual void Update(float deltaTime) = 0;
};