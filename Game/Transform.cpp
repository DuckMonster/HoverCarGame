#include "stdafx.h"
#include "Transform.h"
#include <glm/gtx/matrix_decompose.hpp>

using namespace glm;

size_t CTransform::sm_LocalUpdates = 0;
size_t CTransform::sm_WorldUpdates = 0;

/**	Constructor
*******************************************************************************/
CTransform::CTransform( CActor* actor ) :
	m_Actor( actor )
{
}

/**	Set Parent
*******************************************************************************/
void CTransform::SetParent( CTransform* parent )
{
	if (parent == m_Parent)
		return;

	// Unparent from current
	if (m_Parent)
	{
		transform_iterator foundIt = find( m_Parent->m_Children.begin( ), m_Parent->m_Children.end( ), this );

		if (foundIt != m_Parent->m_Children.end( ))
			m_Parent->m_Children.erase( foundIt );

		m_Parent = nullptr;
	}

	m_Parent = parent;

	// Add as child
	if (m_Parent)
	{
		m_Parent->m_Children.push_back( this );
	}
}

/**	Set Position
*******************************************************************************/
void CTransform::SetPosition( const glm::vec3& position )
{
	vec3 localPosition = position;

	if (GetParent( ))
		localPosition = (vec3)(GetParent( )->GetInverseMatrix( ) * vec4( position, 1.f ));

	SetLocalPosition( localPosition );
}

/**	Set Scale
*******************************************************************************/
void CTransform::SetScale( const glm::vec3& scale )
{
}

/**	Set Rotation
*******************************************************************************/
void CTransform::SetRotation( const glm::quat& rotation )
{
	quat localRotation = rotation;

	if (GetParent( ))
		localRotation = inverse( GetParent( )->GetRotation( ) ) * localRotation;

	SetLocalRotation( localRotation );
}

/**	Look At
*******************************************************************************/
void CTransform::LookAt( const glm::vec3& point, const glm::vec3& up )
{
	mat4 target( 1.f );

	vec3 vecforward = normalize( point - GetPosition( ) );
	vec3 vecright = normalize( cross( vecforward, up ) );
	vec3 vecup = cross( vecright, vecforward );

	target[0] = vec4( vecforward, 0.f );
	target[1] = vec4( vecup, 0.f );
	target[2] = vec4( vecright, 0.f );

	SetRotation( quat( target ) );
}

/**	Get Matrix
*******************************************************************************/
const glm::mat4& CTransform::GetMatrix( )
{
	Clean( );
	return m_Matrix;
}

/**	Get Local Matrix
*******************************************************************************/
const glm::mat4 & CTransform::GetLocalMatrix( )
{
	Clean( );
	return m_LocalMatrix;
}

/**	Get Inverse Matrix
*******************************************************************************/
const glm::mat4& CTransform::GetInverseMatrix( )
{
	Clean( );
	return m_InvMatrix;
}

/**	Get Inverse Local Matrix
*******************************************************************************/
const glm::mat4& CTransform::GetInverseLocalMatrix( )
{
	Clean( );
	return m_InvLocalMatrix;
}

/**	Set Dirty
*******************************************************************************/
void CTransform::SetDirty( bool local )
{
	m_LocalDirty = m_LocalDirty || local;
	m_WorldDirty = true;

	for (size_t i = 0; i < m_Children.size( ); i++)
		m_Children[i]->SetDirty( );
}

/**	Clean
*******************************************************************************/
void CTransform::Clean( )
{
	//--------------------------------------------------- Local Matrix
	if (m_LocalDirty)
	{
		mat4 scaleMat = scale( mat4( 1.f ), m_Scale );

		m_LocalMatrix = (mat4)m_Rotation * scaleMat;
		m_LocalMatrix[3] = vec4( m_Position, 1.f );

		m_InvLocalMatrix = inverse( m_LocalMatrix );

		sm_LocalUpdates++;
	}

	//--------------------------------------------------- World Matrix
	if (m_WorldDirty)
	{
		if (GetParent( ))
		{
			m_Matrix = GetParent( )->GetMatrix( ) * m_LocalMatrix;
			m_InvMatrix = inverse( m_Matrix );

			m_WorldPosition = (vec3)m_Matrix[3];
			m_WorldRotation = GetParent( )->GetRotation( ) * m_Rotation;
			m_WorldScale = GetParent( )->GetScale( ) * m_Scale;
		}
		else
		{
			m_Matrix = m_LocalMatrix;
			m_InvMatrix = m_InvLocalMatrix;

			m_WorldPosition = m_Position;
			m_WorldScale = m_Scale;
			m_WorldRotation = m_Rotation;
		}

		sm_WorldUpdates++;
	}

	m_LocalDirty = m_WorldDirty = false;
}
