#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

struct Vector3
{
	float x, y, z;
};

class Face
{
public:
	using facelist = std::vector<unsigned short>;

private:
	facelist m_faces;

public:
	Face(const facelist& list) : m_faces{ list }
	{}

	const facelist& getFaceElements() const
	{
		return m_faces;
	}
};

class OBJ
{
private:
	std::vector<Vector3> m_vertices;
	std::vector<Face> m_faces;
	// min corner has the smallest x,y,z coordinates from the OBJ,
	// and max corner the biggest
	Vector3 m_minCorner{}, m_maxCorner{};
	std::ifstream m_file;
	bool m_openSuccess{ false };

public:
	const std::vector<Vector3>& getVertices() const { return m_vertices; }
	const std::vector<Face>& getFaces() const { return m_faces; }
	const Vector3& getMinCorner() const { return m_minCorner; }
	const Vector3& getMaxCorner() const { return m_maxCorner; }
	bool wasSuccessful() const { return m_openSuccess; }

	OBJ(const char* filePath)
	{
		m_file.open(filePath);
		if (m_file && m_file.good())
		{
			m_openSuccess = true;
			Vector3 minCorner{}, maxCorner{};

			std::string curLine;
			while (std::getline(m_file, curLine))
			{
				std::stringstream sstream;
				std::string curWord;
				
				sstream << curLine;
				sstream >> curWord;
				if (curWord == "f")
				{
					Face::facelist faces;
					faces.reserve(4);
					while (sstream >> curWord)
					{
						size_t slashOffset{ curWord.find('/') };
						std::string choppedStr{ curWord.substr(0, slashOffset) };
						faces.push_back(static_cast<unsigned short>(std::stoul(choppedStr.substr())));
					}
					Face newFace{ faces };
					m_faces.push_back(std::move(newFace));
				}
				else if (curWord == "v")
				{
					float x{}, y{}, z{};
					sstream >> x >> y >> z;
					Vector3 vector{ x,y,z };
					m_vertices.push_back(std::move(vector));
					// update minCorner and maxCorner
					if (x < minCorner.x)
						minCorner.x = x;
					if (x > maxCorner.x)
						maxCorner.x = x;

					if (y < minCorner.y)
						minCorner.y = y;
					if (y > maxCorner.y)
						maxCorner.y = y;

					if (z < minCorner.z)
						minCorner.z = z;
					if (z > maxCorner.z)
						maxCorner.z = z;
				}
			}
			m_minCorner = minCorner;
			m_maxCorner = maxCorner;

			m_file.close();
		}
	}
};