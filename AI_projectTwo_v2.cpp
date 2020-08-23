//============================================================================
// Name        : AI_projectTwo_v2.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Project Two for Artificial Intelligence
//============================================================================

#include <iostream>
#include <fstream>
#include <queue>
#include <string>
using namespace std;

//Class which represents a variable
class variable{
public:
	int domain[5] = {0,0,0,0,0};
	vector <int> usedList;
	char leftInequality = 'm';
	char rightInequality = 'm';
	char upInequality = 'm';
	char downInequality = 'm';

	//Default constructor
	variable(){

	}

	//Constructor where a string is passed in
	variable(char inStr){
		if(inStr == '0'){
			for(int index = 0; index < 5; index++){
				this->domain[index] = (index + 1);
			}
		}
		else if(inStr == '1' || inStr == '2' || inStr == '3' || inStr == '4' || inStr == '5'){
			for(int index = 0; index < 5; index++){
				if((inStr - '0') == (index + 1)){
					this->domain[index] = (inStr - '0');
				}
				else{
					this->domain[index] = 0;
				}
			}
		}

	}
	//Deconstructor method
	~variable(){

	}

	//Get size of the domain
	//The size of the domain is the number of values in the domain which are not 0
	int sizeOfDomain(){
		int count = 0;
		for(int i = 0; i < 5; i++){
			if(this->domain[i] != 0){
				count++;
			}
		}
		return count;
	}

	int getSingularValue(){
		for(int i = 0; i < 5; i++){
			if(this->domain[i] != 0){
				return this->domain[i];
			}
		}
	}

	int reduceDomain(int inVal, char lessGreater){
		int numOfChanges = 0;
		//Keep only values GREATER THAN the value
		if(lessGreater == 'g'){
			for(int i = 0; i < 5; i++){
				if(this->domain[i] <= inVal && this->domain[i] != 0){
					this->domain[i] = 0;
					numOfChanges++;
				}
			}
		}
		//Keep only values LESS THAN the value
		else if(lessGreater == 'l'){
			for(int i = 0; i < 5; i++){
				if(this->domain[i] >= inVal && this->domain[i] != 0){
					this->domain[i] = 0;
					numOfChanges++;
				}
			}
		}
		return numOfChanges;
	}
};

//Formulate information array with the string
//Create a multi-dimensional 5x5 array of objects
//Each object contains the domain of each variable, as well as the inequality
//relationship to the variable to it's right, left, above and below variable
const int rows = 5;
const int cols = 5;
variable storage[rows][cols];

//Method for processing the input file
string inputProcessing(string inFileName){
	//Accept a file name as input
	fstream inputFile(inFileName);
	string outputStr = "";
	string curLine;
	//Open the file, and parse the data in the file into a string
	if(inputFile.is_open()){
		while(getline(inputFile, curLine)){
			for(int i = 0; i < curLine.size(); i++){
				if(isdigit(curLine[i]) || curLine[i] == '<' || curLine[i] == '>'
						|| curLine[i] == '^' || curLine[i] == 'v'){
					outputStr += curLine[i];
				}
				else if(curLine[i] == ' '){
					outputStr += ',';
				}
			}
			outputStr += ';';
		}
	}
	//Final processing to make sure the string is easy to interpret in the algorithm
	string outputFinal = "";
	outputFinal += outputStr[0];
	for(int i = 1; i < outputStr.size(); i++){
		if(!(outputStr[i] == ';' && outputStr[i - 1] == ';')){
			outputFinal += outputStr[i];
		}
	}
	if(outputFinal[outputFinal.size() - 2] == ','){
		outputFinal = outputFinal.substr(0, outputFinal.size() - 1);
		outputFinal[outputFinal.size() - 1] = ';';
	}
	return outputFinal;
}

string backtrack(string inAssignment){
	string oldAssignment = inAssignment;
	//If assignment is complete, return it
	int segCount = 0;
	for(int i = 0; i < inAssignment.size(); i++){
		if(inAssignment[i] == ';'){
			segCount++;
		}
	}
	if(segCount == 25){
		return inAssignment;
	}

	//Sets up assignment into a usable array
	int tempArr[5][5];
	for(int i = 0; i < 25; i++){
		tempArr[i/5][i%5] = 0;
	}
	int tempPrev = 0;
	int tempEnd = 0;
	for(int i = 0; i < inAssignment.size(); i++){
		if(inAssignment[i] == ';'){
			tempEnd = i;
			string tempSub = inAssignment.substr(tempPrev, (tempEnd-tempPrev));
			for(int j = 0; j < tempSub.size(); j++){
				if(tempSub[j] == ','){
					string subSub = tempSub.substr(0, j);
					int subSubInt = stoi(subSub);
					string endSubSub = tempSub.substr(j + 1, (tempSub.size() - (j + 1)));
					int endSubSubInt = stoi(endSubSub);
					tempArr[subSubInt/5][subSubInt%5] = endSubSubInt;
				}
			}
			tempPrev = i + 1;
		}
	}

	//Select next variable
	//Minimum Remaining Value heuristic
	string smallStore = "";
	int smallest = 6;
	int mrvCount = 0;
	for(int i = 0; i < 25; i++){
		int available = storage[i/5][i%5].sizeOfDomain();
		if(available < smallest && available > 1){
			smallest = available;
			smallStore += to_string(i) + ',';
			mrvCount++;
		}
	}

	//Degree heuristic, if MRV gives more than one variable
	int returnInt;
	if(mrvCount > 1){
		int prev = 0;
		int end = 0;
		int curIndex = -1;
		int mostNeighbors = -1;
		for(int i = 0; i < smallStore.size(); i++){
			if(smallStore[i] == ','){
				end = i;
				string sub = smallStore.substr(prev, (end-prev));
				int myIndex = stoi(sub);

				//Check all of the indices neighbors
				int unassignedNeighbors = 0;
				if((myIndex/5) > 0 && storage[myIndex/5 - 1][myIndex%5].sizeOfDomain() > 1){
					unassignedNeighbors++;
				}
				if((myIndex/5) < 4 && storage[myIndex/5 + 1][myIndex%5].sizeOfDomain() > 1){
					unassignedNeighbors++;
				}
				if((myIndex%5) > 0 && storage[myIndex/5][myIndex%5 - 1].sizeOfDomain() > 1){
					unassignedNeighbors++;
				}
				if((myIndex%5) < 4 && storage[myIndex/5][myIndex%5 + 1].sizeOfDomain() > 1){
					unassignedNeighbors++;
				}
				if(unassignedNeighbors > mostNeighbors){
					curIndex = myIndex;
					mostNeighbors = unassignedNeighbors;
				}
				prev = i + 1;
			}
		}
		returnInt = curIndex;
	}
	else{
		std::size_t strLoc = smallStore.find(',');
		string locStr = smallStore.substr(0, strLoc);
		returnInt = stoi(locStr);
	}

	//For each value in the domain
	for(int i = 0; i < 5; i++){
		if(storage[returnInt/5][returnInt%5].domain[i] != 0){
			int possibleVal = storage[returnInt/5][returnInt%5].domain[i];
			//If value is consistent with assignment
			bool isConsistent = true;
			//Check horizontally
			for(int j = 0; j < 5; j++){
				if(storage[returnInt/5][j].sizeOfDomain() == 1
						&& storage[returnInt/5][j].getSingularValue() == possibleVal){
					isConsistent = false;
				}
			}
			//Check vertically
			for(int j = 0; j < 5; j++){
				if(storage[j][returnInt%5].sizeOfDomain() == 1
						&& storage[j][returnInt%5].getSingularValue() == possibleVal){
					isConsistent = false;
				}
			}
			//Check inequalities
			if(returnInt/5 > 0 && storage[(returnInt/5) - 1][returnInt%5].sizeOfDomain() == 1){
				if(storage[returnInt/5][returnInt%5].upInequality == 'v' && storage[(returnInt/5) - 1][returnInt%5].getSingularValue() < possibleVal){
					isConsistent = false;
				}
				else if(storage[returnInt/5][returnInt%5].upInequality == '^' && storage[(returnInt/5) - 1][returnInt%5].getSingularValue() > possibleVal){
					isConsistent = false;
				}
			}
			if(returnInt/5 < 4 && storage[(returnInt/5) + 1][returnInt%5].sizeOfDomain() == 1){
				if(storage[returnInt/5][returnInt%5].downInequality == 'v' && storage[(returnInt/5) + 1][returnInt%5].getSingularValue() < possibleVal){
					isConsistent = false;
				}
				else if(storage[returnInt/5][returnInt%5].downInequality == '^' && storage[(returnInt/5) + 1][returnInt%5].getSingularValue() > possibleVal){
					isConsistent = false;
				}
			}
			if(returnInt%5 > 0 && storage[returnInt/5][returnInt%5 - 1].sizeOfDomain() == 1){
				if(storage[returnInt/5][returnInt%5].leftInequality == '>' && storage[returnInt/5][returnInt%5 - 1].getSingularValue() < possibleVal){
					isConsistent = false;
				}
				else if(storage[returnInt/5][returnInt%5].leftInequality == '<' && storage[returnInt/5][returnInt%5 - 1].getSingularValue() > possibleVal){
					isConsistent = false;
				}
			}
			if(returnInt%5 < 4 && storage[returnInt/5][returnInt%5 + 1].sizeOfDomain() == 1){
				if(storage[returnInt/5][returnInt%5].rightInequality == '>' && storage[returnInt/5][returnInt%5 + 1].getSingularValue() < possibleVal){
					isConsistent = false;
				}
				else if(storage[returnInt/5][returnInt%5].rightInequality == '<' && storage[returnInt/5][returnInt%5 + 1].getSingularValue() > possibleVal){
					isConsistent = false;
				}
			}

			if(isConsistent){
				//Remove the values that are not the chosen value from the variable's domain, and store them in case they're needed later
				int pushCount = 0;
				for(int j = 0; j < 5; j++){
					if(storage[returnInt/5][returnInt%5].domain[j] != possibleVal){
						storage[returnInt/5][returnInt%5].usedList.push_back(storage[returnInt/5][returnInt%5].domain[j]);
						storage[returnInt/5][returnInt%5].domain[j] = 0;
						pushCount++;
					}
				}
				//Recursively assign the other variables
				string result = backtrack(inAssignment + to_string(returnInt) + ',' + to_string(possibleVal) + ';');
				if(result.compare("FAILURE") != 0){
					return result;
				}

				//Remove last assignment
				while(!storage[returnInt/5][returnInt%5].usedList.empty() && pushCount > 0){
					int oldVal = storage[returnInt/5][returnInt%5].usedList.back();
					storage[returnInt/5][returnInt%5].usedList.pop_back();
					for(int j = 0; j < 5; j++){
						if(j == (oldVal - 1)){
							storage[returnInt/5][returnInt%5].domain[j] = oldVal;
						}
					}
					pushCount--;
				}
			}

		}
	}
	//If the puzzle is unsolvable, return that failure has been achieved
	return "FAILURE";

}

int main(int argc, char* argv[]){
	//Process file input into a string
	string problem = inputProcessing(argv[1]);

	//Dynamically creates the name of the output file, based on the input file
	string name = argv[1];
	string outputName = "";
	size_t nameIndex = name.find("Input");
	if(nameIndex == string::npos){
		outputName = "programOutput.txt";
	}
	else if((static_cast<int>(nameIndex + 5)) != (name.size() - 1)){
		int nameIndexInt = static_cast<int>(nameIndex);
		char num = name[nameIndex + 5];
		outputName = "Output";
		outputName.push_back(num);
		outputName += ".txt";
	}

	//Sets up the global array that will store the domains of all the variables, as well as the inequality relationships
	//between their neighbors
	int count = 0;
	for(int i = 0; i < problem.size(); i++){
		if(count < 25 && problem[i] != ',' && problem[i] != ';'){
			storage[count/5][count%5] = variable(problem[i]);
			count++;
		}
		else if(count >= 25 && count <= 44 && problem[i] != ',' && problem[i] != ';'){
			int horizIterator = count - 25;
			storage[horizIterator/5][horizIterator%5].leftInequality = problem[i];
			storage[horizIterator/5][(horizIterator%5) + 1].rightInequality = problem[i];
			count++;
		}
		else if(count >= 45 && problem[i] != ',' && problem[i] != ';'){
			int vertIterator = count - 45;
			storage[vertIterator/5][vertIterator%5].downInequality = problem[i];
			storage[(vertIterator/5) + 1][vertIterator%5].upInequality = problem[i];
			count++;
		}
	}

	//Forward checking
	int numOfChanges = 0;
	do{
		numOfChanges = 0;
		for(int i = 0; i < 25; i++){
			if(storage[i/5][i%5].sizeOfDomain() == 1){
				int val = storage[i/5][i%5].getSingularValue();
				//Remove the value of the variable from every variable in the row
				for(int j = 0; j < 5; j++){
					if(storage[i/5][j].domain[val - 1] != 0 && j != (i%5)){
						numOfChanges++;
					}
					if(j != (i%5)){
						storage[i/5][j].domain[val - 1] = 0;
					}
				}
				//Remove the value of the variable from every variable in the column
				for(int j = 0; j < 5; j++){
					if(storage[j][i%5].domain[val - 1] != 0 && j != (i/5)){
						numOfChanges++;
					}
					if(j != (i/5)){
						storage[j][i%5].domain[val - 1] = 0;
					}
				}
				//Check inequalities
				//Is there an inequality with the variable above it?
				if(storage[i/5][i%5].upInequality != 'm'){
					if(storage[i/5][i%5].upInequality == 'v' && storage[(i/5) - 1][i%5].sizeOfDomain() > 1){
						numOfChanges += (storage[(i/5)-1][i%5].reduceDomain(val, 'g'));
					}
					else if(storage[i/5][i%5].upInequality == '^' && storage[(i/5) - 1][i%5].sizeOfDomain() > 1){
						numOfChanges += (storage[(i/5)-1][i%5].reduceDomain(val, 'l'));
					}
				}
				//Is there an inequality with the variable below it?
				if(storage[i/5][i%5].downInequality != 'm'){
					if(storage[i/5][i%5].downInequality == 'v' && storage[(i/5) + 1][i%5].sizeOfDomain() > 1){
						numOfChanges += (storage[(i/5) + 1][i%5].reduceDomain(val, 'l'));
					}
					else if(storage[i/5][i%5].downInequality == '^' && storage[(i/5) + 1][i%5].sizeOfDomain() > 1){
						numOfChanges += (storage[(i/5) + 1][i%5].reduceDomain(val, 'g'));
					}
				}
				//Is there an inequality with the variable to the left of it?
				if(storage[i/5][i%5].leftInequality != 'm'){
					if(storage[i/5][i%5].leftInequality == '>' && storage[i/5][(i%5) - 1].sizeOfDomain() > 1){
						numOfChanges += (storage[i/5][(i%5) - 1].reduceDomain(val, 'g'));
					}
					else if(storage[i/5][i%5].leftInequality == '<' && storage[i/5][(i%5) - 1].sizeOfDomain() > 1){
						numOfChanges += (storage[i/5][(i%5) - 1].reduceDomain(val, 'l'));
					}
				}
				//Is there an inequality with the variable to the right of it?
				if(storage[i/5][i%5].rightInequality != 'm'){
					if(storage[i/5][i%5].rightInequality == '>' && storage[i/5][(i%5) + 1].sizeOfDomain() > 1){
						numOfChanges += (storage[i/5][(i%5) + 1].reduceDomain(val, 'l'));
					}
					else if(storage[i/5][i%5].rightInequality == '<' && storage[i/5][(i%5) + 1].sizeOfDomain() > 1){
						numOfChanges += (storage[i/5][(i%5) + 1].reduceDomain(val, 'g'));
					}
				}
			}
		}
	}
	while(numOfChanges > 0);

	//Backtracking
	int assignedCount = 0;
	string assignment = "";

	//Transform the results of forward checking into something usable by the backtracking algorithm
	for(int i = 0; i < 25; i++){
		if(storage[i/5][i%5].sizeOfDomain() == 1){
			int setVal = storage[i/5][i%5].getSingularValue();
			assignment += to_string(i) + ',' + to_string(setVal) + ';';
			assignedCount++;
		}
	}

	//Perform backtracking
	string assign = backtrack(assignment);

	//Create the output file
	ofstream output;
	output.open(outputName);

	if(assign.compare("FAILURE") == 0){
		output << "Failure to complete puzzle" << endl;
	}
	else{
		int finalArray[5][5];
		int finalPrev = 0;
		int finalEnd = 0;
		for(int i = 0; i < assign.size(); i++){
			if(assign[i] == ';'){
				finalEnd = i;
				string finalSub = assign.substr(finalPrev, (finalEnd - finalPrev));
				for(int j = 0; j < finalSub.size(); j++){
					if(finalSub[j] == ','){
						finalArray[stoi(finalSub.substr(0,j))/5][stoi(finalSub.substr(0,j))%5] = stoi(finalSub.substr((j+1), (finalSub.size() - (j + 1))));
					}
				}
				finalPrev = i + 1;
			}
		}
		for(int i = 0; i < 5; i++){
			for(int j = 0; j < 5; j++){
				output << to_string(finalArray[i][j]) << ' ';
			}
			output << endl;
		}
	}
	output.close();
}

