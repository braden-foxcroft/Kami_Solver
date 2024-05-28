

#include <vector>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <cmath>
#include "solver.h"

using namespace std;
typedef vector<int> vInt;
typedef struct Graph graph;


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
		this[val] = nextFree;
		return nextFree++;
	}
};

// A partially-completed search.
class PartialSearch {
protected:
	graph state;
	int movesMade = 0;
	vector<Remapper> reductions; // A list of moves made.
public:
	PartialSearch(RawBoard b) {
		state = b.getZoneGraph();
		movesMade = 0;
	}
	// Get a list of following states
	vector<PartialSearch> followingStates() {
		// TODO
	}
	// Check if the search is complete.
	bool complete() {return state.nodeCount == 1;}
	// Generate a score (larger = worse)
	int score() {
		return movesMade + pow(state.nodeCount,2);
	}
	int moveCount() {return movesMade;}
	// Compare to another PartialSearch. (better score wins)
	bool operator < (RawBoard& other) {
		return score() > other.score();
	}
}

// TODO take graph, produce possibilities.


vector<vector<

// Takes a board, returns a complete solution.
vector<vector<vector<int>>> solve(RawBoard board) {
	graph g = board.getZoneGraph();
	
	return vector<vector<int>>();
}


