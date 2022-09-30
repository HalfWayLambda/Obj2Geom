#include <iostream>
#include "OBJ.h"
#include "CFileWrapper.h"
#include "Geom.h"
#include "GeomCollDefs.h"
#include "VertexData.h"
#include "GeomFile.h"

// creates a geom file and writes the output to the file parameter
GeomFile::GeomFile(smp::file& file, const OBJ& obj, uint32_t selectedMaterial)
{
	// this constructor creates one collision definition

	geom::header header{};
	geom::colldef::entryheader entryHeader{ 48, 0, 0 };

	geom::colldef::entrysecondaryheader entrySecHeader{ GEOM_ENTRY_TYPE::GEOM_ENTRY_TYPE_6, 48 };
	geom::colldef::entrydatachunkdefinitionentry entryDataChunkDefEntry{},
		emptyDataChunkEntry{ true };
	geom::colldef::entrymaterialssection entryMaterialsSection{ selectedMaterial };

	geom::colldef::datachunk_bb dataChunkBoundingBox{ obj };
	// 1) datachunk quad can fail if the OBJ has weird faces or they're more than 255
	// 2) offset updates for entryDataChunkDefEntry also fail if the offset overflows
	try {
		geom::colldef::datachunk_quad dataChunkQuad{ obj };
		geom::VertexData vertexData{ obj.getVertices() };
		// the geom structure is ready at this point, now the offsets have to be updated

		// =============
		// offset updates
		// =============

		const uint32_t totalDataChunksSize{
			2 * dataChunkBoundingBox.getSize() + dataChunkQuad.getSize() + vertexData.getSize()
		};

		const uint32_t dataChunkDefEntry_n_materialsSectionSize{
			entryDataChunkDefEntry.getSize() + emptyDataChunkEntry.getSize() +
			entryMaterialsSection.getSize() + entryMaterialsSection.DATACHUNK_GAP
		};

		const uint32_t sizeAfterSecHeader{
			dataChunkDefEntry_n_materialsSectionSize + totalDataChunksSize
		};

		header.updateFileSize(header.getSize() + entryHeader.getSize() + entrySecHeader.getSize() +
			sizeAfterSecHeader
		);
		entrySecHeader.updateRemainingEntryDataSize(sizeAfterSecHeader);

		entryDataChunkDefEntry.updateDataChunkCount(3); // remains static for now
		entryDataChunkDefEntry.updateDataChunksTotalDataSize(totalDataChunksSize);
		entryDataChunkDefEntry.updateOffsetToVertices(
			sizeAfterSecHeader - vertexData.getSize()
		);
		entryDataChunkDefEntry.updateOffsetToFirstDataChunk(
			dataChunkDefEntry_n_materialsSectionSize
		);
		// a cheap solution if there's only one data chunk attached to the vertex data. if
		// there's more, the size of every single data chunk after this onehas to be summed up
		dataChunkQuad.updateVertexDataOffset(dataChunkQuad.getSize() / 16);

		// ================
		// end of offset updates
		// ================

		header.write(file);
		entryHeader.write(file);
		entrySecHeader.write(file);
		// data chunk definition entries
		entryDataChunkDefEntry.write(file);
		emptyDataChunkEntry.write(file);
		// materials section
		entryMaterialsSection.write(file);
		entryMaterialsSection.writeDataChunkGap(file);

		// data chunks
		dataChunkBoundingBox.write(file);
		dataChunkBoundingBox.write(file);
		dataChunkQuad.write(file);
		vertexData.write(file);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
		std::cout << "\nThe resulting file will be empty.\n";
		std::cin.get();
	}
}