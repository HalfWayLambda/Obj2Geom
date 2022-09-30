#include <vector>
#include <stdint.h>
#include "Geom.h"
#include "OBJ.h"
#include "VertexData.h"

namespace geom {
	VertexData::VertexData(const std::vector<Vector3>& vertices)
	{
		m_vertices.reserve(vertices.size());
		for (unsigned int i{ 0 }; i < vertices.size(); i++)
		{
			m_vertices.push_back({ vertices.at(i).x, vertices.at(i).y, vertices.at(i).z });
		}
		m_curSize = VERTEX_DATA_OFFSET + vertices.size() * sizeof(WideVector3);
	}

	void VertexData::write(smp::file& file) const
	{
		file.write(m_vertices.size() + 1, VERTICES_INDEX_OFFSET,
			UNKNOWN0_OR_1, ORIGIN_INDEX, VERTEX_DATA_OFFSET
		);
		file.fill(8);

		// the first Vector3 is the origin, most often at 0,0,0
		writeWideVector3(file, { 0.0f, 0.0f, 0.0f });
		for (const auto& vector : m_vertices)
		{
			writeWideVector3(file, vector);
		}
	}
}