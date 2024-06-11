

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
string mColored(int i, int mode) {
	if (mode == 0) return to_string(i);
	if (mode == 1) return colored(i);
	if (mode == 2) return fColored(colored(i),i);
	return "?"; // Should never happen.
}

int main(int argc, char * argv[]) {
	// TODO Usage instructions
	if (argc > 1) {
		cout << argv[1] << "\n";
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
				exit(1);
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
		exit(3);
	}
	// TODO: parse board, return result.
	if (argc > 1 and string(argv[1]) == "testFlooding") {
		int zoneCount;
		auto res = genZones(board,zoneCount);
		for (auto row : res) {
			for (auto val : row) {
				cout << colored(val);
			}
			cout << "\n";
		}
		return 0;
	}
	for (int mode = 0; mode <= 2; mode++) {
		for (int i = 0; i < 15; i++) {
			cout << mColored(i,mode) << " ";
		}
		cout << "\n";
	}
	
	return 0;
}

