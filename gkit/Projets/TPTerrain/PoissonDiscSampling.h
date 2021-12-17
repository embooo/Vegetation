#pragma once

#include "vec.h"
#include <list>
#include <vector>
#include <random>

class PoissonDiscSampling
{
public:
	PoissonDiscSampling() = delete;
	static float  s_Radius;
	static int    s_gridWidth;
	static int    s_gridHeight;
	static float  s_cellSize;
	static Vector s_regionSize;

	static std::pair<int, int> CellCoordinate(const Vector& point, float cellSize)
	{
		return std::make_pair<int, int>(point.x / cellSize, point.z / cellSize);
	}
	static bool IsBetween(float v, float a, float b) { return v >= a && v <= b; }
	static bool IsValid(const Vector& point, const std::vector<Vector>& generatedPoints, const std::vector<int>& grid)
	{
		if (IsBetween(point.x, 0, s_regionSize.x) && IsBetween(point.z, 0, s_regionSize.z))
		{
			std::pair<int, int> coord = CellCoordinate(point, s_cellSize);
			// Search around the cell

			for (int x = std::max(0, coord.first - 2); x <= std::min(coord.first + 2, s_gridWidth - 1); x++)
			{
				for (int y = std::max(0, coord.second - 2); y <= std::min(coord.second + 2, s_gridHeight - 1); y++)
				{
					int index = grid.at((x * s_gridWidth + y)) - 1;
					// Check the distance between our point and all other points in this region
					// We want to place a point within a distance of RADIUS of all the other points
					if (index != -1)
					{
						if (length2((point - generatedPoints.at(index))) < (s_Radius * s_Radius)) // length2 to avoid sqrt
						{
							return false;
						}
					}

				}
			}

			return true;
		}
		return false;
	}
	/// <summary>
	/// 
	/// </summary>
	/// <param name="radius"> Radius of the Poisson disc </param>
	/// <param name="regionSize"> </param>
	/// <param name="numTries"> Number of tries before we reject the point around which new points are generated </param>
	/// <returns></returns>
	static std::vector<Vector> GeneratePoints(float radius, const Vector& regionSize, int numTries = 30)
	{
		s_Radius	 = radius;
		s_cellSize   = radius / sqrt(2);
		s_gridWidth  = std::ceil(regionSize.x / s_cellSize);
		s_gridHeight = std::ceil(regionSize.z / s_cellSize);
		s_regionSize = regionSize;

		// To retrieve the index of the point lying in a cell. 
		// Index of 0 indicates that the cell is empty
		//  Otherwise, the index is : index - 1.
		std::vector<int> grid(s_gridWidth * s_gridHeight);
		
		// Points around which we generate new points
		std::vector<Vector> sourcePoints;
		
		// Holds all generated points
		std::vector<Vector> generatedPoints;

		// Add a starting points in the middle
		sourcePoints.push_back(Vector(regionSize.x / 2.0f, 0, regionSize.z / 2.0f));

		std::random_device rd;
		std::mt19937 randGen(rd());
		
		std::uniform_int_distribution<> randSourcePointsIndex(0, sourcePoints.size() - 1);

		std::uniform_real_distribution<float> randDistance(radius, 2.0 * radius);

		while (sourcePoints.size() > 0)
		{
			int    randomSourceIndex = randSourcePointsIndex(randGen);
			Vector sourcePointCenter(sourcePoints.at(randomSourceIndex));

			bool noneValid = true; // To check if the disc is still "good" i.e it can accept new points, is not fully packed

			for (int i = 0; i < numTries; i++)
			{
				double theta = 2 * M_PI * (rand() / double(RAND_MAX));	// Generate a random direction
				Vector dir(sin(theta), 0, cos(theta));

				Vector point(sourcePointCenter + dir * randDistance(randGen));

				if (IsValid(point, generatedPoints, grid))
				{
					noneValid = false;
					sourcePoints.push_back(point);
					generatedPoints.push_back(point);

					// Set the index of the generated point
					int x = point.x / s_cellSize;
					int z = point.z / s_cellSize;
					grid.at(x * s_gridWidth + z) = generatedPoints.size();
					break;
				}
			}

			if (noneValid)
			{
				sourcePoints.erase(sourcePoints.begin() + randomSourceIndex);
			}
		}

		return generatedPoints;
	}

};