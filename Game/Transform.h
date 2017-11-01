#pragma once
#include "Component.h"
#include "Serialize.h"

class CTransform
{
public:
	static size_t	sm_LocalUpdates;
	static size_t	sm_WorldUpdates;

private:
	typedef std::vector<CTransform*>::iterator transform_iterator;

public:
	CTransform( class CActor* actor );

	void SetParent( CTransform* parent );
	CTransform* GetParent() const { return m_Parent; }

	size_t NumChildren() const { return m_Children.size(); }
	CTransform* GetChild( size_t index ) const { return m_Children[index]; }

	CActor* GetActor() { return m_Actor; }

	// Transform stuff
	const glm::vec3& GetWorldPosition() { Clean(); return m_WorldPosition; }
	const glm::vec3& GetWorldScale() { Clean();  return m_WorldScale; }
	const glm::quat& GetWorldRotation() { Clean(); return m_WorldRotation; }
	const glm::vec3& GetWorldRotationEuler() { Clean(); return Math::QuatToEuler(m_WorldRotation); }

	const glm::vec3& GetPosition() const { return m_Position; }
	const glm::vec3& GetScale() const { return m_Scale; }
	const glm::quat& GetRotation() const { return m_Rotation; }
	const glm::vec3 GetRotationEuler() const { return Math::QuatToEuler( m_Rotation ); }

	const glm::vec3 GetForward() { Clean(); return glm::normalize( ( glm::vec3 )m_Matrix[0] ); }
	const glm::vec3 GetUp() { Clean(); return glm::normalize( ( glm::vec3 )m_Matrix[1] ); }
	const glm::vec3 GetRight() { Clean(); return glm::normalize( ( glm::vec3 )m_Matrix[2] ); }

	void SetTransform( CTransform* other );
	void SetTransform( const glm::mat4& transform );
	void SetPositionAndRotation( const glm::vec3& position, const glm::quat& rotation );
	void SetPositionAndRotation( const glm::vec3& position, const glm::vec3& euler );

	void SetWorldPosition( const glm::vec3& position );
	void SetWorldScale( const glm::vec3& scale );
	void SetWorldRotation( const glm::quat& rotation );
	void SetWorldRotation( const glm::vec3& euler ) { SetWorldRotation( Math::EulerToQuat( euler ) ); }

	void AddWorldPosition( const glm::vec3& delta ) { SetWorldPosition( GetWorldPosition() + delta ); }
	void AddWorldScale( const glm::vec3& delta ) { SetWorldScale( GetWorldScale() + delta ); }
	void AddWorldRotation( const glm::quat& delta ) { SetWorldRotation( delta * GetWorldRotation() ); }
	void AddWorldRotation( const glm::vec3& delta ) { AddWorldRotation( Math::EulerToQuat( delta ) ); }

	void SetPosition( const glm::vec3& position ) { m_Position = position; SetDirty( true ); }
	void SetScale( const glm::vec3& scale ) { m_Scale = scale; SetDirty( true ); }
	void SetRotation( const glm::quat& rotation ) { m_Rotation = rotation; SetDirty( true ); }
	void SetRotation( const glm::vec3& euler ) { SetRotation( Math::EulerToQuat( euler ) ); }

	void AddPosition( const glm::vec3& delta ) { SetPosition( m_Position + delta ); }
	void AddScale( const glm::vec3& delta ) { SetScale( m_Scale + delta ); }
	void AddRotation( const glm::quat& delta ) { SetRotation( m_Rotation * delta ); }
	void AddRotation( const glm::vec3& delta ) { AddRotation( Math::EulerToQuat( delta ) ); }

	void LookAt( const glm::vec3& point, const glm::vec3& up = glm::vec3( 0.f, 1.f, 0.f ) );

	const glm::mat4& GetMatrix();
	const glm::mat4& GetLocalMatrix();
	const glm::mat4& GetInverseMatrix();
	const glm::mat4& GetInverseLocalMatrix();

	void Serialize( CSerializer& Serializer );

private:
	void SetDirty( bool local = false );
	void Clean();

private:
	class CActor* const m_Actor;

	CTransform*	m_Parent = nullptr;
	std::vector<CTransform*> m_Children;

	bool		m_LocalDirty = true;
	bool		m_WorldDirty = true;

	glm::mat4	m_Matrix = glm::mat4( 1.f );
	glm::mat4	m_InvMatrix = glm::mat4( 1.f );

	glm::mat4	m_LocalMatrix = glm::mat4( 1.f );
	glm::mat4	m_InvLocalMatrix = glm::mat4( 1.f );

	glm::vec3	m_Position = glm::vec3( 0.f );
	glm::vec3	m_Scale = glm::vec3( 1.f );
	glm::quat	m_Rotation = glm::quat();

	glm::vec3	m_WorldPosition = glm::vec3( 0.f );
	glm::vec3	m_WorldScale = glm::vec3( 1.f );
	glm::quat	m_WorldRotation = glm::quat();
};