#pragma once
#include "Component.h"
#include "Material.h"

class CLineRenderer : public CComponent
{
	struct Segment
	{
		glm::vec3 verts[2];
	};
	struct MetaSegment
	{
		glm::vec3 point;
		glm::vec3 tangent;
		float time = 1.f;
	};

public:
	CLineRenderer( CActor* actor ) : CComponent( actor ) { Register<CLineRenderer>(); }

#if !defined(GAME_SERVER)
	void OnInit() override;
	void OnBegin() override;
	void OnDestroy() override;
	void OnUpdate( const SUpdateInfo& info ) override;
	void OnRender( const SRenderInfo& info ) override;

	MetaSegment& LastSegment() { return m_MetaData[m_MetaData.size() - 1]; }
#endif

	float m_Width = 1.f;

private:
#if !defined(GAME_SERVER)
	void AddSegment( const glm::vec3& position );
	void UpdateSegments( const float delta );

	std::vector<Segment> m_SegmentData;
	std::vector<MetaSegment> m_MetaData;

	size_t m_LastSegmentCount = 0;

	CMaterial m_Material;
	GLuint m_VertexObject;
	GLuint m_VertexBuffer;

	glm::vec3 m_LastPosition;
	bool m_FirstSegment = true;
#endif
};
