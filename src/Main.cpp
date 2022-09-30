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

static const std::array<const char*, 82> materialMap
{
	"MTR_IRON_A",
	"MTR_IRON_B",
	"MTR_IRON_C",
	"MTR_IRON_D",
	"MTR_IRON_E",
	"MTR_IRON_F",
	"MTR_IRON_G",
	"MTR_IRON_M",
	"MTR_IRON_N",
	"MTR_IRON_W",
	"MTR_PIPE_A",
	"MTR_PIPE_B",
	"MTR_PIPE_S",
	"MTR_TIN_A",
	"MTR_FENC_A",
	"MTR_FENC_B",
	"MTR_FENC_F",
	"MTR_CONC_A",
	"MTR_CONC_B",
	"MTR_BRIC_A",
	"MTR_PLAS_A",
	"MTR_PLAS_B",
	"MTR_PLAS_W",
	"MTR_PAPE_A",
	"MTR_PAPE_B",
	"MTR_PAPE_C",
	"MTR_PAPE_D",
	"MTR_RUBB_A",
	"MTR_RUBB_B",
	"MTR_CLOT_A",
	"MTR_CLOT_B",
	"MTR_CLOT_C",
	"MTR_CLOT_D",
	"MTR_CLOT_E",
	"MTR_GLAS_A",
	"MTR_GLAS_B",
	"MTR_GLAS_C",
	"MTR_VINL_A",
	"MTR_VINL_W",
	"MTR_TILE_A",
	"MTR_TLRF_A",
	"MTR_ALRM_A",
	"MTR_COPS_A",
	"MTR_COPS_B",
	"MTR_BRIR_A",
	"MTR_BLOD_A",
	"MTR_SOIL_A",
	"MTR_SOIL_B",
	"MTR_SOIL_C",
	"MTR_SOIL_D",
	"MTR_SOIL_E",
	"MTR_SOIL_F",
	"MTR_SOIL_G",
	"MTR_SOIL_H",
	"MTR_SOIL_R",
	"MTR_SOIL_W",
	"MTR_GRAV_A",
	"MTR_SAND_A",
	"MTR_SAND_B",
	"MTR_SAND_C",
	"MTR_LEAF",
	"MTR_RLEF",
	"MTR_RLEF_B",
	"MTR_WOOD_A",
	"MTR_WOOD_B",
	"MTR_WOOD_C",
	"MTR_WOOD_D",
	"MTR_WOOD_G",
	"MTR_WOOD_M",
	"MTR_WOOD_W",
	"MTR_FWOD_A",
	"MTR_PLNT_A",
	"MTR_ROCK_A",
	"MTR_ROCK_B",
	"MTR_ROCK_P",
	"MTR_MOSS_A",
	"MTR_TURF_A",
	"MTR_WATE_A",
	"MTR_WATE_B",
	"MTR_WATE_C",
	"MTR_AIR_A",
	"MTR_NONE_A",
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
				else if (index >= materialMap.size())
				{
					std::cout << "Out of bounds index: " << index << ", ignoring\n";
					std::cout << "Material set to default: MTR_IRON_A";
					std::cin.get();
				}
				else
				{
					selectedMaterial = MtrCode32(materialMap.at(index));
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
			line.append(std::to_string(i));
			line.append(" -> ");
			line.append(materialMap.at(i));
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