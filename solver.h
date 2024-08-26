

#ifndef Solver_H
#define Solver_H

#include <unordered_set>
#include <unordered_map>
#include <string>

// Describes a graph.
struct Graph {
	int nodeCount;
	std::vector<int> colors; // The color of each node.
	std::unordered_map<int,std::unordered_set<int>> adjacent;
};

// Takes a board of colors, returns a board of zone numbers.
std::vector<std::vector<int>> genZones(std::vector<std::vector<int>> rawInput, int& zoneCount, std::vector<int> & zoneColors);


struct Graph genGraph(std::vector<std::vector<int>> zones, int zoneCount, std::vector<int> zoneColors);

bool solve(struct Graph startingPoint, std::vector<std::vector<int>> zoneMap, std::vector<std::vector<std::vector<int>>> & result1, std::vector<struct Graph> & result2, unsigned int maxTime, uint & iterations, int minSol, int maxSol);

#endif
