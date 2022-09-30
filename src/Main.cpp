#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <string_view>
#include <array>
#include "CFileWrapper.h"
#include "Geom.h"
#include "GeomCollDefs.h"
#include "GeomCreator.h"
#include "VertexData.h"
#include "OBJ.h"
#include "Common.h"

int MtrCode32(const char* materialName)
{
	uint32_t materialNameLength = strlen(materialName);
	uint32_t hash = materialNameLength;

	for (int i = materialNameLength - 1; i >= 0; i--)
		hash = hash ^ materialName[i] + hash * 0x20 + (hash >> 2);
	return hash;
}

geom::COLLISION_TAGS parseCollisionConfigFile()
{
	std::ifstream input{ "collision_flags.txt" };
	geom::COLLISION_TAGS output{};
	if (input.is_open())
	{
		std::string curLine{};
		while (std::getline(input, curLine))
		{
			// ignore comments, spaces and empty lines
			if (curLine[0] == '#' || curLine[0] == ' ' || curLine[0] == '\t' ||
				curLine[0] == '\n' || curLine == "")
				continue;
			else
			{
				for (size_t i{ 0 }; i < curLine.length(); i++)
					curLine[i] = static_cast<char>(
						toupper(static_cast<char>(curLine[i]))
					);

				if (curLine == "UNKNOWN1")
					output.UNKNOWN1 = 1;
				else if (curLine == "RECOIL_FLOOR_MAYBE")
					output.RECOIL_FLOOR_MAYBE = 1;
				else if (curLine == "UNKNOWN0")
					output.UNKNOWN0 = 1;
				else if (curLine == "CHARA_MAYBE")
					output.CHARA_MAYBE = 1;
				else if (curLine == "PLAYER")
					output.PLAYER = 1;
				else if (curLine == "ENEMY")
					output.ENEMY = 1;
				else if (curLine == "BULLET")
					output.BULLET = 1;
				else if (curLine == "MISSILE")
					output.MISSILE = 1;
				else if (curLine == "BOMB")
					output.BOMB = 1;
				else if (curLine == "UNUSED5")
					output.UNUSED5 = 1;
				else if (curLine == "BLOOD")
					output.BLOOD = 1;
				else if (curLine == "IK")
					output.IK = 1;
				else if (curLine == "STAIRWAY")
					output.STAIRWAY = 1;
				else if (curLine == "STOP_EYE")
					output.STOP_EYE = 1;
				else if (curLine == "CLIFF")
					output.CLIFF = 1;
				else if (curLine == "UNUSED4")
					output.UNUSED4 = 1;
				else if (curLine == "UNUSED6")
					output.UNUSED6 = 1;
				else if (curLine == "DONT_FALL")
					output.DONT_FALL = 1;
				else if (curLine == "CAMERA")
					output.CAMERA = 1;
				else if (curLine == "UNUSED2")
					output.UNUSED2 = 1;
				else if (curLine == "UNUSED1")
					output.UNUSED1 = 1;
				else if (curLine == "UNUSED0")
					output.UNUSED0 = 1;
				else if (curLine == "CLIFF_FLOOR")
					output.CLIFF_FLOOR = 1;
				else if (curLine == "BULLET_MARK")
					output.BULLET_MARK = 1;
				else if (curLine == "HEIGHT_LIMIT")
					output.HEIGHT_LIMIT = 1;
				else if (curLine == "DOUBLE_SLIDE")
					output.DOUBLE_SLIDE = 1;
				else if (curLine == "WATER_SURFACE")
					output.WATER_SURFACE = 1;
				else if (curLine == "TARGET_BLOCK")
					output.TARGET_BLOCK = 1;
				else if (curLine == "DOG")
					output.DOG = 1;
				else if (curLine == "NO_EFFECT")
					output.NO_EFFECT = 1;
				else if (curLine == "EVENT_PHYSICS")
					output.EVENT_PHYSICS = 1;
				else if (curLine == "NO_WALL_MOVE")
					output.NO_WALL_MOVE = 1;
				else if (curLine == "MISSILE2")
					output.MISSILE2 = 1;
				else if (curLine == "RIDE_ON_OUTER")
					output.RIDE_ON_OUTER = 1;
				else if (curLine == "FLAME")
					output.FLAME = 1;
				else if (curLine == "IGNORE_PHYSICS")
					output.IGNORE_PHYSICS = 1;
				else if (curLine == "CLIMB")
					output.CLIMB = 1;
				else if (curLine == "HORSE")
					output.HORSE = 1;
				else if (curLine == "VEHICLE")
					output.VEHICLE = 1;
				else if (curLine == "MARKER")
					output.MARKER = 1;
				else if (curLine == "RIDE_ON")
					output.RIDE_ON = 1;
				else if (curLine == "THROUGH_LINE_OF_FIRE")
					output.THROUGH_LINE_OF_FIRE = 1;
				else if (curLine == "THROUGH_ITEM_CHECK")
					output.THROUGH_ITEM_CHECK = 1;
				else if (curLine == "NO_CREEP")
					output.NO_CREEP = 1;
				else if (curLine == "NO_FULTON")
					output.NO_FULTON = 1;
				else if (curLine == "FULTON")
					output.FULTON = 1;
				else if (curLine == "ITEM")
					output.ITEM = 1;
				else if (curLine == "BOSS")
					output.BOSS = 1;
				else
				{
					std::cout << "WARNING: Unknown flag type \"" << curLine << "\"";
					std::cin.get();
				}
			}
		}
	}
	else
	{
		std::cout << "WARNING: No collision_flags.txt config file detected! The tool "
			"will automatically use the most useful flags!\n";
		output.ITEM = 1; output.MARKER = 1; output.VEHICLE = 1; output.HORSE = 1;
		output.MISSILE2 = 1; output.NO_WALL_MOVE = 1; output.BULLET_MARK = 1;
		output.CAMERA = 1; output.STOP_EYE = 1; output.IK = 1; output.BLOOD = 1;
		output.BOMB = 1; output.MISSILE = 1; output.BULLET = 1; output.ENEMY = 1;
		output.PLAYER = 1; output.CHARA_MAYBE = 1; output.UNKNOWN0 = 1;
		output.RECOIL_FLOOR_MAYBE = 1;
	}
	return output;
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

geom::COLLISION_TAGS selectedTags{};
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
		selectedTags = parseCollisionConfigFile();
		for (int i{ 1 }; i < argc; i++)
		{
			if (checkForFlag(argc, i, argv))
				continue;

			OBJ obj{ argv[i] };
			if (obj.wasSuccessful())
			{
				GeomCreator::createGeom(argv[i], obj, selectedMaterial, selectedTags);
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