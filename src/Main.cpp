#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <array>
#include "CFileWrapper.h"
#include "Geom.h"
#include "GeomCollDefs.h"
#include "GeomCreator.h"
#include "VertexData.h"
#include "OBJ.h"

/*
uint32_t MtrCode32(const std::string& str)
{
	uint32_t hash{ str.length() };
	for (int i{ static_cast<int>(str.length()) }; i >= 0; i--)
	{
		hash = hash ^ str[i] + hash * 0x20 + (hash >> 2);
	}
	return hash;
}
*/

int MtrCode32(const char* materialName)
{
	uint32_t materialNameLength = strlen(materialName);
	uint32_t hash = materialNameLength;

	for (int i = materialNameLength - 1; i >= 0; i--)
		hash = hash ^ materialName[i] + hash * 0x20 + (hash >> 2);
	return hash;
}

static const std::array<std::pair<uint32_t, const char*>, 82> materialMap
{
	std::pair{ 0, "MTR_IRON_A" },
	std::pair{ 1, "MTR_IRON_B" },
	std::pair{ 2, "MTR_IRON_C" },
	std::pair{ 3, "MTR_IRON_D" },
	std::pair{ 4, "MTR_IRON_E" },
	std::pair{ 5, "MTR_IRON_F" },
	std::pair{ 6, "MTR_IRON_G" },
	std::pair{ 7, "MTR_IRON_M" },
	std::pair{ 8, "MTR_IRON_N" },
	std::pair{ 9, "MTR_IRON_W" },
	std::pair{ 10, "MTR_PIPE_A" },
	std::pair{ 11, "MTR_PIPE_B" },
	std::pair{ 12, "MTR_PIPE_S" },
	std::pair{ 13, "MTR_TIN_A" },
	std::pair{ 14, "MTR_FENC_A" },
	std::pair{ 15, "MTR_FENC_B" },
	std::pair{ 16, "MTR_FENC_F" },
	std::pair{ 17, "MTR_CONC_A" },
	std::pair{ 18, "MTR_CONC_B" },
	std::pair{ 19, "MTR_BRIC_A" },
	std::pair{ 20, "MTR_PLAS_A" },
	std::pair{ 21, "MTR_PLAS_B" },
	std::pair{ 22, "MTR_PLAS_W" },
	std::pair{ 23, "MTR_PAPE_A" },
	std::pair{ 24, "MTR_PAPE_B" },
	std::pair{ 25, "MTR_PAPE_C" },
	std::pair{ 26, "MTR_PAPE_D" },
	std::pair{ 27, "MTR_RUBB_A" },
	std::pair{ 28, "MTR_RUBB_B" },
	std::pair{ 29, "MTR_CLOT_A" },
	std::pair{ 30, "MTR_CLOT_B" },
	std::pair{ 30, "MTR_CLOT_C" },
	std::pair{ 31, "MTR_CLOT_D" },
	std::pair{ 32, "MTR_CLOT_E" },
	std::pair{ 33, "MTR_GLAS_A" },
	std::pair{ 34, "MTR_GLAS_B" },
	std::pair{ 35, "MTR_GLAS_C" },
	std::pair{ 36, "MTR_VINL_A" },
	std::pair{ 37, "MTR_VINL_W" },
	std::pair{ 38, "MTR_TILE_A" },
	std::pair{ 39, "MTR_TLRF_A" },
	std::pair{ 40, "MTR_ALRM_A" },
	std::pair{ 41, "MTR_COPS_A" },
	std::pair{ 42, "MTR_COPS_B" },
	std::pair{ 43, "MTR_BRIR_A" },
	std::pair{ 44, "MTR_BLOD_A" },
	std::pair{ 45, "MTR_SOIL_A" },
	std::pair{ 46, "MTR_SOIL_B" },
	std::pair{ 47, "MTR_SOIL_C" },
	std::pair{ 48, "MTR_SOIL_D" },
	std::pair{ 49, "MTR_SOIL_E" },
	std::pair{ 50, "MTR_SOIL_F" },
	std::pair{ 51, "MTR_SOIL_G" },
	std::pair{ 52, "MTR_SOIL_H" },
	std::pair{ 53, "MTR_SOIL_R" },
	std::pair{ 54, "MTR_SOIL_W" },
	std::pair{ 55, "MTR_GRAV_A" },
	std::pair{ 56, "MTR_SAND_A" },
	std::pair{ 57, "MTR_SAND_B" },
	std::pair{ 58, "MTR_SAND_C" },
	std::pair{ 59, "MTR_LEAF" },
	std::pair{ 60, "MTR_RLEF" },
	std::pair{ 61, "MTR_RLEF_B" },
	std::pair{ 62, "MTR_WOOD_A" },
	std::pair{ 63, "MTR_WOOD_B" },
	std::pair{ 64, "MTR_WOOD_C" },
	std::pair{ 65, "MTR_WOOD_D" },
	std::pair{ 66, "MTR_WOOD_G" },
	std::pair{ 67, "MTR_WOOD_M" },
	std::pair{ 68, "MTR_WOOD_W" },
	std::pair{ 69, "MTR_FWOD_A" },
	std::pair{ 70, "MTR_PLNT_A" },
	std::pair{ 71, "MTR_ROCK_A" },
	std::pair{ 72, "MTR_ROCK_B" },
	std::pair{ 73, "MTR_ROCK_P" },
	std::pair{ 74, "MTR_MOSS_A" },
	std::pair{ 75, "MTR_TURF_A" },
	std::pair{ 76, "MTR_WATE_A" },
	std::pair{ 77, "MTR_WATE_B" },
	std::pair{ 78, "MTR_WATE_C" },
	std::pair{ 80, "MTR_AIR_A" },
	std::pair{ 81, "MTR_NONE_A" },
};

uint32_t selectedMaterial{ 3123657899 }; // MTR_IRON_A by default

// returns true if this is a flag (and adjusts whatever is needed), false otherwise
bool checkForFlag(int argc, int& i, char* argv[])
{
	if (*argv[i] != '-')
		return false;
	else
	{
		if (strcmp(argv[i], "-mtr") == 0)
		{
			i++;
			if (i == argc)
			{
				std::cout << "No parameter provided for -mtr!";
				std::cin.get();
			}
			else
			{
				std::stringstream sstream{};
				sstream << argv[i];
				unsigned int index{};
				sstream >> index;
				if (sstream.fail())
				{
					std::cout << "Bad parameter \"" << sstream.str() << "\""
						<< ", ignoring";
				}
				else if (index == materialMap.size())
				{
					std::cout << "Out of bounds index: " << index << ", ignoring\n";
					std::cout << "Material set to default: MTR_IRON_A";
					std::cin.get();
				}
				else
				{
					selectedMaterial = MtrCode32(materialMap.at(index).second);
				}
			}
		}
		else
			std::cout << "Unknown parameter " << argv[i];
	}
	return true;
}

int main(int argc, char* argv[])
{
	std::cout << "===========================\n";
	std::cout << "|        OBJ To Geom      |\n";
	std::cout << "===========================\n";

	if (argc == 1)
	{
		std::cout << "| Flags:                  |\n";
		std::cout << "===========================\n";
		std::cout << "-mtr [0,79]\n";
		for (size_t i{ 0 }; i < materialMap.size(); i++)
		{
			std::string line{ "  " };
			line.append(std::to_string(materialMap.at(i).first));
			line.append(" -> ");
			line.append(materialMap.at(i).second);
			line.append("\n");
			std::cout << line;
		}
		std::cin.get();
	}
	else if (argc > 1)
	{
		for (int i{ 1 }; i < argc; i++)
		{
			if (checkForFlag(argc, i, argv))
				continue;

			OBJ obj{ argv[i] };
			if (obj.wasSuccessful())
			{
				GeomCreator::createGeom(argv[i], obj, selectedMaterial);
			}
			else
			{
				std::cout << "Error opening " << argv[i] << ". Are you sure the path "
					"is correct and that you have the right permissions?\n";
				std::cin.get();
			}
		}
	}
}