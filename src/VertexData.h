#pragma once
#include <vector>
#include <stdint.h>
#include "Geom.h"

namespace geom {
	class VertexData : ISection
	{
	private:
		static constexpr uint32_t VERTICES_INDEX_OFFSET{ 1 };
		static constexpr uint32_t UNKNOWN0_OR_1{ 1 };
		static constexpr uint32_t ORIGIN_INDEX{ 0 };
		static constexpr uint64_t VERTEX_DATA_OFFSET{ 32 };
		static constexpr uint32_t FMDL_VERTEX_DATA_OFFSET{ 0 };
		unsigned int m_curSize{};
		std::vector<WideVector3> m_vertices{};

	public:
		VertexData(const std::vector<Vector3>&);
		void write(smp::file&) const;
		unsigned int getSize() const { return m_curSize + sizeof(WideVector3); }
	};
}