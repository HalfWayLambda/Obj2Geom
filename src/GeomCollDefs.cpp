#include <exception>
#include <stdint.h>
#include "Geom.h"
#include "CFileWrapper.h"
#include "GeomCollDefs.h"

namespace geom::colldef {
	void entryheader::write(smp::file& file) const
	{
		file.write(m_unusedHash0, static_cast<uint32_t>(0), m_unknownFlag);
		file.fill(12);
		file.write(m_nextSectionOffset);
		file.write(m_previousEntryOffset, m_nextEntryOffset);

		// 12 bytes padding
		file.fill(12);
	}

	void entrysecondaryheader::write(smp::file& file) const
	{
		file.write(UNUSED_HASH0);
		file.fill(4);
		file.write(static_cast<uint32_t>(m_geomentrytype));
		file.write(m_nextSectionOffset, m_remainingEntryDataSize);
		file.fill(28);
	}

	void entrysecondaryheader::updateRemainingEntryDataSize(uint32_t newSize)
	{
		m_remainingEntryDataSize = newSize;
	}

	void entrydatachunkdefinitionentry::write(smp::file& file) const
	{
		file.write(m_isFinalEntry);
		if (!m_isFinalEntry)
		{
			file.write(m_dataChunkCount, m_dataChunksTotalSize);
			file.write(m_bboxOrSubverticesOffset0, static_cast<uint16_t>(0));
			file.write(m_bboxOrSubverticesOffset1, static_cast<uint16_t>(0), m_verticesOffset);
			file.write(m_firstDataChunkOffset, m_nextSectionOffset, m_collisionFlags);
		}
		else
		{
			// padding
			file.fill(31);
		}
	}

	void entrydatachunkdefinitionentry::updateDataChunkCount(uint32_t newCount)
	{
		if (newCount > UINT8_MAX)
			throw std::overflow_error("OVERFLOW ERROR: Couldn't create geom file, data chunk count too large");
		m_dataChunkCount = static_cast<uint8_t>(newCount);
	}

	void entrydatachunkdefinitionentry::updateOffsetToFirstDataChunk(uint32_t newOffset)
	{
		if (newOffset > UINT16_MAX)
			throw std::overflow_error("OVERFLOW ERROR: Couldn't create geom file, offset too large");
		m_bboxOrSubverticesOffset1 = static_cast<uint16_t>(newOffset);
		m_firstDataChunkOffset = newOffset;
	}

	void entrydatachunkdefinitionentry::updateOffsetToVertices(uint32_t newOffset)
	{
		if (newOffset > UINT16_MAX)
			throw std::overflow_error("OVERFLOW ERROR: Couldn't create geom file, offset too large");
		m_bboxOrSubverticesOffset0 = static_cast<uint16_t>(newOffset);
		m_verticesOffset = newOffset;
	}

	void entrydatachunkdefinitionentry::updateDataChunksTotalDataSize(uint32_t newSize)
	{
		if (newSize > UINT16_MAX)
			throw std::overflow_error("OVERFLOW ERROR: Couldn't create geom file, too many data chunks");
		m_dataChunksTotalSize = static_cast<uint16_t>(newSize);
	}

	void entrymaterialssection::write(smp::file& file) const
	{
		file.fill(1);
		file.write(infoTriplet0, infoTriplet0, infoTriplet1);
		file.write(m_materialNameHash);
		file.fill(8);
	}
	void entrymaterialssection::writeDataChunkGap(smp::file& file) const
	{
		// padding
		file.fill(DATACHUNK_GAP);
	}

	void base_datachunk::write(smp::file& file) const
	{
		uint32_t combinedData{
			static_cast<uint32_t>(m_shapeHeader.m_shapeType) |
			(static_cast<uint32_t>(m_shapeHeader.m_shapeFlags) << 4) |
			m_shapeHeader.m_EntryCount << 24
		};
		file.write(combinedData);
		file.write(m_shapeHeader.m_nextBBDataChunkOffset);
		file.write(m_shapeHeader.m_unknown0, m_shapeHeader.m_nextIndexDataChunkOffset);
		file.write(static_cast<uint64_t>(m_shapeHeader.m_collisionTags));
		file.write(m_shapeHeader.m_strCode32Name);
		file.write(m_shapeHeader.m_vertexDataOffset);
	}

	void base_datachunk::updateVertexDataOffset(uint32_t newOffset)
	{
		m_shapeHeader.m_vertexDataOffset = newOffset;
	}

	datachunk_bb::datachunk_bb(const OBJ& obj, const COLLISION_TAGS& tags)
	{
		geom::GeoCollisionShapeHeader shapeHeader{
			geom::GEO_COLLISION_SHAPE_TYPE::BOUNDING_BOX,
			geom::GEO_COLLISION_SHAPE_FLAGS::UNKNOWN3,
			1, 0, 4, tags, 0
		};

		geom::WideVector3 center{
			(obj.getMaxCorner().x + obj.getMinCorner().x) / 2,
			(obj.getMaxCorner().y + obj.getMinCorner().y) / 2,
			(obj.getMaxCorner().z + obj.getMinCorner().z) / 2
		};

		geom::WideVector3 radii{
			(obj.getMaxCorner().x - obj.getMinCorner().x) / 2,
			(obj.getMaxCorner().y - obj.getMinCorner().y) / 2,
			(obj.getMaxCorner().z - obj.getMinCorner().z) / 2
		};
		radii.x += MIN_FACTOR;
		radii.y += MIN_FACTOR;
		radii.z += MIN_FACTOR;

		m_baseDataChunk = shapeHeader;
		m_boundingBoxRadii = radii;
		m_boundingBoxCenter = center;
	}

	void datachunk_bb::write(smp::file& file) const
	{
		m_baseDataChunk.write(file);
		writeWideVector3(file, m_boundingBoxRadii);
		writeWideVector3(file, m_boundingBoxCenter);
	}

	datachunk_quad::datachunk_quad(const OBJ& objFile, const COLLISION_TAGS& tags)
	{
		geom::GeoCollisionShapeHeader shapeHeader{
			geom::GEO_COLLISION_SHAPE_TYPE::QUAD,
			static_cast<geom::GEO_COLLISION_SHAPE_FLAGS>(0),
			objFile.getFaces().size(), 0, 0, tags, 0
		};
		m_baseDataChunk = shapeHeader;

		// 10 since that's the size of every index set
		m_curSize = objFile.getFaces().size() * sizeof(IndexSet);
		m_faces.reserve(objFile.getFaces().size());

		if (objFile.getFaces().size() > 255U)
			throw std::length_error{ "ERROR: OBJs with more than 255 faces aren't supported!" };
		for (const auto& faceSet : objFile.getFaces())
		{
			const auto& facelist{ faceSet.getFaceElements() };
			if (facelist.size() == 4) // if there's 4 faces
			{
				IndexSet indexset{ facelist[0] - 1U, facelist[1] - 1U,
					facelist[2] - 1U, facelist[3] - 1U };
				m_faces.push_back(indexset);
			}
			else if (facelist.size() == 3) // if it's a triangle
			{
				IndexSet indexset{ facelist[0] - 1U, facelist[1] - 1U,
					facelist[2] - 1U, facelist[0] - 1U }; // use the 1st index as the 4th one
				m_faces.push_back(indexset);
			}
			else
				throw std::logic_error{ "ERROR: Weird face with "
				+ std::to_string(facelist.size()) + " indices detected!"
			};
		}
		// the extra padding near the end of the faces needs to be taken into account too
		unsigned int extraSize{ 16 - (m_curSize % 16) };
		m_curSize += extraSize;
	}

	void datachunk_quad::write(smp::file& file) const
	{
		m_baseDataChunk.write(file);
		for (const auto& face : m_faces)
		{
			for (int i{ 0 }; i < 4; i++)
				file.write(face.offsets[i]);
			file.write(face.unknown);
		}
		long filePos{ file.ftell() };
		filePos = 16 - (filePos % 16);

		for (int i{ 0 }; i < filePos; i++)
			file.write(static_cast<uint8_t>(i + 49));
	}

	void datachunk_quad::updateVertexDataOffset(uint32_t newOffset)
	{
		m_baseDataChunk.updateVertexDataOffset(newOffset);
	}
}