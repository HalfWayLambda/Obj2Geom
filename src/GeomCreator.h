#pragma once
#include <string>
#include "Common.h"
#include "OBJ.h"

namespace GeomCreator {
	std::string replaceFileExtension(const char*);
	void createGeom(const char*, const OBJ&, uint32_t, const geom::COLLISION_TAGS&);
}