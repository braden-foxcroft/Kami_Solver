

#ifndef Solver_H
#define Solver_H

#include <unordered_set>
#include <unordered_set>
#include <string>

// Describes a graph.
struct Graph {
	int nodeCount;
	std::vector<int> colors; // The color of each node.
	std::unordered_map<int,std::unordered_set<int>> adjacent;
};

class RawBoard {
protected:
	// A Board which associates nodes with zone numbers.
	std::vector<std::vector<int>> zones;
	// A lookup table for zone colors.
	std::vector<int> zoneColors;
public:
	// Make a board from a 2d array of colors.
	// Assumes board is nonempty and well-formatted.
	RawBoard(std::vector<std::vector<int>> board);
	// Returns a graph structure corresponding to zone adjacency.
	Graph getZoneGraph();
	// Returns a string representing the board,
	// using coloring info from the input.
	std::string coloredResult(std::vector<int>);
};

// Returns a solution, as a list of coloring-info vectors.
std::vector<std::vector<std::vector<int>>> solve(RawBoard board);

#endif