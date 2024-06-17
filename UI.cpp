

#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include "solver.h"

using namespace std;
typedef struct Graph graph;
typedef vector<vector<int>> board;

// Like 'colored', but modifies the foreground.
string fColored(string s, int i) {
	switch (i) {
		case 0:
			return "\033[91m"s + s + "\033[39m"s;
		case 1:
			return "\033[94m"s + s + "\033[39m"s;
		case 2:
			return "\033[93m"s + s + "\033[39m"s;
		case 3:
			return "\033[92m"s + s + "\033[39m"s;
		case 4:
			return "\033[95m"s + s + "\033[39m"s;
		case 5:
			return "\033[31m"s + s + "\033[39m"s;
		case 6:
			return "\033[34m"s + s + "\033[39m"s;
		case 7:
			return "\033[33m"s + s + "\033[39m"s;
		case 8:
			return "\033[32m"s + s + "\033[39m"s;
		case 9:
			return "\033[96m"s + s + "\033[39m"s;
		default:
			return s;
	}
}

// Turns an integer into a string
string myIntToStr(int i) {
	if (i >= 10 and i <= 35) {
		// Use letters for large inputs.
		string res;
		res += 'A' + (i - 10);
		return res;
	}
	return to_string(i);
}

// Turns an int into a color-coded string.
// red, blue, orange, green, magenta,
// dark red, dark blue, dark orange, dark green, cyan
string colored(int i) {
	switch (i) {
		case 0:
			return "\033[101m0\033[49m";
		case 1:
			return "\033[104m1\033[49m";
		case 2:
			return "\033[103m2\033[49m";
		case 3:
			return "\033[102m3\033[49m";
		case 4:
			return "\033[105m4\033[49m";
		case 5:
			return "\033[41m5\033[49m";
		case 6:
			return "\033[44m6\033[49m";
		case 7:
			return "\033[43m7\033[49m";
		case 8:
			return "\033[42m8\033[49m";
		case 9:
			return "\033[106m9\033[49m";
		default:
			return myIntToStr(i);
	}
}

// combines 'colored' and 'fColored', depending on mode.
// 0: nothing
// 1: Background only
// 2: Foreground only
// 3: Both
string mColored(int i, int mode) {
	if (mode == 0) return myIntToStr(i);
	if (mode == 1) return colored(i);
	if (mode == 2) return fColored(myIntToStr(i),i);
	if (mode == 3) return fColored(colored(i),i);
	return "?"; // Should never happen.
}

string graph2Str(struct Graph g, int colorMode, bool includeColors) {
	string res;
	res += "Graph:\n";
	if (includeColors) {
		for (int i = 0; i < g.nodeCount; i++) {
			res += mColored(i,colorMode) + ": " + mColored(g.colors[i],colorMode) + "\n    => [";
			for (auto other : g.adjacent[i]) {
				res += mColored(other,colorMode) + ", ";
			}
			res += "]\n";
		}
	} else {
		for (int i = 0; i < g.nodeCount; i++) {
			res += mColored(i,colorMode) + " => [";
			for (auto other : g.adjacent[i]) {
				res += mColored(other,colorMode) + ", ";
			}
			res += "]\n";
		}
	}
	return res;
}

// A variation of graph2Str,
// Which uses zone colors instead.
string graph2StrV2(struct Graph g, int colorMode) {
	string res;
	res += "Graph:\n";
	for (int i = 0; i < g.nodeCount; i++) {
		res += mColored(g.colors[i],colorMode) + " => [";
		for (auto other : g.adjacent[i]) {
			res += mColored(g.colors[other],colorMode) + ", ";
		}
		res += "]\n";
	}
	return res;
}

string board2Str(board b, int colorMode, bool drawBorders) {
	string res;
	if (drawBorders) {
		res += "┌";
		for (uint i = 0; i < b[0].size(); i++) res += "─";
		res += "┐\n";
	}
	for (auto row : b) {
		if (drawBorders) res += "│";
		for (auto val : row) {
			res += mColored(val,colorMode);
		}
		if (drawBorders) res += "│";
		res += "\n";
	}
	if (drawBorders) {
		res += "└";
		for (uint i = 0; i < b[0].size(); i++) res += "─";
		res += "┘";
	}
	return res;
}

// Checks if the first input is a prefix of the second.
bool isPrefix(string pref, string longer) {
	auto a = std::mismatch(pref.begin(),pref.end(),longer.begin(),longer.end());
	return a.first == pref.end();
}	

void printHelp() {
	cout << "To use, enter a grid of numbers, ending in EOF (ctrl-d on linux)\n";
	cout << "Or, include a filename as one of the args.\n\n";
	cout << "args:\n";
	cout << "\t-c0        No colors.\n";
	cout << "\t-c1        Background colors only.\n";
	cout << "\t-c2        Foreground colors only.\n";
	cout << "\t-c3        Solid text (rather nice-looking).\n";
	cout << "\t-borders   Draw borders around boards (can fix some graphical bugs).\n";
	cout << "\t-auto      Assume input comes from a file or pipe, and skip user prompts.\n";
	cout << "\t           This is automatically enabled if you pass a file parameter\n";
	cout << "\t-help      This page.\n";
	cout << "\t-t=time    Stop solver after <time> seconds. Integers only.\n";
	cout << "\t-echo      debugging tool: show parsed input.\n";
	cout << "\t-zones     debugging tool: show zones.\n";
	cout << "\t-graph     debugging tool: show initial graph state.\n";
	cout << "\t-graphC    debugging tool: show initial graph state, with node colors as well.\n";
	cout << "\t-colors    debugging tool: show different color options.\n";
	cout << "\t-graphs    debugging tool: show graph history for solutions, as well.\n";
	cout << "\t-noSolve   debugging tool: don't compute solutions.\n\n";
	cout << "\t<filename> A file to automatically open and use for input.\n";
}

// like cin.get(c).
// If 'canFinish' is false, it will behave as though EOF was actually reading a linebreak.
// This is to ensure the input always ends on an empty line.
// 'fileInput' and 'inputFile' track if to read from a file instead,
// and the file to read from.
bool nextChar(char & c, bool canFinish, bool fileInput, ifstream & inputFile) {
	bool res;
	// Read a char from the appropriate source.
	if (!fileInput) {
		res = static_cast<bool>(cin.get(c));
	} else {
		res = static_cast<bool>(inputFile.get(c));
	}
	if (canFinish) return res;
	if (res) return res;
	c = '\n';
	return true;
}

int main(int argc, char ** argv) {
	// Handle args.
	bool colorTest = false; // Show color options and quit.
	bool echoMode = false; // Show input
	bool zoneMode = false; // Show zones
	bool graphMode = false; // Show graph
	bool graphCsMode = false; // Show graph with color matching.
	bool noSolve = false; // Stop before computing solution
	bool graphHistory = false; // Show graphs involved in solution.
	bool drawBorders = false; // Draw borders for graphs.
	bool fileInput = false; // Decides if we are using input from a file.
	bool noUserMessage = false; // Skips message when user manually enters input.
	ifstream inputFile; // File to use, if applicable.
	uint maxTime = 0; // The maximum time. Use timedMode to enable.
	int colorMode = 2; // The color mode to use.
	for (int i = 1; i < argc; i++) {
		string arg = argv[i];
		if (arg == "-zones") {
			zoneMode = true;
		} else if (arg == "-echo") {
			echoMode = true;
		} else if (arg == "-graph") {
			graphMode = true;
		} else if (arg == "-graphs") {
			graphHistory = true;
		} else if (arg == "-graphs") {
			graphHistory = true;
		} else if (arg == "-noSolve") {
			noSolve = true;
		} else if (arg == "-graphC") {
			graphMode = true;
			graphCsMode = true;
		} else if (arg == "-c0") {
			colorMode = 0;
		} else if (arg == "-c1") {
			colorMode = 1;
		} else if (arg == "-c2") {
			colorMode = 2;
		} else if (arg == "-c3") {
			colorMode = 3;
		} else if (arg == "-borders") {
			drawBorders = true;
		} else if (arg == "-colors") {
			colorTest = true;
		} else if (arg == "-auto") {
			noUserMessage = true;
		} else if (arg == "-help") {
			printHelp();
			return 0;
		} else if (isPrefix("-t=",arg)) {
			maxTime = stoi(arg.substr(3));
			if (maxTime == 0) {
				cout << "invalid arg: '" + arg + "'\n";
				cout << "time must be an int greater than 0!\n";
				exit(1);
			}
		} else {
			inputFile.open(arg);
			if (!inputFile.is_open()) {
				cout << "invalid arg: '" + arg + "'\n";
				cout << "use '-help' for help.\n";
				exit(1);
			}
			cout << "input file '" + arg + "' successfully opened\n";
			fileInput = true;
			noUserMessage = true;
		}
	}
	if (colorTest) {
		for (int mode = 0; mode <= 3; mode++) {
			cout << "-c" << mode << ":   ";
			for (int i = 0; i < 15; i++) {
				cout << mColored(i,mode) << " ";
			}
			cout << "\n";
		}
		return 0;
	}
	
	if (!noUserMessage) {
		cout << "Please enter a grid of digits from 0-9. This will represent the game board.\n";
		cout << "Spaces and empty lines will be safely ignored.\n";
		cout << "When you are done, please leave an empty line and press ctrl-d (EOF on linux),\n";
		cout << "or ctrl-z (EOF on Windows) to finish the input.\n";
	}
	
	char c;
	vector<vector<int>> board;
	unsigned int rowLen = 0;
	vector<int> row;
	while (nextChar(c,row.size() == 0,fileInput,inputFile)) {
		// ignore non-linebreak whitespace
		if (c == '\r' or c == ' ' or c == '\t') {continue;}
		// Ignore empty lines
		if (row.size() == 0 and c == '\n') {
			continue;
		}
		// handle non-empty lines
		if (c == '\n') {
			// Set length of rows, if needed.
			if (rowLen == 0) {rowLen = row.size();}
			// Handle mismatched row lengths.
			if (rowLen != row.size()) {
				cerr << "Bad input: lines are different lengths!\n";
				exit(2);
			}
			board.push_back(row); // save row.
			row = vector<int>(); // clear row.
			continue;
		}
		// Handle int input
		if (not (c >= '0' and c <= '9')) {
			cerr << "Bad input: non-int character given as input!\n";
			exit(2);
		}
		row.push_back(c - '0');
	}
	if (board.size() == 0) {
		cerr << "Bad input: no board provided!\n";
		exit(2);
	}
	if (echoMode) {
		cout << "Input:\n";
		cout << board2Str(board,colorMode,drawBorders);
		cout << "\n";
	}
	// Generate zone graph.
	int zoneCount; // The number of zones.
	vector<int> zoneColors; // The colors of each zone
	auto zoneBoard = genZones(board, zoneCount, zoneColors);
	if (zoneMode) {
		cout << "Zones:\n";
		for (auto row : zoneBoard) {
			for (auto val : row) {
				cout << mColored(val,colorMode);
			}
			cout << "\n";
		}
		cout << "\n";
	}
	// turn zone board into zone graph
	struct Graph startingGraph = genGraph(zoneBoard,zoneCount,zoneColors);
	if (graphMode) {
		cout << graph2Str(startingGraph, colorMode, graphCsMode) << "\n";
	}
	
	if (noSolve) return 0;
	
	
	
	// Generate solution (via solver.cpp)
	vector<vector<vector<int>>> sequence;
	vector<graph> gHistory;
	bool perfect = solve(startingGraph,zoneBoard,sequence,gHistory,maxTime);
	
	// Print results.
	if (perfect) {
		cout << "An optimal solution:\n\n";
	} else {
		cout << "Timed out. Best path found:\n\n";
	}
	for (uint i = 0; i < sequence.size(); i++) {
		cout << board2Str(sequence[i],colorMode,drawBorders);
		if (graphHistory) cout << graph2StrV2(gHistory[i],colorMode) << "\n";
		cout << "\n\n";
	}
	if (!perfect) {
		cout << "Note: due to time-out, solution may not be optimal!\n\n";
	}
	
	return 0;
}

