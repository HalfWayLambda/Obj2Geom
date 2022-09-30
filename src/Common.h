#pragma once
#include <stdint.h>

namespace geom {
	struct WideVector3
	{
		float x, y, z, empty{ 0 };
	};

	struct COLLISION_TAGS
	{
		uint64_t UNKNOWN1 : 1;
		uint64_t RECOIL_FLOOR_MAYBE : 1;
		uint64_t UNKNOWN0 : 1;
		uint64_t CHARA_MAYBE : 1;

		uint64_t PLAYER : 1;
		uint64_t ENEMY : 1;
		uint64_t BULLET : 1;
		uint64_t MISSILE : 1;

		uint64_t BOMB : 1;
		uint64_t UNUSED5 : 1;
		uint64_t BLOOD : 1;
		uint64_t IK : 1;

		uint64_t STAIRWAY : 1;
		uint64_t STOP_EYE : 1;
		uint64_t CLIFF : 1;
		uint64_t UNUSED4 : 1;

		uint64_t UNUSED6 : 1;
		uint64_t DONT_FALL : 1;
		uint64_t CAMERA : 1;
		uint64_t UNUSED2 : 1;

		uint64_t UNUSED1 : 1;
		uint64_t UNUSED0 : 1;
		uint64_t CLIFF_FLOOR : 1;
		uint64_t BULLET_MARK : 1;

		uint64_t HEIGHT_LIMIT : 1;
		uint64_t DOUBLE_SLIDE : 1;
		uint64_t WATER_SURFACE : 1;
		uint64_t TARGET_BLOCK : 1;

		uint64_t DOG : 1;
		uint64_t NO_EFFECT : 1;
		uint64_t EVENT_PHYSICS : 1;
		uint64_t NO_WALL_MOVE : 1;

		uint16_t Padding0 : 16;

		uint64_t MISSILE2 : 1;
		uint64_t RIDE_ON_OUTER : 1;
		uint64_t FLAME : 1;
		uint64_t IGNORE_PHYSICS : 1;

		uint64_t CLIMB : 1;
		uint64_t HORSE : 1;
		uint64_t VEHICLE : 1;
		uint64_t MARKER : 1;

		uint64_t RIDE_ON : 1;
		uint64_t THROUGH_LINE_OF_FIRE : 1;
		uint64_t THROUGH_ITEM_CHECK : 1;
		uint64_t NO_CREEP : 1;

		uint64_t NO_FULTON : 1;
		uint64_t FULTON : 1;
		uint64_t ITEM : 1;
		uint64_t BOSS : 1;

		operator uint64_t() const {
			return { static_cast<uint64_t>(
				UNKNOWN1 |
				RECOIL_FLOOR_MAYBE << 1 |
				UNKNOWN0 << 2 |
				CHARA_MAYBE << 3 |

				PLAYER << 4 |
				ENEMY << 5 |
				BULLET << 6 |
				MISSILE << 7 |

				BOMB << 8 |
				UNUSED5 << 9 |
				BLOOD << 10 |
				IK << 11 |

				STAIRWAY << 12 |
				STOP_EYE << 13 |
				CLIFF << 14 |
				UNUSED4 << 15 |

				UNUSED6 << 32 |
				DONT_FALL << 33 |
				CAMERA << 34 |
				UNUSED2 << 35 |

				UNUSED1 << 36 |
				UNUSED0 << 37 |
				CLIFF_FLOOR << 38 |
				BULLET_MARK << 39 |

				HEIGHT_LIMIT << 40 |
				DOUBLE_SLIDE << 41 |
				WATER_SURFACE << 42 |
				TARGET_BLOCK << 43 |

				DOG << 44 |
				NO_EFFECT << 45 |
				EVENT_PHYSICS << 46 |
				NO_WALL_MOVE << 47 |

				MISSILE2 << 48 |
				RIDE_ON_OUTER << 49 |
				FLAME << 50 |
				IGNORE_PHYSICS << 51 |

				CLIMB << 52 |
				HORSE << 53 |
				VEHICLE << 54 |
				MARKER << 55 |

				RIDE_ON << 56 |
				THROUGH_LINE_OF_FIRE << 57 |
				THROUGH_ITEM_CHECK << 58 |
				NO_CREEP << 59 |

				NO_FULTON << 60 |
				FULTON << 61 |
				ITEM << 62 |
				BOSS << 63
			) };
		}
	};

	enum class GEO_COLLISION_SHAPE_TYPE : uint32_t
	{
		UNKNOWN0 = 0,   	// Entry size: 0x30
		UNKNOWN1 = 1,		// Entry size: 0x30
		QUAD = 2,		    // Entry size: 0xA
		BOX_TRAP = 3,		// Entry size: 0x60
		BOUNDING_BOX = 4,	// Entry size: 0x20
		UNKNOWN5 = 5,		// Entry size: 0x80
		UNKNOWN6 = 6,		// Entry size: 0
		UNKNOWN7 = 7,		// Entry size: 0
		GEO_PATH = 8,		// Entry size: 0x8
		UNKNOWN9 = 6,		// Entry size: 0
		UNKNOWN10 = 7,		// Entry size: 0
		AREA_PATH = 11,		// Entry size: 0x10
	};

	enum class GEO_COLLISION_SHAPE_FLAGS : uint32_t
	{
		UNKNOWN6 = 0x1,				// Found in TRAP.
		UNKNOWN4 = 0x4,				// Found in GPFP.
		NO_PAYLOAD = 0x20,			// mgsvtpp.exe!141bdc498 // info & 0x200
		UNKNOWN1 = 0x200,				// mgsvtpp.exe!141bdc863, 141bd8041 // info >> 0xd, info & 0x2000
		USE_FMDL_VERTEX_DATA = 0x800,	// mgsvtpp.exe!141bd7e30 // info & 0x8000
		UNKNOWN3 = 0x2000,			// mgsvtpp.exe!141bdc7a7 // info >> 0x11
		UNKNOWN2 = 0x8000,			// mgsvtpp.exe!141bdc830 // info >> 0x13
		UNKNOWN5 = 0x80000,			// Found in GPFP and TRAP.
	};

	class GeoCollisionShapeHeader
	{
	// maybe make these private and organize them in some other way in the future?
	public:
		GEO_COLLISION_SHAPE_TYPE m_shapeType : 4;
		GEO_COLLISION_SHAPE_FLAGS m_shapeFlags : 20;
		uint32_t m_EntryCount : 8;
		uint32_t m_nextBBDataChunkOffset;
		uint32_t m_unknown0, m_nextIndexDataChunkOffset;
		COLLISION_TAGS m_collisionTags;
		uint32_t m_strCode32Name, m_vertexDataOffset;

	public:
		GeoCollisionShapeHeader()
		{
			m_shapeType = static_cast<GEO_COLLISION_SHAPE_TYPE>(0);
			m_shapeFlags = static_cast<GEO_COLLISION_SHAPE_FLAGS>(0);
			m_EntryCount = 0;
			m_nextBBDataChunkOffset = 0;
			m_unknown0 = 0;
			m_nextIndexDataChunkOffset = 0;
			COLLISION_TAGS tags{};
			m_collisionTags = tags;
			m_strCode32Name = 0;
			m_vertexDataOffset = 0;
		}

		GeoCollisionShapeHeader(const GEO_COLLISION_SHAPE_TYPE& shapeType,
			const GEO_COLLISION_SHAPE_FLAGS& shapeFlags,
			uint32_t entryCount, uint32_t nextBBDataChunkOffset, uint32_t nextIndexDataChunkOffset,
			const COLLISION_TAGS& collisionTags, uint32_t vertexDataOffset)
		{
			m_shapeType = shapeType;
			m_shapeFlags = shapeFlags;
			m_EntryCount = entryCount;
			m_nextBBDataChunkOffset = nextBBDataChunkOffset;
			m_unknown0 = 0;
			m_nextIndexDataChunkOffset = nextIndexDataChunkOffset;
			m_collisionTags = collisionTags;
			m_strCode32Name = 0;
			m_vertexDataOffset = vertexDataOffset;
		}
	};
}