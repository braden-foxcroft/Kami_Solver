

#include <vector>
#include <iostream>


using namespace std;

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
	
	
	return 0;
}

