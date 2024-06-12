

#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <cmath>
#include <memory>
#include <queue>
#include "solver.h"
#include <iostream>

using namespace std;
typedef vector<int> vInt;
typedef struct Graph graph;
typedef vector<vector<int>> board;
typedef unsigned int uint;

// pops and returns an item.
template <typename T>
T pop(queue<T> & q) {
	T res = q.front();
	q.pop();
	return res;
}

// Same, but for priority_queue
template <typename T>
T pop(priority_queue<T> & q) {
	T res = q.top();
	q.pop();
	return res;
}

// TODO remove, for testing purposes only.
// (This is like shared_ptr, but with debugging info)
class myShared {
	
}


// Maps integers to other integers.
// Unless otherwise specified, maps ints to themselves.
class Remapper {
protected:
	unordered_map<int,int> myMap;
	int nextFree = 0; // Assumes all remappings assigned via 'next()'
public:

	// sets a remapping
	int& operator [](int val) {
		if (myMap.find(val) == myMap.end()) {
			myMap[val] = val;
		}
		return myMap[val];
	}
	
	// Assigns a number to the next free possibility.
	// Uses existing mapping, if found
	int next(int val) {
		if (myMap.find(val) != myMap.end()) {
			return myMap[val];
		}
		(*this)[val] = nextFree;
		return nextFree++;
	}
	
	// Takes a graph, and remaps the node numbers, merging when necessary.
	// Assumes this Remapper was generated using 'next'.
	graph reduce(graph input) {
		graph result;
		// TODO check
		result.nodeCount = nextFree;
		// Make blank spaces for results
		for (int node = 0; node < nextFree; node++) {
			result.colors.push_back(-1);
			result.adjacent[node]; // Make blank adjacency set.
		}
		// Copy across colors and adjacency.
		for (int node = 0; node < input.nodeCount; node++) {
			int newNode = (*this)[node];
			result.colors[newNode] = input.colors[node];
			for (int val : input.adjacent[node]) result.adjacent[newNode].insert(val);
		}
		return result;
	}
	
	
	// Makes a remapper by applying the input to 'this', then 'other'.
	Remapper chain(Remapper & other) {
		Remapper res;
		// First, ensure this remapper has all needed keys
		for (auto key : other.myMap) {
			if (myMap.find(key.first) != myMap.end()) {
				myMap[key.first] = key.first;
			}
		}
		// Then, remap all keys as needed.
		for (auto key : myMap) {
			res[key.first] = other[key.second];
		}
		return res;
	}
};

// A partially-completed search.
// Note: original board state is not preserved,
// simply color assignments by zone.
class Path {
protected:
	int initialNodeCount = 0;
	Remapper progress;
	graph state;
	int movesMade = 0;
	vector<shared_ptr<vInt>> history; // A list of color mappings over time.
public:
	
	// The default constructor is a ridiculously inefficient path.
	// Useful for 
	Path() {
		this->state.nodeCount = 1000000;
		this->movesMade = 1000000;
	}
	
	Path(graph state) {
		this->state = state;
		initialNodeCount = state.nodeCount;
		movesMade = 0;
		// Start the history with the current coloring.
		shared_ptr<vInt> startingColors = make_shared<vInt>(state.colors);
		history.push_back(move(startingColors));
	}
	
	// Get a list immediately-reachable states.
	vector<Path> followingStates() {
		// TODO check
		vector<Path> result;
		// For each node, try all reasonable actions
		for (int node = 0; node < this->state.nodeCount; node++) {
			unordered_set<int> colorOptions;
			// Make a list of color changes for the node.
			for (int node2 : this->state.adjacent[node]) {
				colorOptions.insert(this->state.colors[node2]);
			}
			// Iterate through valid colorings, adding necessary new options
			for (int nColor : colorOptions) {
				Path nPath(*this); // Copy the existing setup.
				nPath.state.colors[node] = nColor; // Change node color.
				nPath.movesMade += 1; // One more move made.
				Remapper reduction; // Remaps the node numbers.
				// Find nodes to combine, and populate 'reduction'
				for (int node2 = 0; node2 < state.nodeCount; node2++) {
					// If the nodes should be merged.
					if (state.colors[node2] == nPath.state.colors[node] and
						state.adjacent[node].find(node2) != state.adjacent[node].end())
					{
						// Should be remapped to 'node'
						reduction[node2] = reduction[node];
					} else {
						// No merge, new node.
						reduction.next(node);
					}
				}
				// Apply reduction to graph.
				nPath.state = reduction.reduce(nPath.state);
				// track reductions, relative to initial state of board.
				nPath.progress = nPath.progress.chain(reduction);
				// Add an entry to 'history'
				shared_ptr<vInt> newHEntry = make_shared<vInt>();
				for (int i = 0; i < initialNodeCount; i++) {
					// Add a new color entry, mapping from original zone #.
					newHEntry->push_back(nPath.state.colors[progress[i]]);
				}
				history.push_back(move(newHEntry));
				result.push_back(nPath); // Finally, done with the new path.
			}
		}
		return result;
	}
	
	// Check if the search is complete.
	bool done() const {return state.nodeCount == 1;}
	
	int moveCount() const {return movesMade;}
	
	// Takes a zone map, and returns a list of zone maps, each with the colors filled in.
	vector<vector<vector<int>>> applyHistory(vector<vector<int>> zoneMap) const {
		// TODO check.
		vector<vector<vector<int>>> result;
		for (auto csP : this->history) {
			vInt & cs = *csP; // Dereference pointer.
			vector<vector<int>> nextBoard = zoneMap;
			for (auto & row : nextBoard) {
				for (auto & val : row) {
					val = cs[val]; // Zone -> color mapping.
				}
			}
			result.push_back(nextBoard);
		}
		return result;
	}
	
	// Compare to another Path. (smaller moveCount first in maxQueue)
	bool operator < (Path other) const {
		return moveCount() > other.moveCount();
	}
	
	operator string() {
		return "A Path object."s;
	}
};

// Takes a blank (-1 populated) 'zones' board, a 'colors' board,
// an x,y, and zoneNum.
// Fills the area of the 'zones' board which corresponds to the given zone in 'colors'
void floodZone(board& zones, board& colors, uint x, uint y, uint zoneNum) {
	uint width = zones[0].size();
	uint height = zones.size();
	queue<int> xs;
	queue<int> ys;
	xs.push(x);
	ys.push(y);
	int color = colors[y][x]; // The color to flood.
	while (xs.size() != 0) {
		x = pop(xs);
		y = pop(ys);
		if (zones[y][x] != -1) continue;
		if (colors[y][x] != color) continue;
		zones[y][x] = zoneNum;
		if (x > 0) {
			xs.push(x-1);
			ys.push(y);
		}
		if (y > 0) {
			xs.push(x);
			ys.push(y-1);
		}
		if (x < width - 1) {
			xs.push(x+1);
			ys.push(y);
		}
		if (y < height - 1) {
			xs.push(x);
			ys.push(y+1);
		}
	}
}

// Takes a board (of colors), returns a board (of zones)
board genZones(board rawInput, int& zoneCount, vector<int> & zoneColors) {
	// Generate a blank board.
	board result = rawInput;
	// Clear board
	for (auto& row : result) {
		for (auto& val : row) {
			val = -1;
		}
	}
	uint height = result.size();
	uint width = result[0].size();
	int zoneNum = 0; // A counter for next free zone#
	for (uint y = 0; y < height; y++) {
		for (uint x = 0; x < width; x++) {
			if (result[y][x] == -1) {
				zoneColors.push_back(rawInput[y][x]);
				floodZone(result,rawInput,x,y,zoneNum++);
			}
		}
	}
	zoneCount = zoneNum;
	return result;
}

graph genGraph(board zones, int zoneCount, vector<int> zoneColors) {
	graph res;
	res.nodeCount = zoneCount;
	res.colors = zoneColors;
	for (int i = 0; i < zoneCount; i++) res.adjacent[i] = unordered_set<int>();
	uint height = zones.size();
	uint width = zones[0].size();
	for (uint y = 0; y < height; y++) {
		for (uint x = 0; x < width; x++) {
			if (x > 0 and zones[y][x] != zones[y][x-1]) {
				res.adjacent[zones[y][x]].insert(zones[y][x-1]);
				res.adjacent[zones[y][x-1]].insert(zones[y][x]);
			}
			if (y > 0 and zones[y][x] != zones[y-1][x]) {
				res.adjacent[zones[y][x]].insert(zones[y-1][x]);
				res.adjacent[zones[y-1][x]].insert(zones[y][x]);
			}
			// Only check above and left, since insertion is bidirectional.
		}
	}
	return res;
}


vector<vector<vector<int>>> solve(struct Graph startingPoint, vector<vector<int>> zoneMap) {
	priority_queue<Path> q;
	q.push(Path(startingPoint));
	// TODO Do the search.
	while (!q.top().done()) {
		Path p = pop(q);
		// Add following states.
		for (auto pNew : p.followingStates()) q.push(pNew);
	}
	return q.top().applyHistory(zoneMap);
}

