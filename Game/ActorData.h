#pragma once
#include "Input.h"

struct SUpdateInfo
{
	float Delta = 0.f;
	const Input::CInput& Input;
};

struct SRenderInfo
{
	glm::mat4 CameraMatrix;
	GLuint ShaderOverride = -1;
	bool UseShaderOverride = false;
	bool DebugDraw = false;
};

typedef void( *ActorScript )(class CActor* actor, const SUpdateInfo& info);