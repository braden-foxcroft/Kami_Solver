

#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include "solver.h"

using namespace std;
typedef struct Graph graph;
typedef vector<vector<int>> board;

// Changes the foreground color of a string.
// Takes an int. 0-9 chooses a color, any other case leaves the text the default color.
string fColored(string s, int i) {
	switch (i) {
		case 0:
			return "\033[90m"s + s + "\033[39m"s;
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

// Turns a single-digit integer into a single-char string.
// Converts larger integers to capital letters.
// It should never recieve larger values than that,
// but if it does, it returns them as-is.
string myIntToStr(int i) {
	if (i >= 10 and i <= 35) {
		// Use letters for large inputs.
		string res;
		res += 'A' + (i - 10);
		return res;
	}
	return to_string(i);
}

// Turns a single-digit int into a string with a corresponding background color.
// for ints larger than 9, returns an uncolored capital letter.
// red, blue, orange, green, magenta,
// dark red, dark blue, dark orange, dark green, cyan
string colored(int i) {
	switch (i) {
		case 0:
			return "\033[100m0\033[49m";
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

// Returns a colored digit by mode.
// Mode options:
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

// Converts a graph to a printable, potentially colored string.
// 'includeColors' decides if to display the node colors (graph metadata)
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
// Which displays zone colors instead.
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

// Converts a board to a string. Does not validate inputs.
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
		res += "┘\n";
	}
	return res;
}

// Checks if the first input is a prefix of the second.
bool isPrefix(string pref, string longer) {
	auto a = std::mismatch(pref.begin(),pref.end(),longer.begin(),longer.end());
	return a.first == pref.end();
}	

void printHelp(bool debug) {
	cout << "To use, enter a grid of numbers, ending in the character 'q'.\n";
	cout << "Or, include an input filename as one of the args.\n";
	cout << "Alternatively, a solution file can be printed with color, using '-s=<file>' and '-c0' through '-c3'.\n\n";
	cout << "args:\n";
	cout << "\t-c0         No colors.\n";
	cout << "\t-c1         Background colors only.\n";
	cout << "\t-c2         Foreground colors only.\n";
	cout << "\t-c3         Solid text (rather nice-looking).\n";
	cout << "\t-borders    Draw borders around boards (can fix some graphical bugs).\n";
	cout << "\t-auto       Assume input comes from a file or pipe, and skip user prompts.\n";
	cout << "\t            This is automatically enabled if you pass a file parameter\n";
	cout << "\t-help       This page.\n";
	cout << "\t-help=d     This page, with debugging arguments.\n";
	cout << "\t-t=time     Stop solver after <time> seconds. Integers only.\n";
	if (debug) {
		cout << "\n";
		cout << "debugging tools:\n";
		cout << "\t-echo       show parsed and colored input.\n";
		cout << "\t-zones      show zones.\n";
		cout << "\t-graph      show initial graph state.\n";
		cout << "\t-graphC     show initial graph state, with node colors as well.\n";
		cout << "\t-colors     show different color options.\n";
		cout << "\t-graphs     show graph history for solutions, as well.\n";
		cout << "\t-count      count number of partial paths processed.\n";
		cout << "\t-noSolve    don't compute solutions.\n";
	}
	cout << "\n";
	cout << "The following two args can also be included on blank lines in an input file:\n";
	cout << "(If the optimal move count is known, set 'min' and 'max' to that number to massively improve performance!)\n";
	cout << "\t-min=n      Stop solver if a solution is found with 'n' moves or less.\n";
	cout << "\t-max=n      Don't consider solutions with more than 'n' moves.\n";
	cout << "\n";
	cout << "file inputs:\n";
	cout << "\t-s=<file>   A solution file to color and print out. Use -c0 to -c3 to select coloring.\n";
	cout << "\t<filename>  A file to automatically open and use for input.\n";
}

// An enriched variation of cin.get(c).
// If 'canFinish' is false, it will behave as though EOF was actually a linebreak.
// This is to ensure the input always ends on an empty line.
// 'fileInput' and 'inputFile' track if to read from a file instead,
// and the file to read from, when applicable.
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

// Prints out a file, replacing colors with the selected colors.
// Starts coloring when it encounters ":\n"
void colorFile(ifstream & file, int colorMode) {
	char c;
	int startColoring = 0;
	while (file.get(c)) {
		if (!startColoring) {
			if (c == ':') {
				if (not file.get(c)) {
					cout << ':';
					break;
				} else if (c == '\n' or c == '\r') {
					startColoring = true;
					cout << ':' << c;
				}					
			} else {
				cout << c;
			}
		} else if (c >= '0' and c <= '9') {
			cout << mColored(c - '0',colorMode);
		} else if (c == '\033') {
			// Color code. skip to after 'm'.
			while (c != 'm') {
				if (not file.get(c)) break; // End of file.
			}
		} else {
			cout << c;
		}
	}
	cout << '\n';
}

// The main routine for the solver.
int main(int argc, char ** argv) {
	// Handle args.
	bool colorTest = false; // Show color options and quit.
	bool echoMode = false; // Show input
	bool zoneMode = false; // Show zone boards
	bool graphMode = false; // Show graph
	bool showCount = false; // Show number of iterations.
	bool graphCsMode = false; // Show graph with color matching.
	bool noSolve = false; // Stop before computing solution
	bool graphHistory = false; // Show graphs involved in solution.
	bool drawBorders = false; // Draw borders for graphs.
	bool fileInput = false; // Decides if we are using input from a file.
	ifstream inputFile; // File to use, if applicable.
	bool fileSol = false; // Decides if we are printing a solution from a file.
	ifstream solFile; // File to use, if applicable.
	bool noUserMessage = false; // Skips help message when user manually enters input.
	int minSol = 0; // The cutoff for when to return a solution.
	int maxSol = -1; // The cutoff for solutions to ignore. -1 means no cutoff.
	uint maxTime = 0; // The maximum time. '0' means 'none'
	int colorMode = 3; // The color mode to use.
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
		} else if (arg == "-count") {
			showCount = true;
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
		} else if (arg == "-help" or arg == "--help" or arg == "/?" or arg == "-h") {
			printHelp(false);
			return 0;
		} else if (arg == "-help=d") {
			printHelp(true);
			return 0;
		} else if (isPrefix("-t=",arg)) {
			maxTime = stoi(arg.substr(3));
			if (maxTime == 0) {
				cout << "invalid arg: '" + arg + "'\n";
				cout << "time must be an int greater than 0!\n";
				exit(1);
			}
		} else if (isPrefix("-min=",arg)) {
			minSol = stoi(arg.substr(5));
			if (minSol == 0) {
				cout << "invalid arg: '" + arg + "'\n";
				cout << "minimum must be an int greater than 0!\n";
				exit(1);
			}
		} else if (isPrefix("-max=",arg)) {
			maxSol = stoi(arg.substr(5));
			if (maxSol == 0) {
				cout << "invalid arg: '" + arg + "'\n";
				cout << "maximum must be an int greater than 0!\n";
				exit(1);
			}
			maxSol++; // Since it's an exclusive max, not an inclusive max.
		} else if (isPrefix("-s=",arg)) {
			solFile.open(arg.substr(3));
			if (!solFile.is_open()) {
				cout << "Failed to open solution file: " << arg.substr(3) << "\n";
				exit(1);
			}
			fileSol = true;
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
	
	if (fileSol) {
		colorFile(solFile,colorMode);
		return 0;
	}
	
	if (!noUserMessage) {
		cout << "Please enter a grid of digits from 0-9. This will represent the game board.\n";
		cout << "Spaces and empty lines will be safely ignored.\n";
		cout << "When you are done, enter 'q' to finish entering input.\n\n";
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
		// Quit if 'q' is sent on a blank line.
		if (c == 'q' and row.size() == 0) break;
		// handle non-empty lines
		if (c == '\n' or c == 'q') {
			// Set length of rows, if needed.
			if (rowLen == 0) {rowLen = row.size();}
			// Handle mismatched row lengths.
			if (rowLen != row.size()) {
				cerr << "Bad input: lines are different lengths!\n";
				exit(2);
			}
			board.push_back(row); // save row.
			row = vector<int>(); // clear row.
			// Quit if 'q' sent.
			if (c == 'q') break;
			continue;
		}
		// Handle '-' min and max
		if (c == '-') {
			string token = "-";
			for (int i = 0; i < 4; i++) {
				nextChar(c,false,fileInput,inputFile);
				token += c;
			}
			if (token == "-min=") {
				token = "";
				while (true) {
					nextChar(c,false,fileInput,inputFile);
					if (c == '\n') break;
					token += c;
				}
				if (minSol == 0) {
					minSol = stoi(token);
				} else if (stoi(token) != minSol) {
					cout << "ignored '-min=" << stoi(token) << "' in file, used value from args (" << minSol << ") instead.\n";
				}
			} else if (token == "-max=") {
				token = "";
				while (true) {
					nextChar(c,false,fileInput,inputFile);
					if (c == '\n') break;
					token += c;
				}
				if (maxSol == -1) {
					maxSol = stoi(token) + 1;
				} else if (stoi(token) + 1 != maxSol) {
					cout << "ignored '-max=" << stoi(token) << "' in file, used value from args (" << maxSol-1 << ") instead.\n";
				}
			} else {
				cerr << "Bad input: '-' not part of '-min=<int>' or '-max=<int>'\n";
				exit(2);
			}
			continue;
		}
		if (c == '#') { // A comment.
			while (c != '\n') nextChar(c,false,fileInput,inputFile);
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
	
	// Quit if solution not required.
	if (noSolve) return 0;
	
	
	// Generate solution (via solver.cpp)
	vector<vector<vector<int>>> sequence;
	vector<graph> gHistory;
	uint loopCount = 0;
	if (showCount) loopCount = 1;
	bool perfect = solve(startingGraph,zoneBoard,sequence,gHistory,maxTime,loopCount,minSol,maxSol);
	
	if (sequence.size() > 0) {
		// Print results.
		if (minSol > 0 and (unsigned)minSol == sequence.size()-1) {
			cout << "A solution in " << minSol << " moves or fewer:\n\n";
		} else if (perfect) {
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
	} else {
		cout << "No solution found.\n";
	}
	if (showCount) {
		cout << "Number of board states processed: " + to_string(loopCount) + "\n";
		cout << "Number of moves: " << sequence.size()-1 << "\n";
	}
	
	return 0;
}

