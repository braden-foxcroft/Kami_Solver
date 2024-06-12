

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

// Maps integers to other integers.
// Unless otherwise specified, maps ints to themselves.
class Remapper {
private:
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
	int next(int val) {
		(*this)[val] = nextFree;
		return nextFree++;
	}
	
	// Makes a remapper by applying the input to 'this', then 'other'.
	// Assumes all inputs to 'other' are outputs from 'this'.
	// (since remappers act like the ID function unless otherwise specified.)
	Remapper chain(Remapper & other) {
		Remapper res;
		for (auto key : this->myMap) {
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
		movesMade = 0;
		// Start the history with the current coloring.
		vector<int>* startingColors = new vector(state.colors);
		history.push_back(shared_ptr<vector<int>>(startingColors));
	}
	
	// Get a list of following states
	vector<Path> followingStates() {
		// TODO
		return vector<Path>();
	}
	
	// Check if the search is complete.
	bool done() {return state.nodeCount == 1;}
	
	// Generate a score (larger = worse)
	int score() const {
		return movesMade + pow(state.nodeCount,2);
	}
	
	int moveCount() {return movesMade;}
	
	// Takes a zone map, and returns a list of zone maps, each with the colors filled in.
	vector<vector<vector<int>>> applyHistory(vector<vector<int>> zoneMap) {
		vector<vector<vector<int>>> result;
		
		// TODO
		return result;
	}
	
	// Compare to another Path. (better score first in maxQueue)
	bool operator < (Path other) const {
		return score() > other.score();
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
	Path best; // The best path we find.
	priority_queue<Path> toTry;
	toTry.push(Path(startingPoint));
	// TODO Do the search.
	
	return best.applyHistory(zoneMap);
}

