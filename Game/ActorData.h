#pragma once
#include "Input.h"

enum class UpdatePhase
{
	Pre,
	Main,
	Post
};

enum class RenderPhase
{
	Deferred,
	Shadow,
	Forward,
	Post
};

struct SUpdateInfo
{
#if !defined(GAME_SERVER)
	SUpdateInfo( const Input::CInput& input ) : Input( input ) {}
#endif

	float Delta = 0.f;
	UpdatePhase Phase = UpdatePhase::Main;

#if !defined(GAME_SERVER)
	const Input::CInput& Input;
#endif
};

#if !defined(GAME_SERVER)
struct SRenderInfo
{
	glm::mat4 CameraMatrix;
	bool DebugDraw = false;
	RenderPhase Phase = RenderPhase::Deferred;
};
#endif

typedef void( *ActorScript )(class CActor* actor, const SUpdateInfo& info);