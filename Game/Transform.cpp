#include "stdafx.h"
#include "Transform.h"
#include "Actor.h"
#include "Scene.h"
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
	if ( parent == m_Parent )
		return;

	// Unparent from current
	if ( m_Parent )
	{
		transform_iterator foundIt = find( m_Parent->m_Children.begin(), m_Parent->m_Children.end(), this );

		if ( foundIt != m_Parent->m_Children.end() )
			m_Parent->m_Children.erase( foundIt );

		m_Parent = nullptr;
	}

	m_Parent = parent;

	if ( m_Parent )
	{
		// Make sure name is unique
		CName uniqueName = CName::MakeUnique( GetActor()->GetName(), parent->GetActor() );
		if ( uniqueName != GetActor()->GetName() )
		{
			Debug_Log( "Unique name: %s -> %s", GetActor()->GetName().c_str(), uniqueName.c_str() );
			GetActor()->SetName( uniqueName );
		}

		// Add as child
		m_Parent->m_Children.push_back( this );
	}

	GetActor()->SetPathDirty();
}

/**	Set Transform
*******************************************************************************/
void CTransform::SetTransform( CTransform * other )
{
	m_Position = other->m_Position;
	m_Rotation = other->m_Rotation;
	m_Scale = other->m_Scale;

	SetDirty( true );
}

/**	Set Transform
*******************************************************************************/
void CTransform::SetTransform( const glm::mat4& transform )
{
	// Get scale factor
	vec3 scale( length( vec3( transform[0] ) ), length( vec3( transform[1] ) ), length( vec3( transform[2] ) ) );

	// Rotation
	mat3 rotMatrix( transform );
	rotMatrix[0] /= scale[0];
	rotMatrix[1] /= scale[1];
	rotMatrix[2] /= scale[2];

	quat rotation( rotMatrix );

	// Translation
	vec3 translation( transform[3] );

	SetPosition( translation );
	SetRotation( rotation );
	SetScale( scale );
}

/**	Set Position And Rotation (quat)
*******************************************************************************/
void CTransform::SetPositionAndRotation( const vec3 & position, const glm::quat & rotation )
{
	m_Position = position;
	m_Rotation = rotation;

	SetDirty( true );
}

/**	Set Position And Rotation (euler)
*******************************************************************************/
void CTransform::SetPositionAndRotation( const vec3 & position, const glm::vec3 & euler )
{
	m_Position = position;
	m_Rotation = Math::EulerToQuat( euler );

	SetDirty( true );
}

/**	Set Position
*******************************************************************************/
void CTransform::SetWorldPosition( const vec3& position )
{
	vec3 localPosition = position;

	if ( GetParent() )
		localPosition = (vec3)( GetParent()->GetInverseMatrix() * vec4( position, 1.f ) );

	SetPosition( localPosition );
}

/**	Set Scale
*******************************************************************************/
void CTransform::SetWorldScale( const vec3& scale )
{
}

/**	Set Rotation
*******************************************************************************/
void CTransform::SetWorldRotation( const quat& rotation )
{
	quat localRotation = rotation;

	if ( GetParent() )
		localRotation = inverse( GetParent()->GetWorldRotation() ) * localRotation;

	SetRotation( localRotation );
}

/**	Look At
*******************************************************************************/
void CTransform::LookAt( const vec3& point, const glm::vec3& up )
{
	mat4 target( 1.f );

	vec3 vecforward = normalize( point - GetWorldPosition() );
	vec3 vecright = normalize( cross( vecforward, up ) );
	vec3 vecup = cross( vecright, vecforward );

	target[0] = vec4( vecforward, 0.f );
	target[1] = vec4( vecup, 0.f );
	target[2] = vec4( vecright, 0.f );

	SetWorldRotation( quat( target ) );
}

/**	Get Matrix
*******************************************************************************/
const mat4& CTransform::GetMatrix()
{
	Clean();
	return m_Matrix;
}

/**	Get Local Matrix
*******************************************************************************/
const mat4 & CTransform::GetLocalMatrix()
{
	Clean();
	return m_LocalMatrix;
}

/**	Get Inverse Matrix
*******************************************************************************/
const mat4& CTransform::GetInverseMatrix()
{
	Clean();
	return m_InvMatrix;
}

/**	Get Inverse Local Matrix
*******************************************************************************/
const mat4& CTransform::GetInverseLocalMatrix()
{
	Clean();
	return m_InvLocalMatrix;
}

/**	Serialize
*******************************************************************************/
void CTransform::Serialize( CSerializer& Serializer )
{
	Serializer.Serialize( m_Position );
	Serializer.Serialize( m_Rotation );
	Serializer.Serialize( m_Scale );

	if ( Serializer.IsWriting() )
		SetDirty( true );
}

/**	Set Dirty
*******************************************************************************/
void CTransform::SetDirty( bool local )
{
	m_LocalDirty = m_LocalDirty || local;
	m_WorldDirty = true;

	for ( size_t i = 0; i < m_Children.size(); i++ )
		m_Children[i]->SetDirty();
}

/**	Clean
*******************************************************************************/
void CTransform::Clean()
{
	//--------------------------------------------------- Local Matrix
	if ( m_LocalDirty )
	{
		mat4 scaleMat = scale( mat4( 1.f ), m_Scale );

		m_LocalMatrix = (mat4)m_Rotation * scaleMat;
		m_LocalMatrix[3] = vec4( m_Position, 1.f );

		m_InvLocalMatrix = inverse( m_LocalMatrix );

		sm_LocalUpdates++;
	}

	//--------------------------------------------------- World Matrix
	if ( m_WorldDirty )
	{
		if ( GetParent() )
		{
			m_Matrix = GetParent()->GetMatrix() * m_LocalMatrix;
			m_InvMatrix = inverse( m_Matrix );

			m_WorldPosition = (vec3)m_Matrix[3];
			m_WorldRotation = GetParent()->GetWorldRotation() * m_Rotation;
			m_WorldScale = GetParent()->GetWorldScale() * m_Scale;
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
