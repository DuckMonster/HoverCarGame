// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <chrono>
#include <gl/glew.h>

#if !defined(GAME_SERVER)
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <Common/debug.h>

#include "Math.h"

// TODO: reference additional headers your program requires here
