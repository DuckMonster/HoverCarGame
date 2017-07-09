#pragma once
#include "Input.h"

struct SUpdateData
{
	float Delta;
	const CInput& Input;
};

struct SRenderData
{
	float Delta;
	const glm::mat4& CameraMatrix;
};