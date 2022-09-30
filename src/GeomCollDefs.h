#pragma once
#include <stdint.h>

namespace geom::colldef {
	class entryheader : public ISection
	{
	private:
		uint32_t m_unusedHash0, m_unknownFlag, m_nextSectionOffset;
		int32_t m_previousEntryOffset;
		uint32_t m_nextEntryOffset;

	public:
		entryheader(uint32_t nextSectionOffset, int32_t previousEntryOffset,
			uint32_t nextEntryOffset)
			: m_unusedHash0{ 3561833074 }, m_unknownFlag{ 16 },
			m_nextSectionOffset{ nextSectionOffset },
			m_previousEntryOffset{ previousEntryOffset },
			m_nextEntryOffset{ nextEntryOffset }
		{}
		void write(smp::file&) const override;
		unsigned int getSize() const override { return 48; }
	};

	class entrysecondaryheader : public ISection
	{
	public:
		enum class GEOM_ENTRY_TYPE : uint32_t
		{
			GEOM_ENTRY_TYPE_0,
			GEOM_ENTRY_TYPE_6 = 6
		};

	private:
		static constexpr inline uint32_t UNUSED_HASH0{ 3197782967 };
		GEOM_ENTRY_TYPE m_geomentrytype;
		uint32_t m_nextSectionOffset;
		uint32_t m_remainingEntryDataSize{ 0 }; // has to be updated later

	public:
		entrysecondaryheader(const GEOM_ENTRY_TYPE& entryType, uint32_t nextSectionOffset)
			: m_geomentrytype{ entryType },
			m_nextSectionOffset{ nextSectionOffset }
		{}
		void write(smp::file&) const override;
		unsigned int getSize() const override { return 48; }
		void updateRemainingEntryDataSize(uint32_t newSize);
	};

	class entrydatachunkdefinitionentry : public ISection
	{
	private:
		bool m_isFinalEntry;
		uint8_t m_dataChunkCount{ 0 }; // is updated later
		uint16_t m_dataChunksTotalSize{ 1216 }; // same
		uint16_t m_bboxOrSubverticesOffset0{ 640 }; // same
		uint16_t m_bboxOrSubverticesOffset1{ 112 }; // same
		uint32_t m_verticesOffset{ 640 }; // same
		uint32_t m_firstDataChunkOffset{ 112 }; // same
		uint32_t m_nextSectionOffset{ 64 }; // it would be if this tool
										// made more than one definition entry
		uint64_t m_collisionFlags{ 36028797027626434 };

	public:
		entrydatachunkdefinitionentry(bool isFinalEntry = false)
			: m_isFinalEntry{ isFinalEntry }
		{}
		void write(smp::file& file) const override;
		unsigned int getSize() const override { return 32; }

		void updateDataChunkCount(uint32_t);
		void updateOffsetToFirstDataChunk(uint32_t);
		void updateOffsetToVertices(uint32_t );
		void updateDataChunksTotalDataSize(uint32_t);
	};

	class entrymaterialssection : public ISection
	{
	private:
		static constexpr inline uint8_t infoTriplet0{ 2 };
		static constexpr inline uint8_t infoTriplet1{ 1 };
		uint32_t m_materialNameHash;

	public:
		static constexpr inline uint32_t DATACHUNK_GAP{ 32 };

		entrymaterialssection(uint32_t materialNameHash)
			: m_materialNameHash{ materialNameHash }
		{}
		void write(smp::file& file) const override;
		void writeDataChunkGap(smp::file& file) const;
		unsigned int getSize() const override { return 16; } // okay for now
	};

	class base_datachunk : public ISection
	{
	private:
		GeoCollisionShapeHeader m_shapeHeader;

	public:
		base_datachunk() {}
		base_datachunk(const GeoCollisionShapeHeader& shapeHeader)
			: m_shapeHeader{ shapeHeader }
		{}
		unsigned int getSize() const override { return 32; }
		void write(smp::file&) const;

		void updateVertexDataOffset(uint32_t newOffset);
	};

	class datachunk_bb : public ISection
	{
	private:
		base_datachunk m_baseDataChunk;
		WideVector3 m_boundingBoxRadii;
		WideVector3 m_boundingBoxCenter;
		static constexpr inline float MIN_FACTOR{ 0.5f };

	public:
		datachunk_bb(const OBJ& obj);
		void write(smp::file&) const;
		unsigned int getSize() const { return m_baseDataChunk.getSize() + 32; }
	};

	class datachunk_quad : public ISection
	{
	private:
		base_datachunk m_baseDataChunk;
		struct IndexSet
		{
			uint16_t offsets[4]{};
			int16_t unknown{ -510 };
		};
		std::vector<IndexSet> m_faces;
		static constexpr inline int16_t unknownOffset{ -510 };
		unsigned int m_curSize;

	public:
		datachunk_quad(const OBJ& objFile);
		void write(smp::file&) const;
		unsigned int getSize() const { return m_baseDataChunk.getSize() + m_curSize; }

		void updateVertexDataOffset(uint32_t);
	};
}

using GEOM_ENTRY_TYPE = geom::colldef::entrysecondaryheader::GEOM_ENTRY_TYPE;