

#include <vector>
#include <iostream>
#include <string>
#include "solver.h"

using namespace std;


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
			return to_string(i);
	}
}

// combines 'colored' and 'fColored', depending on mode.
// 0: nothing
// 1: Background only
// 2: Foreground only
// 3: Both
string mColored(int i, int mode) {
	if (mode == 0) return to_string(i);
	if (mode == 1) return colored(i);
	if (mode == 2) return fColored(to_string(i),i);
	if (mode == 3) return fColored(colored(i),i);
	return "?"; // Should never happen.
}

void printHelp() {
	cout << "args:\n";
	cout << "\t-c0       no colors.\n";
	cout << "\t-c1       background colors only.\n";
	cout << "\t-c2       foreground colors only.\n";
	cout << "\t-c3       both colors.\n";
	cout << "\t-help     this page.\n";
	cout << "\t-echo     debugging tool: show parsed input.\n";
	cout << "\t-zones    debugging tool: only show zones.\n";
	cout << "\t-graph    debugging tool: show initial graph state.\n";
	cout << "\t-colors   debugging tool: show different color options.\n";
}

int main(int argc, char ** argv) {
	// TODO Usage instructions
	bool echoMode = false; // Show input and quit
	bool zoneMode = false; // Show zones and quit
	bool graphMode = false; // Show graph and quit
	bool colorTest = false; // Show color options and quit.
	int colorMode = 1; // The color mode to use.
	for (int i = 1; i < argc; i++) {
		string arg = argv[i];
		if (arg == "-zones") {
			zoneMode = true;
		} else if (arg == "-echo") {
			echoMode = true;
		} else if (arg == "-graph") {
			graphMode = true;
		} else if (arg == "-c0") {
			colorMode = 0;
		} else if (arg == "-c1") {
			colorMode = 1;
		} else if (arg == "-c2") {
			colorMode = 2;
		} else if (arg == "-c3") {
			colorMode = 3;
		} else if (arg == "-colors") {
			colorTest = true;
		} else if (arg == "-help") {
			printHelp();
			return 0;
		} else {
			cout << "invalid arg: '" + arg + "'\n";
			cout << "use '-help' for help.\n";
			exit(1);
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
	
	char c;
	vector<vector<int>> board;
	unsigned int rowLen = 0;
	vector<int> row;
	while (cin.get(c)) {
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
		for (auto row : board) {
			for (auto val : row) {
				cout << mColored(val,colorMode);
			}
			cout << "\n";
		}
	}
	// Generate zone graph.
	int zoneCount; // The number of zones.
	vector<int> zoneColors; // The colors of each zone
	auto zoneBoard = genZones(board, zoneCount, zoneColors);
	if (zoneMode) {
		for (auto row : zoneBoard) {
			for (auto val : row) {
				cout << mColored(val,colorMode);
			}
			cout << "\n";
		}
		return 0;
	}
	// TODO zone board -> zone graph
	// TODO zone graph -> Path
	// TODO Path search. (in solver.cpp)
	// TODO print results.
	
	return 0;
}

