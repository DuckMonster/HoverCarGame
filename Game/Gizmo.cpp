#include "stdafx.h"
#include "Gizmo.h"
#include "GLUtils.h"

using namespace glm;

namespace
{
	//--------------------------------------------------- Line Drawing
	const char* LINE_VERT_SRC = "\
#version 330 core\n\
uniform mat4 u_Matrix;\
const float lineVerts[] = float[] ( 0.0, 1.0 );\
void main() {\
	gl_Position = u_Matrix * vec4(lineVerts[gl_VertexID], 0.0, 0.0, 1.0);\
	gl_Position.z = -1.0;\
}";
	const char* LINE_FRAG_SRC = "\
#version 330 core\n\
uniform vec4 u_Color;\
out vec4 o_Color;\
void main() {\
	o_Color = u_Color;\
};";

	GLuint lineShader;
}

void CGizmo::InitGizmos( )
{
	// Line shader
	lineShader = GLUtils::CreateShaderSrc( LINE_VERT_SRC, LINE_FRAG_SRC );
}

void CGizmo::DestroyGizmos( )
{
	glDeleteShader( lineShader );
}

void CGizmo::DrawLine( const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float size, const glm::mat4& camera )
{
	mat4 matrix( 1.f );
	vec3 dif = end - start;

	matrix[0] = vec4( dif, 0.f );
	matrix[3] = vec4( start, 1.f );

	// Transform line matrix
	matrix = camera * matrix;

	glUseProgram( lineShader );
	GLuint u_Matrix = glGetUniformLocation( lineShader, "u_Matrix" ),
		u_Color = glGetUniformLocation( lineShader, "u_Color" );

	glUniformMatrix4fv( u_Matrix, 1, false, value_ptr( matrix ) );
	glUniform4fv( u_Color, 1, value_ptr( color ) );

	glLineWidth( size );
	glDrawArrays( GL_LINES, 0, 2 );
}