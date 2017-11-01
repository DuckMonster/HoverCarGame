#pragma once
#include <random>

class CRandom
{
public:
	static float Value();
	static float Value( float max );
	static float Value( float min, float max );

private:
	static std::random_device s_Device;
};