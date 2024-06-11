

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

// Only visible for testing purposes.
std::vector<std::vector<int>> genZones(std::vector<std::vector<int>> rawInput, int& zoneCount);

// Returns a solution, as a list of coloring-info vectors.
std::vector<std::vector<std::vector<int>>> solve(std::vector<std::vector<int>>);

#endif