#pragma once
#include <stdint.h>
#include <vector>
#include "CFileWrapper.h"
#include "Common.h"
#include "OBJ.h"

class ISection
{
public:
	void virtual write(smp::file&) const = 0;
	unsigned int virtual getSize() const = 0;
};

namespace geom {
	void writeWideVector3(smp::file&, const WideVector3&);

	class header : public ISection
	{
	private:
		uint32_t m_version{ 201406020 };
		uint32_t m_firstEntryOffset{ 32 };
		uint32_t m_fileSize{ 0 }; // has to be updated later
		static constexpr inline uint32_t UNUSED_HASH0{ 1778469995 };
		static constexpr inline uint32_t UNUSED_HASH1{ 2321276988 };
		static constexpr inline uint32_t u1{ 7050094 };

	public:
		void updateFileSize(uint32_t newSize) { m_fileSize = newSize; }
		void write(smp::file&) const override;
		unsigned int getSize() const override { return 32; }
	};
}