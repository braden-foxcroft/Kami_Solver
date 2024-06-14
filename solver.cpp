

#include <vector>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <string>
#include <cmath>
#include <memory>
#include <queue>
#include <iostream>
#include <ctime>
#include "solver.h"

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

// Below, there is a variant of 'pop' for priority_queue_Path

string graphShow(struct Graph g) {
	string res;
	res += "Graph:\n";
	for (int i = 0; i < g.nodeCount; i++) {
		res += to_string(i) + " => [";
		for (auto other : g.adjacent[i]) {
			res += to_string(other) + ", ";
		}
		res += "]\n";
	}
	return res;
}

// A simple linked list.
// Instances of this list should always be constructed using 'new'
template <typename T>
class LinkedList {
public:
	// Members are public, since this is basically
	// a glorified struct.
	T val;
	shared_ptr<LinkedList<T>> next;
	
	// Intutive constructors.
	LinkedList() {}
	LinkedList(T val, shared_ptr<LinkedList<T>> next) : val(val), next(next) {}
	LinkedList(T val) : val(val) {}
	
	// Converts it to a vector, with items in reverse order
	// (since this is how we will use it.)
	vector<T> rVector() {
		if (next == nullptr) return {val}; // For final element.
		vector<T> res = next->rVector();
		res.push_back(val);
		return res;
	}
	
};


// Maps integers to other integers.
// Unless otherwise specified, maps ints to themselves.
class Remapper {
protected:
	unordered_map<int,int> myMap;
	int nextFree = 0; // Assumes all remappings assigned via 'next()'
public:
	
	Remapper() {}
	Remapper(int generate) {
		for (int i = 0; i < generate; i++) {
			next(i);
		}
	}
	
	// Same as '[]'
	int& retrieve(int val) {
		if (myMap.find(val) == myMap.end()) {
			myMap[val] = val;
		}
		return myMap[val];
	}
	
	// sets a remapping
	int& operator [](int val) {
		return retrieve(val);
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
		result.nodeCount = nextFree;
		// Make blank spaces for results
		for (int node = 0; node < nextFree; node++) {
			result.colors.push_back(-1);
			result.adjacent[node]; // Make blank adjacency set.
		}
		// Copy across colors and adjacency.
		for (int node = 0; node < input.nodeCount; node++) {
			int newNode = retrieve(node);
			result.colors[newNode] = input.colors[node];
			for (int val : input.adjacent[node]) {
				val = retrieve(val); // Remap to new pos.
				// Node cannot be adjacent to self.
				if (val == newNode) continue;
				result.adjacent[newNode].insert(val);
			}
		}
		return result;
	}
	
	
	// Makes a remapper by applying the input to 'this', then 'other'.
	Remapper chain(Remapper & other) {
		Remapper res;
		// First, ensure this remapper has all needed keys
		for (auto key : other.myMap) {
			if (myMap.find(key.first) == myMap.end()) {
				myMap[key.first] = key.first;
			}
		}
		// Then, remap all keys as needed.
		for (auto key : myMap) {
			res[key.first] = other[key.second];
		}
		return res;
	}
	
	operator string() {
		string res = "{";
		for (auto key : myMap) {
			res += to_string(key.first) + " : " + to_string(key.second) + ", ";
		}
		return res + "}(" + to_string(nextFree) + ")\n";
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
	shared_ptr<LinkedList<vInt>> history; // A list of color mappings over time.
	shared_ptr<LinkedList<graph>> historyG; // A list of previous graphs. For debugging purposes.
public:
	
	// The default constructor is a ridiculously inefficient path.
	// Useful for ensuring that 'best' gets replaced quickly in the search.
	Path() {
		this->state.nodeCount = 1000000;
		this->movesMade = 1000000;
	}
	
	Path(graph state) : progress(state.nodeCount) {
		this->state = state;
		initialNodeCount = state.nodeCount;
		movesMade = 0;
		history = make_shared<LinkedList<vInt>>(state.colors);
		historyG = make_shared<LinkedList<graph>>(state);
	}
	
	// Get a list immediately-reachable states.
	vector<Path> followingStates() {
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
				vInt toMerge; // A list of nodes to merge with 'node'
				for (int node2 = 0; node2 < state.nodeCount; node2++) {
					// If the nodes should be merged.
					if (node != node2 and
						state.colors[node2] == nPath.state.colors[node] and
						state.adjacent[node].find(node2) != state.adjacent[node].end())
					{
						// Do nothing this pass; we do these at the end.
						toMerge.push_back(node2);
					} else {
						// No merge, new node.
						reduction.next(node2);
					}
				}
				for (auto node2 : toMerge) reduction[node2] = reduction[node];
				
				// Apply reduction to graph.
				nPath.state = reduction.reduce(nPath.state);
				// track reductions, relative to initial state of board.
				nPath.progress = nPath.progress.chain(reduction);
				// Add an entry to 'history'
				vInt newHEntry = vInt();
				for (int i = 0; i < initialNodeCount; i++) {
					// Add a new color entry, mapping from original zone #.
					newHEntry.push_back(nPath.state.colors[nPath.progress[i]]);
				}
				// Update history. Since we are dealing with linked lists,
				// we need to make a new node for each.
				nPath.history = shared_ptr<LinkedList<vInt>>(new LinkedList(newHEntry,nPath.history));
				nPath.historyG = shared_ptr<LinkedList<graph>>(new LinkedList(nPath.state,nPath.historyG));
				result.push_back(nPath); // Finally, done with the new path.
			}
		}
		return result;
	}
	
	// Check if the search is complete.
	bool done() const {return state.nodeCount == 1;}
	
	int moveCount() const {return movesMade;}
	
	vector<graph> graphHistory() {
		return historyG->rVector();
	}
	
	// Takes a zone map, and returns a list of zone maps, each with the colors filled in.
	vector<vector<vector<int>>> applyHistory(vector<vector<int>> zoneMap) const {
		vector<vector<vector<int>>> result;
		vector<vInt> myHistory = history->rVector();
		for (auto cs : myHistory) {
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
	
	// A score for the quality of the run. Low score = good.
	int score() const {
		return moveCount() + state.nodeCount * state.nodeCount;
	}
	
	// Compare to another Path. (smaller moveCount first in maxQueue)
	bool operator < (Path other) const {
		return score() > other.score();
	}
	
	// Checks which one is superior ('done' always beats 'not done')
	// True means 'other' is superior.
	bool beaten(Path other) const {
		if (other.done() and !done()) return true;
		return score() > other.score();
	}
	
	// Gives the 'fingerprint'. If two paths have the same fingerprint, then
	// they will have the same following states.
	pair<int,vInt> fingerprint() {
		return {movesMade,history->val};
	}
	
	operator string() {
		return "Path: len "s + to_string(movesMade) + "\n"s + graphShow(state);
	}
};

// An improved priority_queue.
// Extends priority_queue<Path> by enforcing that a path cannot be added if another path
// reached the same coloring in the same or fewer moves.
// TODO
class priority_queue_Path {
	priority_queue<Path> q; // The internal queue.
	map<vInt,int> bestSoFar; // For each state, the best move-count to reach it.
public:
	// push: needed from priority_queue
	void push(Path p) {
		pair<int,vInt> fp = p.fingerprint();
		int moves = fp.first;
		vInt state = fp.second;
		if (bestSoFar.find(state) != bestSoFar.end()) {
			// Another path already reached this state.
			// Check if they reached it faster.
			if (bestSoFar[state] <= moves) {
				// cout << "discarded!\n";
				return;
			}
		}
		bestSoFar[state] = moves; // Update best path.
		q.push(p);
	}
	
	// pop: works better than priority_queue version
	Path pop() {
		Path res = q.top();
		q.pop();
		return res;
	}
	
	
	// size: needed from priority_queue
	size_t size() {
		return q.size();
	}
	
	
};


Path pop(priority_queue_Path & q) {
	return q.pop();
}

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


// Solves the problem, and returns results to the '&' parameters.
// The boolean result is 'true' unless it times out.
bool solve(graph startingPoint, vector<vector<int>> zoneMap, vector<vector<vector<int>>> & result1, vector<graph> & result2, uint maxTime) {
	bool fullSearch = true;
	priority_queue_Path q;
	// priority_queue<Path> q;
	Path best;
	q.push(Path(startingPoint));
	clock_t start;
	if (maxTime) start = clock();
	clock_t cMaxTime = maxTime * CLOCKS_PER_SEC;
	// Do the search.
	while (q.size() > 0) {
		if (maxTime != 0 and clock() - start > cMaxTime) {
			fullSearch = false;
			break;
		}
		Path p = pop(q);
		// update best, if needed
		if (best.beaten(p)) best = p;
		// If a solution has already been found, trim invalid solutions.
		if (best.done() and p.moveCount() + 1 >= best.moveCount()) continue;
		// cout << (string)p << "\n";
		// Add following states.
		for (Path pNew : p.followingStates()) {
			q.push(pNew);
		}
	}
	// cout << (string)best << "\n";
	result1 = best.applyHistory(zoneMap);
	result2 = best.graphHistory();
	return fullSearch;
}

