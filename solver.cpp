

#include <vector>
#include <algorithm>
#include <unordered_map>
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
};

// A partially-completed search.
class Path {
protected:
	graph state;
	int movesMade = 0;
	vector<shared_ptr<board>> history; // A list of board states (int=zone)
	vector<shared_ptr<vector<int>>> colors; // Maps zones to colors, for history.
public:
	
	Path(graph state) {
		this->state = state;
		movesMade = 0;
	}
	
	// Get a list of following states
	vector<Path> followingStates() {
		// TODO
		return vector<Path>();
	}
	
	// Check if the search is complete.
	bool complete() {return state.nodeCount == 1;}
	
	// Generate a score (larger = worse)
	int score() {
		return movesMade + pow(state.nodeCount,2);
	}
	
	int moveCount() {return movesMade;}
	
	// Compare to another Path. (better score wins)
	bool operator < (Path& other) {
		return score() > other.score();
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
board genZones(board rawInput, int& zoneCount) {
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
				floodZone(result,rawInput,x,y,zoneNum++);
			}
		}
	}
	zoneCount = zoneNum;
	return result;
}


// Takes a board, returns a complete solution.
vector<vector<vector<int>>> solve(vector<vector<int>>) {
	
	
	
	return vector<vector<vector<int>>>();
}


