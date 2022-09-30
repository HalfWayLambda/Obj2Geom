#include "Geom.h"

namespace geom {
	void writeWideVector3(smp::file& file, const WideVector3& vector)
	{
		file.write(vector.x, vector.y, vector.z, vector.empty);
	}

	void header::write(smp::file& file) const
	{
		file.write(GEOM_VERSION, FIRST_ENTRY_OFFSET, m_fileSize, u1);
		file.fill(8);
		file.write(UNUSED_HASH0, UNUSED_HASH1);
	}
}