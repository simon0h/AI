// Simon Oh
// ho587
// Hyper Sudoku

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <map>
using namespace std;

void updateWithHNeighbors(map<tuple<int, int>, map<int, int>>& state, int i, int j, int cell) {
// Remove the value passed in as cell from the domains of cells in the same row
	for (int k = 0; k < 9; k++) {
		tuple <int, int> pos(i, k);
		state[pos].erase(cell);
	}
}

void updateWithVNeighbors(map<tuple<int, int>, map<int, int>>& state, int i, int j, int cell) {
// Remove the value passed in as cell from the domains of cells in the same column
	for (int k = 0; k < 9; k++) {
		tuple <int, int> pos(k, j);
		state[pos].erase(cell);
	}
}

void updateWithNotOverlappingNeighbors(map<tuple<int, int>, map<int, int>>& state, int i, int j, int cell) {
// Remove the value passed in as cell from the domains of cells in the same non-overlapping region
	int segment_i = int(i / 3) * 3;
	int segment_j = int(j / 3) * 3;
	for (int k = 0; k < 3; k ++) {
		for (int l = 0; l < 3; l++) {
			tuple <int, int> pos(segment_i + k, segment_j + l);
			state[pos].erase(cell);
		}
	}
}

void updateWithOverlappingNeighbors(map<tuple<int, int>, map<int, int>>& state, int i, int j, int cell) {
// Remove the value passed in as cell from the domains of cells in the same overlapping region
	int segment_i = (int(i / 4) * 4) + 1;
	int segment_j = (int(j / 4) * 4) + 1;
	for (int k = 0; k < 3; k ++) {
		for (int l = 0; l < 3; l++) {
			tuple <int, int> pos(segment_i + k, segment_j + l);
			state[pos].erase(cell);
		}
	}
}

bool inOverlapping(int i, int j) {
	if ((i == 0) or (j == 0) or (i == 4) or (i == 8) or (j == 4) or (j == 8)) {
		return false;
	}
	return true;
}

map<tuple<int, int>, map<int, int>> forwardChecking(map<tuple<int, int>, map<int, int>>& state) {
	map<tuple<int, int>, map<int, int>> copy = state;
	// Loop through all cells and find the cell that has a value assigned 
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			tuple <int, int> pos(i, j); 
			if (copy[pos][-1] != 0) { 
			// If a cell has a value assigned, go through its neighbors and remove the value from the domains of its neighbors
				int cell = copy[pos][-1];
				updateWithHNeighbors(copy, i, j, cell);
				updateWithVNeighbors(copy, i, j, cell);
				updateWithNotOverlappingNeighbors(copy, i, j, cell);
				if (inOverlapping(i, j)) {
					updateWithOverlappingNeighbors(copy, i, j, cell);
				}
			}
		}
	}
	return copy;
}

vector<tuple <int, int>> getVarByMinRemainingValue(map<tuple<int, int>, map<int, int>>& state) {
// Minimum remaining value heurstic
	vector<tuple <int, int>> minRemainingValuesPos;
	int minRemainingValue = 10;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			tuple <int, int> pos(i, j); 
			if ((state[pos].size() - 1) < minRemainingValue and state[pos][-1] == 0) {
				minRemainingValuesPos.clear();
				minRemainingValuesPos.push_back(pos);
				minRemainingValue = state[pos].size() - 1;
			}
			else if ((state[pos].size() - 1) == minRemainingValue and state[pos][-1] == 0) {
				minRemainingValuesPos.push_back(pos);
			}
		}
	}
	return minRemainingValuesPos;
}

int getHUnassignedNeighborNum(map<tuple<int, int>, map<int, int>>& state, tuple <int, int> pos) {
// Checking how many cells in the same row are unassigned
	int unassignedVarNum = 0;
	int i = get<0>(pos);
	int j = get<1>(pos);
	for (int k = 0; k < 9; k++) {
		tuple <int, int> currPos(k, j);
		if (state[currPos][-1] == 0 and k != i) {
			unassignedVarNum++;
		}
	}
	return unassignedVarNum;
}

int getVUnassignedNeighborNum(map<tuple<int, int>, map<int, int>>& state, tuple <int, int> pos) {
// Checking how many cells in the same column are unassigned
	int unassignedVarNum = 0;
	int i = get<0>(pos);
	int j = get<1>(pos);
	for (int k = 0; k < 9; k++) {
		tuple <int, int> currPos(i, k);
		if (state[currPos][-1] == 0 and k != j) {
			unassignedVarNum++;
		}
	}
	return unassignedVarNum;
}

int getNotOverlapUnassignedNeighborNum(map<tuple<int, int>, map<int, int>>& state, tuple <int, int> pos) {
// Checking how many cells in the same non-overlapping region are unassigned
	int unassignedVarNum = 0;
	int i = get<0>(pos);
	int j = get<1>(pos);
	int segment_i = int(i / 3) * 3;
	int segment_j = int(j / 3) * 3;
	for (int k = 0; k < 3; k ++) {
		for (int l = 0; l < 3; l++) {
			tuple <int, int> currPos(segment_i + k, segment_j + l);
			if (state[currPos][-1] == 0 and k != i and l != j) {
				unassignedVarNum++;
			}
		}
	}
	return unassignedVarNum;
}

int getOverlapUnassignedNeighborNum(map<tuple<int, int>, map<int, int>>& state, tuple <int, int> pos) {
// Checking how many cells in the same overlapping region are unassigned
	int unassignedVarNum = 0;
	int i = get<0>(pos);
	int j = get<1>(pos);
	int segment_i = (int(i / 4) * 4) + 1;
	int segment_j = (int(j / 4) * 4) + 1;
	for (int k = 0; k < 3; k ++) {
		for (int l = 0; l < 3; l++) {
			tuple <int, int> currPos(segment_i + k, segment_j + l);
			if (state[currPos][-1] == 0 and k != i and l != j) {
				unassignedVarNum++;
			}
		}
	}
	return unassignedVarNum;
}

bool getVarByDegreeDebug = false;
vector<tuple <int, int>> getVarByDegree(map<tuple<int, int>, map<int, int>>& state, vector<tuple <int, int>>& minRemainingValuesPos) {
// Degree heurstic
	vector<tuple <int, int>> newMRVP;
	int largestDegree = 0;
	for (int k = 0; k < minRemainingValuesPos.size(); k++) {
		tuple<int, int> pos = minRemainingValuesPos[k];
		int h = getHUnassignedNeighborNum(state, pos);
		int v = getVUnassignedNeighborNum(state, pos);
		int nonOverlapping = getNotOverlapUnassignedNeighborNum(state, pos);
		int i = get<0>(pos);
		int j = get<1>(pos);
		int overlapping = 0;
		if (inOverlapping(i, j)) {
			overlapping = getOverlapUnassignedNeighborNum(state, pos);
		}
		int degreeSum = h + v + nonOverlapping + overlapping;
		if (getVarByDegreeDebug) {
			cout << "Degree for position (" << i << ", " << j << "): " << degreeSum << "\n";
		}
		if (degreeSum > largestDegree) {
			newMRVP.clear();
			newMRVP.push_back(pos);
			largestDegree = degreeSum;
		}
		else if (degreeSum == largestDegree) {
			newMRVP.push_back(pos);
		}
	}
	if (getVarByDegreeDebug) {
		cout << "getVarByDegree result:\n";
		for (tuple<int, int> t: newMRVP) {
			cout << get<0>(t) << ", " << get<1>(t) << "\n";
		}
	}
	return newMRVP;
}

vector<tuple <int, int>> getRandomVar(map<tuple <int, int>, map <int, int>>& state, vector<tuple <int, int>>& minRemainingValuesPos) {
// Arbitrarily choosing the next cell if the two heursitics fail
	vector<tuple <int, int>> newMRVP;
	for (int k = 0; k < minRemainingValuesPos.size(); k++) {
		tuple <int, int> pos = minRemainingValuesPos[k];
		if ((state[pos].size() - 1) > 0) {
			newMRVP.push_back(minRemainingValuesPos[k]);
			return newMRVP;
		}
	}
	return newMRVP;
}

vector<tuple <int, int>> selectUnassignedVariable(map<tuple <int, int>, map <int, int>>& state) {
	vector<tuple <int, int>> minRemainingValuesPos = getVarByMinRemainingValue(state);
	if (minRemainingValuesPos.size() > 1) {
		minRemainingValuesPos = getVarByDegree(state, minRemainingValuesPos);
	}
	if (minRemainingValuesPos.size() > 1) {
		minRemainingValuesPos = getRandomVar(state, minRemainingValuesPos);
	}
	return minRemainingValuesPos;
}

bool successH(map<tuple <int, int>, map <int, int>>& state, int i) { 
// Checks that all cells in a row have different values
// Loops through a row, stores the values in the row in an array and if a same value is found in the array, returns false
	vector<int> row {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	for (int j = 0; j < 9; j++) {
		tuple <int, int> pos (i, j);
		if (row[state[pos][-1]] != 0) {
			return false;
		}
		row[state[pos][-1]] = 1;
	}
	return true;
}

bool successV(map<tuple <int, int>, map <int, int>>& state, int j) {
// Checks that all cells in a column have different values
// Loops through a column, stores the values in the column in an array and if a same value is found in the array, returns false
	vector<int> row {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	for (int i = 0; i < 9; i++) {
		tuple <int, int> pos (i, j);
		if (row[state[pos][-1]] != 0) {
			return false;
		}
		row[state[pos][-1]] = 1;
	}
	return true;
}

bool successNotOverlapping(map<tuple <int, int>, map <int, int>>& state, int i, int j) {
// Checks that all cells in a non-overlapping region have different values
// Loops through a section, stores the values in the section in a map and if a same value is found in the map, returns false
	map<int, int> section;
	int segment_i = int(i / 3) * 3;
	int segment_j = int(j / 3) * 3;
	for (int k = 0; k < 3; k ++) {
		for (int l = 0; l < 3; l++) {
			tuple <int, int> pos(segment_i + k, segment_j + l);
			if (section[state[pos][-1]] != 0) { // Accessing the assigned value. 0 means unassigned.
				return false;
			}
			section[state[pos][-1]] = 1;
		}
	}
	return true;
}

bool successOverlapping(map<tuple <int, int>, map <int, int>>& state, int i, int j) {
// Checks that all cells in an overlapping region have different values
// Loops through a section, stores the values in the section in a map and if a same value is found in the map, returns false
	map<int, int> section;
	int segment_i = (int(i / 4) * 4) + 1;
	int segment_j = (int(j / 4) * 4) + 1;
	for (int k = 0; k < 3; k ++) {
		for (int l = 0; l < 3; l++) {
			tuple <int, int> pos(segment_i + k, segment_j + l);
			if (section[state[pos][-1]] != 0) { // Accessing the assigned value. 0 means unassigned.
				return false;
			}
			section[state[pos][-1]] = 1;
		}
	}
	return true;
}

bool success(map<tuple <int, int>, map <int, int>>& state) {
// Loops through every cell and returns true if all the values are assigned with the correct assignment, false otherwise
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			bool success = false;
			success = successH(state, i);
			success = successV(state, j);
			success = successNotOverlapping(state, i, j);
			if (inOverlapping(i, j)) {
				success = successOverlapping(state, i, j);
			}
			if (success == false) {
				return false;
			}

		}
	}
	return true;
}

bool assignmentComplete(map<tuple<int, int>, map<int, int>>& state) {
// Loops through every cell and returns true if all of the values are assigned, false otherwise
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			tuple <int, int> pos (i, j);
			if (state[pos][-1] == 0) { // Accessing the assigned value. 0 means unassigned.
				return false;
			}
		}
	}
	return true;
}

bool backtrackDebug = false;
map<tuple<int, int>, map<int, int>> backtrack(map<tuple <int, int>, map <int, int>>& state) {
	map<tuple<int, int>, map<int, int>> copy = forwardChecking(state); // Forward checking the cells at the beginning of every backtrack call
	if (backtrackDebug) { // For debugging purposes
		cout << "Board: \n";
		for (int i = 0; i < 9; i ++) {
			for (int j = 0; j < 9; j++) {
				tuple <int, int> pos(i, j); 
				map<int, int> m = copy[pos];
				cout << m[-1] << " ";
			}
			cout << "\n";
		}
		for (int k = 0; k < 9; k++) {
			cout << "In domain for line " << k << ": \n";
			for (int i = 0; i < 9; i++) {
				tuple <int, int> pos(k, i); 
				cout << copy[pos][-1] << ": ";
				for (int j = 0; j < 10; j++) {
					if (copy[pos][j] != 0) {
						cout << j << " ";
					}
					else {
						copy[pos].erase(j);
					}
				}
				cout << "| size: " << copy[pos].size();
				cout << "\n";
			}
		}
		if (assignmentComplete(copy)) {
			cout << "Complete\n";
		}
		cout << "-------------------\n\n";
	}
	if (assignmentComplete(copy)) { // if assignment is complete then return asignment
		return copy;
	}
	else {
		vector<tuple <int, int>> minRemainingValuesPos = selectUnassignedVariable(copy);
		map<tuple <int, int>, map <int, int>> result;
		tuple <int, int> pos = minRemainingValuesPos[0];
		for (int i = 0; i < 10; i++) {
		// This is the same as the "for each value in order domain values" in backtracking
		// However, instead of getting the domain of a cell and looping through that
		// It loops through 0-9, all possible domains, and if that value is not available in
		// the cell's domain, is not assigned 
			if (copy[pos][i] > 0) { // Checking if a possible assignment is in a cell's domain
				copy[pos][-1] = i;
				result = backtrack(copy);
				if (success(result)) {
					return result;
				}
				copy[pos][-1] = 0;
			}
			else {
				copy[pos].erase(i);
			}
		}
		return result;
	}
}

map<tuple<int, int>, map<int, int>> hyperSudoku(map<tuple<int, int>, map<int, int>>& state) {
	map<tuple<int, int>, map<int, int>> fc = forwardChecking(state);
	fc = backtrack(fc);
	return fc;
}

int main() {
	map<tuple<int, int>, map<int, int>> initialState;
	// The game board is a map that has a key value of a tuple and mapped value of a map
	// The tuple is for getting the cell by the position
	// For example, tuple (0, 0) would get the cell in the top left corner of the sudoku board
	// The map<int, int> is for storing the domain of a cell
	// In the map, -1 is used to represent the assigned value
	// For example, if a cell has a value of 0 in map[-1], then it is not assigned a value
	// To get the domain of a cell, loop through the map and see which assignment returns 1
	// For example, if a cell has 1, 3, 5 in its domain, then map[1], map[3] and map[5] would return 1
	// for all other values, it would return 0, because if you try to access values in a map that does not exist, it returns a 0
	string inputFileName;
  	string outputFileName;
  	ifstream inputFile;
  	bool fileOpened = false;
  	int cell;
     while (!fileOpened) { // For checking if the file name exists
	  	cout << "Enter the name of the input file with the filename extension with matching cases \n";
	  	cout << "Example: File.txt instead of file or file.txt \n"; 
	  	cin >> inputFileName;
	  	inputFile.open(inputFileName);
	  	if (inputFile.fail()) {
	  		cout << "Exception opening/reading file " << inputFileName << "\n";
	  	}
	  	else {
	  		fileOpened = true;
	  	}
	}
  	int i = 0;
  	int j = 0;
  	while (inputFile >> cell) {
  		map<int, int> domain;
  		for (int k = 1; k < 10; k++) {
  			domain[k] = 1; // Assign 1 to all values in a domain because all values have domain of 1-9
  		}
  		tuple <int, int> pos(i, j); 
  		domain[-1] = cell;
  		initialState[pos] = domain;
  		j++;
  		if (j == 9) {
  			j = 0;
  			i++;
  		}
  	}
  	inputFile.close();
	map<tuple<int, int>, map<int, int>> solution = hyperSudoku(initialState); // Run the hypersudoku code
	cout << "Enter the name of the output file with the filename extension: \n";
  	cin >> outputFileName;
  	ofstream outputFile(outputFileName);
	for (int i = 0; i < 9; i ++) { // For writing to a given file name
		for (int j = 0; j < 9; j++) {
			tuple <int, int> pos(i, j); 
			map<int, int> m = solution[pos];
			outputFile << m[-1] << " ";
		}
		outputFile << "\n";
	}
	outputFile.close();
	return 0;
}

