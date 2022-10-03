#include <iostream>
#include "OBJ.h"
#include "CFileWrapper.h"
#include "Geom.h"
#include "GeomCollDefs.h"
#include "VertexData.h"
#include "GeomCreator.h"

std::string GeomCreator::replaceFileExtension(const char* fileName)
{
	std::string outputName{ fileName };
	std::size_t extensionOffset{ outputName.rfind('.') };
	std::size_t backslashOffset{ outputName.rfind('\\') };
	// erase the current OBJ's file extension, if it exists
	if (extensionOffset != std::string::npos && extensionOffset > backslashOffset)
	{
		outputName.erase(extensionOffset);
	}
	outputName.append(".geom");
	return outputName;
}

// creates a geom file
void GeomCreator::createGeom(const char* fileName, const OBJ& obj, uint32_t selectedMaterial,
	const geom::COLLISION_TAGS& collisionTags)
{
	geom::header header{};
	geom::colldef::entryheader entryHeader{ 48, 0, 0 };

	geom::colldef::entrysecondaryheader entrySecHeader{ GEOM_ENTRY_TYPE::GEOM_ENTRY_TYPE_6, 48 };
	geom::colldef::entrydatachunkdefinitionentry entryDataChunkDefEntry{ collisionTags },
		emptyDataChunkEntry{ true };
	geom::colldef::entrymaterialssection entryMaterialsSection{ selectedMaterial };

	geom::colldef::datachunk_bb dataChunkBoundingBox{ obj, collisionTags };
	// 1) datachunk quad can fail if the OBJ has weird faces or they're more than 255
	// 2) offset updates for entryDataChunkDefEntry also fail if the offset overflows
	try {
		geom::colldef::datachunk_quad dataChunkQuad{ obj, collisionTags };
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
		// there's more, the size of every single data chunk after this one has to be summed up
		dataChunkQuad.updateVertexDataOffset(dataChunkQuad.getSize() / 16);

		// ================
		// end of offset updates
		// ================

		// create the file
		std::string outputName{ replaceFileExtension(fileName) };
		smp::file outputGeom(outputName.c_str(), smp::fileflags::WRITE);

		header.write(outputGeom);
		entryHeader.write(outputGeom);
		entrySecHeader.write(outputGeom);
		// data chunk definition entries
		entryDataChunkDefEntry.write(outputGeom);
		emptyDataChunkEntry.write(outputGeom);
		// materials section
		entryMaterialsSection.write(outputGeom);
		entryMaterialsSection.writeDataChunkGap(outputGeom);

		// data chunks
		dataChunkBoundingBox.write(outputGeom);
		dataChunkBoundingBox.write(outputGeom);
		dataChunkQuad.write(outputGeom);
		vertexData.write(outputGeom);

		outputGeom.close();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
		std::cin.get();
	}
}