#pragma once
#include "OBJ.h"
#include "CFileWrapper.h"

class GeomFile
{
public:
	GeomFile(smp::file&, const OBJ&, uint32_t);
};