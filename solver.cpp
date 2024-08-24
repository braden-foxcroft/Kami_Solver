

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

// Further down, there is an additional variant of 'pop' for priority_queue_Path

// Creates a graph representation, for debugging purposes.
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
// Instances of this list should always be owned by a shared_ptr.
// Can be turned into a vector, though doing this reverses the elements.
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

// Pad a string to a given length with leading spaces.
string padSpace(string in, int max) {
	reverse(in.begin(),in.end()); // Reverse, so we can post-append spaces.
	while (in.size() < (uint)max) {
		in += ' ';
	}
	reverse(in.begin(),in.end()); // reverse again, to put spaces at beginning.
	return in;
}



// A multiset for integers from 0 to N.
// Tracks number of nonzero elements.
// Basically just an array.
class IntMultiSet {
protected:
	vector<int> data;
	int countVal = 0; // Number of nonzero elements.
	
public:
	
	IntMultiSet(int n) : data(n), countVal(0) {}
	
	// Increment or decrement value.
	void inc(int pos) {
		int& dat = data[pos];
		if (dat == 0) countVal++;
		dat++;
	}
	
	void dec(int pos) {
		int& dat = data[pos];
		if (dat == 1) countVal--;
		dat--;
	}
	
	// Counts the number of times each int occurs in the result.
	// Assumes all values are valid.
	// Should only be used as an initializer.
	void tally(vector<int> & v) {
		for (int i : v) {
			inc(i);
		}
	}
	
	// Read-only.
	int operator [](int i) {return data[i];}
	
	// Counts number of nonzero vals.
	int count() {
		return countVal;
	}

};


// Maps integers to other integers.
// Unless otherwise specified, maps ints to themselves.
// Highly cusomized for specific use cases, so functions have very weak guarantees in the general case.
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
	// Uses existing mapping, if found.
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
	
	// Gets next free int.
	int getNextFree() {
		return nextFree;
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

// Tracks the distance between every pair of nodes in a graph.
// Maintains a square grid, even though one half is unused (since the coordinate math is easier)
// Generates distances with iterative Dijkstra's for each node. O(n^3).
// Can do update by combining nodes (and using lowest value for each dist),
// then Floyd-Warshal for k=mergedNode only. O(n^2).
class DistTracker {
protected:
	vector<int> distances;
	int size = 0; // width and height of square.
	int greatestDist = -1; // Greatest distance between nodes.
	
	// Same as '()'.
	// For internal use, to make syntax easier to read.
	int& pos(int x, int y) {return operator()(x,y);}
	int& pos(int x, int y, int size, vector<int> & toAccess) {return operator()(x,y,size,toAccess);}
	
	// Takes a vector and does Dijkstra's to calculate distances from the given node.
	// Uses the graph for adjacency checks.
	int dijkstra(int node, graph & g) {
		int dist = 0; // Simply the distance of the last node checked.
		queue<int> nodes;
		// '-1' is used to say that the node we look at next is
		// one unit further than the node we just looked at.
		// It is replaced at the end of the queue whenever it is reached, to ensure
		// exactly one instance at all times.
		nodes.push(node);
		nodes.push(-1);
		while (true) {
			int next = pop(nodes);
			if (nodes.size() == 0) break; // only '-1' was left in the list.
			if (next == -1) {
				dist++;
				nodes.push(-1);
				continue;
			}
			// Update dist of node
			pos(next,node) = dist;
			// Add adjacent nodes to queue if needed.
			for (int i : g.adjacent[next]) {
				if (pos(i,node) == -1) nodes.push(i); // Add to end of list.
			}
		}
		return dist; // The distance of the furthest node.
	}
	
	
public:
	// Figure out distances
	DistTracker(graph & g) {
		size = g.nodeCount;
		// Create distance table.
		distances = vector<int>(size*size,-1);
		// Start with iterative Dijkstra's.
		// Better than Floyd-Warshal's O(n^3),
		// since graphs are sparse and equal-weighted, meaning O(n^2 + ne) overall.
		// This may not be asymptotically better, but e < n^2, so it's better in practice.
		int max = 0;
		for (int i = 0; i < size; i++) {
			int newMax = dijkstra(i,g);
			if (newMax > max) max = newMax;
		}
		greatestDist = max;
	}
	
	// like [], but allows an x and y pair.
	int& operator ()(int x, int y) {
		return operator()(x,y,size,distances);
	}
	
	// like [], but allows an x and y pair.
	int& operator ()(int x, int y, int size, vector<int> & dists) {
		// Reverses params if needed.
		if (y > x) return dists[y + x * size];
		return dists[x + y * size];
	}
	
	// combines nodes in accordance with remapper, recalculates distances as needed.
	void reduce(Remapper & red, int mergedNode) {
		int nodeCount = red.getNextFree();
		vector<int> newDist(nodeCount*nodeCount,-1);
		// Merge cells, keeping smallest values.
		for (int x = 0; x < nodeCount; x++) {
			int xN = red[x];
			for (int y = 0; y < x; y++) {
				int yN = red[y];
				int & p = pos(x,y);
				int & pN = pos(xN,yN,nodeCount,newDist);
				if (pN == -1) {
					pN = p; // New value
				} else if (p < pN) {
					pN = p; // Updated value
				} // Otherwise, leave value as-is.
			}
		}
		// Update minimum distances, using Floyd-Warshal's algorithm with k=mergedNode only.
		int max = -1;
		for (int x = 0; x < nodeCount; x++) {
			for (int y = 0; y < x; y++) {
				int & xy = pos(x,y,nodeCount,newDist);
				int & xk = pos(x,mergedNode,nodeCount,newDist);
				int & ky = pos(y,mergedNode,nodeCount,newDist);
				if (xy > xk + ky) xy = xk + ky;
				if (xy > max) max = xy;
			}
		}
		// Store results back.
		distances = newDist;
		size = nodeCount;
		greatestDist = max;
	}
	
	operator string() {
		string res;
		for (int y = 0; y < size; y++) {
			for (int x = 0; x < (size - y); x++) {
				res += padSpace(to_string(pos(x,y)),3);
			}
			res += '\n';
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
	IntMultiSet colorCounts; // The number of instances of each color.
	int movesMade = 0;
	shared_ptr<LinkedList<vInt>> history; // A list of color mappings over time.
	shared_ptr<LinkedList<graph>> historyG; // A list of previous graphs. For debugging purposes.
public:
	
	// The default constructor is a ridiculously inefficient path.
	// Useful for ensuring that 'best' gets replaced quickly in the search.
	Path() : colorCounts(1) {
		this->state.nodeCount = 1000000;
		this->movesMade = 1000000;
	}
	
	Path(graph state) : progress(state.nodeCount), colorCounts(10) {
		this->state = state;
		initialNodeCount = state.nodeCount;
		movesMade = 0;
		history = make_shared<LinkedList<vInt>>(state.colors);
		historyG = make_shared<LinkedList<graph>>(state);
		colorCounts.tally(state.colors); // use for filtering.
	}
	
	// Finds the two furthest nodes apart on the current graph.
	// Returns a vector with a list of nodes in the path.
	// If the length is greater, returns two nodes further than that distance apart.
	vector<int> longestPath(int moveLimit) {
		// TODO
		return {}; // For now, ignore.
	}
	
	// Get a list of immediately-reachable states.
	// TODO various params: doneOnly, colorCapped, distCapped.
	vector<Path> followingStates(int moveLimit) {
		vector<Path> result;
		// checks if a color must be eliminated this turn.
		bool colorCapped = false;
		// checks if the longest path must be shortened this turn.
		// If so, which nodes are part of the path.
		bool distCapped = false;
		vector<bool> keyNodes;
		// Check if there are too many colors to complete in time.
		if (moveLimit != -1) {
			if (movesMade + colorCounts.count() - 1 >= moveLimit) return result;
			// Check if there are the exact limit of colors.
			if (movesMade + colorCounts.count() == moveLimit) {
				colorCapped = true;
			} else {
				// TODO Check if dist-capped.
				// TODO check max dist and path.
				
			}
		}
		// TODO use 'capped' vars.
		
		// For each node, try all reasonable actions
		for (int node = 0; node < state.nodeCount; node++) {
			// if colorCapped, require color count to be 1.
			if (colorCapped and colorCounts[state.colors[node]] != 1) continue;
			
			unordered_set<int> colorOptions;
			// Make a list of color changes for the node.
			for (int node2 : state.adjacent[node]) {
				colorOptions.insert(state.colors[node2]);
			}
			// Iterate through valid colorings, adding necessary new options
			for (int nColor : colorOptions) {
				Path nPath(*this); // Copy the existing setup.
				nPath.colorCounts.dec(nPath.state.colors[node]); // Decrement color count for color.
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
				// If this is a winning state, then return only this.
				// Everything else is extraneous.
				if (nPath.done()) return {nPath};
				// Otherwise, append completed path.
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
	
	// Takes a zone map, and returns a list of boards, each with the colors filled in.
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
	
	// Compare to another Path. (used for a maxQueue)
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
bool solve(graph startingPoint, vector<vector<int>> zoneMap, vector<vector<vector<int>>> & result1, vector<graph> & result2, uint maxTime, uint & iterations) {
	bool fullSearch = true;
	priority_queue_Path q;
	// priority_queue<Path> q;
	Path best;
	q.push(Path(startingPoint));
	clock_t start;
	if (maxTime) start = clock();
	clock_t cMaxTime = maxTime * CLOCKS_PER_SEC;
	// Do the search.
	uint iterCount = 0;
	while (q.size() > 0) {
		iterCount++;
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
		int moveLimit = -1; // Means 'no limit'
		if (best.done()) moveLimit = best.moveCount(); // Sets a move limit.
		for (Path pNew : p.followingStates(moveLimit)) {
			q.push(pNew);
		}
	}
	// cout << (string)best << "\n";
	if (!best.done()) {
		result1 = {};
		result2 = {};
		cout << "Failed to find result.\n";
		return false;
	}
	
	result1 = best.applyHistory(zoneMap);
	result2 = best.graphHistory();
	iterations = iterCount;
	return fullSearch;
}

// TODO: add additional optimizations: color count, (max distance - 1) / 2

