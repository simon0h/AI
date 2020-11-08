// Simon Oh
// ho587
// 11 puzzle problem

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
using namespace std;

bool debug = false;

struct Node {
	vector<vector<int>> state; // How the state is
	vector<int> h; // Stores ALL of the h(n) values of its ancestors
	int level;
	bool leaf = true;
	vector<int> prevStates; // Stores all the swaps that have been made before
	// Ex - if three swaps of L, R and U were made, prevStates would store
	// R, L and D because to "go back" to the previous states, these moves would have to be made
	bool possibleState;
};

int getManhattanDistance(int tile, vector<vector<int>>& goalState, int x, int y) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			if (goalState[i][j] == tile) {
				return (abs(i - x) + abs(j - y));
			}	
		}
	}
	return -1;
}

tuple<int, int> getEmptyTile(vector<vector<int>>& initialState) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			if (initialState[i][j] == 0) {
				return {i, j};
			}	
		}
	}
	return {-1, -1};
}

vector<vector<int>> getNeighbors(vector<vector<int>>& initialState, int x, int y) {
	// Returns a 2D vector containing vectors of x and y positions of the neighbors
	vector<vector<int>> neighbors {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
	neighbors[0] = {x, y + 1}; //Right
	neighbors[1] = {x + 1, y}; //Bottom
	neighbors[2] = {x, y - 1}; //Left
	neighbors[3] = {x - 1, y}; //Top
	return neighbors;
}

bool swap(vector<vector<int>>& state, tuple<int, int> first, vector<int> second) {
	// Swaps the tiles in position first and second
	// If they are unable to be swapped (because the position to swap to is out of bounds),
	// returns false
	int x1 = get<0>(first);
	int y1 = get<1>(first);
	int x2 = second[0];
	int y2 = second[1];
	if (x1 > -1 and x1 < 3 and y1 > -1 and y1 < 4 and x2 > -1 and x2 < 3 and y2 > -1 and y2 < 4) {
		int a = state[x1][y1];
		int b = state[x2][y2];
		state[x1][y1] = b;
		state[x2][y2] = a;
		return true;
	}
	return false;
}

int h(vector<vector<int>>& currState, vector<vector<int>>& goalState) {
	int manhattanDistanceSum = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			manhattanDistanceSum += getManhattanDistance(currState[i][j], goalState, i, j);
		}
	}
	return manhattanDistanceSum;
}

struct Node generateChild(struct Node& parent, int direction, vector<vector<int>>& goalState) {
	struct Node child;
	vector<vector<int>> childState = parent.state;
	tuple<int, int> empty = getEmptyTile(parent.state); // Gets the position of 0 in the parent node's state
	vector<vector<int>> neighbors = getNeighbors(parent.state, get<0>(empty), get<1>(empty));
	child.possibleState = swap(childState, empty, neighbors[direction]);
	if (child.possibleState) {
		child.state = childState;
		child.h = parent.h; // Copy over all values stored in the parent's h variable
		child.h.push_back(h(childState, goalState)); // And store the current h(n) value
		child.level = parent.level + 1;
		child.leaf = true;
		child.prevStates = parent.prevStates;
		if (direction == 0) { // Stores the opposite of the swap that has been made
			child.prevStates.push_back(2);
		}
		else if (direction == 1) {
			child.prevStates.push_back(3);
		}
		else if (direction == 2) {
			child.prevStates.push_back(0);
		}
		else {
			child.prevStates.push_back(1);
		}
	}
	return child;
}

bool notDuplicate(vector<Node>& allPossibleNodes, struct Node toCheck) {
	for (int i = 0; i < allPossibleNodes.size(); i++) {
		if (allPossibleNodes[i].state == toCheck.state) {
			return false;
		}
	}
	return true;
}

tuple<struct Node, int> solvePuzzle(vector<vector<int>>& initialState, vector<vector<int>>& goalState) {
	vector<Node> allPossibleNodes; // Array of nodes that stores all nodes that have been expanded
	vector<Node> currentNodes; // Array of nodes that stores all nodes that have been expanded IN ONE CYCLE
	struct Node rootNode;
	rootNode.state = initialState;
	rootNode.h.push_back(h(initialState, goalState));
	rootNode.level = 0;
	rootNode.prevStates = {-1};
	rootNode.leaf = true;
	allPossibleNodes.push_back(rootNode);
	int smallestH = 0;
	bool found = false;
	struct Node goalNode;
	while (!found) {
		for (int i = 0; i < allPossibleNodes.size(); i++) {
			// Looping through all the nodes that have been expanded so far
			struct Node currNode = allPossibleNodes[i];
			if (debug) {
				cout << "h is: " << currNode.h.back() << "\n";
				if (currNode.leaf) {
					cout << "Is leaf" << "\n";
				}
				else {
					cout << "Not leaf" << "\n";
				}
				for (int k = 0; k < 3; k++) {
					for (int l = 0; l < 4; l++) {
						cout << currNode.state[k][l] << " ";
					}
					cout << "\n";
				}
				cout << "\n";
			}
			if (currNode.leaf and currNode.h.back() == smallestH) {
				// Checking to see that the node we are about to expand is a leaf node
				// and has the smallest f(n) value
				for (int j = 0; j < 4; j++) {
					if (currNode.prevStates.back() != j) {
						struct Node childNode = generateChild(currNode, j, goalState);
						if (childNode.possibleState) {
							// If the child node generated is in a state that the parent can be
							// This will be false if the state change generated is out of the board
							// Ex - if the 0 is at (0, 0), possible state will be false if the 
							// state generated is (-1, 0) or (0, -1)
							currentNodes.push_back(childNode);
							allPossibleNodes[i].leaf = false;
							if (childNode.state == goalState) {
								found = true;
								goalNode = childNode;
							}
							if (debug) {
								cout << "   Adding:\n";
								cout << "   h:" << childNode.h.back() << "\n";
								if (childNode.leaf) {
									cout << "   Is leaf" << "\n";
								}
								else {
									cout << "   Not leaf" << "\n";
								}
								for (int k = 0; k < 3; k++) {
									cout << "   ";
									for (int l = 0; l < 4; l++) {
										cout << childNode.state[k][l] << " ";
									}
									cout << "\n";
								}
								cout << "\n";
							}
						}
					}
				}
			}
		}
		for (int i = 0; i < currentNodes.size(); i++) {
			if (notDuplicate(allPossibleNodes, currentNodes[i])) {
				// If the node generated is not a repeat of any nodes that have been previously generated
				allPossibleNodes.push_back(currentNodes[i]);
			}
		}
		for (int i = 0; i < allPossibleNodes.size(); i ++) {
			if (allPossibleNodes[i].leaf) {
				// Sets the new lowest h(n) as the first leaf that is in the vector
				smallestH = allPossibleNodes[i].h.back();
				if (debug) {
					cout << "\nNew smallestH: " << smallestH << "\n";
				}
				break;
			}
		}
		for (int i = 0; i < allPossibleNodes.size(); i ++) {
			if (allPossibleNodes[i].h.back() < smallestH and allPossibleNodes[i].leaf) {
				// Loops through all leaves in the vector to find the actual smallet h(n)
				smallestH = allPossibleNodes[i].h.back();
				if (debug) {
					cout << "\nNew smallestH: " << smallestH << "\n";
				}
			}
		}
		if (debug) {
			cout << "\nActual smallestH: " << smallestH << "\n";
		}
		currentNodes = {};
	}
	if (debug) {
	 	cout << "Levels: " << goalNode.level << "\n\n";
		for (int k = 0; k < 3; k++) {
			for (int l = 0; l < 4; l++) {
				cout << goalNode.state[k][l] << " ";
			}
			cout << "\n";
		}
		cout << "\n";
		for (int k = 1; k < goalNode.prevStates.size(); k++) {
			int move = goalNode.prevStates[k];
			if (move == 0) {
				cout << "Left\n";
			}
			else if (move == 1) {
				cout << "Up\n";
			}
			else if (move == 2) {
				cout << "Right\n";
			}
			else {
				cout << "Down\n";
			}
		}
		cout << "\n";
		for (int k = 0; k < goalNode.h.size(); k++) {
			cout << goalNode.h[k] << " ";
		}
		cout << "\n";
	}
	return {goalNode, allPossibleNodes.size()};
}

tuple <vector<vector<int>>, vector<vector<int>>> readFromFile(ifstream & inputFile, string fileName) {
	// Reads every character in the input file and returns a tuple containing
	// two 2D arrays, initialState and goalState
	vector<vector<int>> initialState {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
  	vector<vector<int>> goalState {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}; 
  	vector<int> allTiles;
	int tile;
	int tileIterator = 0;
  	while (inputFile >> tile) {
  		allTiles.push_back(tile); // Puts all elements in input file into an array
  	}
  	for (int i = 0; i < 2; i++) { // for loop for putting the tiles in initial and goal state arrays
  		for (int j = 0; j < 3; j++) {
  			for (int k = 0; k < 4; k++) {
  				if (i == 0) {
  					initialState[j][k] = allTiles[tileIterator];
  				}
  				else {
  					goalState[j][k] = allTiles[tileIterator];
  				}
  				tileIterator += 1;
  			}
  		}
  	}
  	inputFile.close();
  	return {initialState, goalState};
}

void writeToFile(string outputFileName, tuple<vector<vector<int>>, vector<vector<int>>> states, tuple<struct Node, int>& solution) {
	ofstream outputFile(outputFileName);
	vector<vector<int>> initialState {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
  	vector<vector<int>> goalState {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
  	initialState = get<0>(states);
	goalState = get<1>(states);
	struct Node goalNode = get<0>(solution);
	int allNodes = get<1>(solution) + 1;
	for (int i = 0; i < 2; i++) {
	  	for (int j = 0; j < 3; j++) {
	  		for (int k = 0; k < 4; k++) {
	  			if (i == 0) {
	  				outputFile << initialState[j][k] << " ";
	  			}
	  			else {
	  				outputFile << goalState[j][k] << " ";
	  			}
	  		}
	  		outputFile << "\n";
	  	}
	  	outputFile << "\n";
	}
	outputFile << goalNode.level << "\n";
	outputFile << allNodes << "\n";
	for (int i = 1; i < goalNode.prevStates.size(); i++) {
		int move = goalNode.prevStates[i];
		if (move == 0) {
			outputFile << "L ";
		}
		else if (move == 1) {
			outputFile << "U ";
		}
		else if (move == 2) {
			outputFile << "R ";
		}
		else {
			outputFile << "D ";
		}
	}
	outputFile << "\n";
	for (int i = 0; i < goalNode.h.size(); i++) {
		outputFile << goalNode.h[i] + 1 << " "; 
		// This is for writing the f(n) costs. I am adding one to all h(n) values because 
		// since we were not given the value of g(n), 
		// I assumed that g(n) is one for every move, meaning f(n) = h(n) + 1
	}
	outputFile << "\n";
	outputFile.close();
}

int main() {
  	vector<vector<int>> initialState {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}; // Initial state of the puzzle, stored as a 2D vector
  	vector<vector<int>> goalState {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}; // Goal state of the ouzzle, stored as a 2D vector
  	tuple<vector<vector<int>>, vector<vector<int>>> states;
  	tuple<struct Node, int> solution;
  	string inputFileName;
  	string outputFileName;
  	ifstream inputFile;
  	bool fileOpened = false;
  	while (!fileOpened) {
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
  	cout << "Enter the name of the output file with the filename extension: \n";
  	cin >> outputFileName;
  	states = readFromFile(inputFile, inputFileName); // Returns a tuple that contains initialState and goalState
  	initialState = get<0>(states);
  	goalState = get<1>(states);
  	solution = solvePuzzle(initialState, goalState);
  	writeToFile(outputFileName, states, solution);
  	return 0;
}



