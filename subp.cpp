#include <chrono>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

struct arrr {
string value = "";
arrr* next = nullptr;
size_t size;



void push(const string& newValue) {
	arrr* head = this; 
	arrr* current = this;
	if (current->size == 0){
	current->size = 1;
	current->value = newValue;
	return;}
	current = current->get(current->size-1);
	current->next = new arrr();
	current->next->value = newValue;
	
	if (current->next->next){current->next->next = nullptr;}
	
	head->size++;
	current->next->size = head->size;
	return;
}

string pop() {
	if (!this->next) {
	string poppedValue = this->value;
	this->size = 0; //probably bad
	this->value = "";
	return poppedValue;
	}
	arrr* current = this;
	while (current->next->next) {
	current = current->next;
	}
	string poppedValue = current->next->value;
	//delete current->next;
	current->next = nullptr;
	this->size--;
	return poppedValue;
}


void setSize(int sizze) {
	if (sizze < 0) return;
	if (sizze = 0) {
		//cout<< "SET A SIZE TO 0!!!\n\n\n";
	this->size = sizze;
	this->next = nullptr;
	return;
	}
	this->size = sizze;
	arrr* current = this;
	for (int index = 1; index < sizze; ++index) {
	if (!current->next) {
	current->size = sizze;
	current->next = new arrr();
	current->next->size = sizze;
	}
	current = current->next;
	}
}

arrr* get(size_t index) {
	arrr* current = this;
	if(index==0){return current;}

	if (index>current->size){
		throw out_of_range("OOB!");
	}

	for (size_t i = 0; index > i; i++) {
	if (current->next==nullptr){
		return current;
	}
	current = current->next;
	}
	return current; 
}

void print(){
	if (this->size>4000000){throw runtime_error("Overflow!");}
	for (int j = 0; j < this->size+1; ++j) {
	cout << this->get(j)->value<<" "<<j<<" ";
	}
	return;
	}

};

struct arrr2d {
	arrr* row;
	arrr2d* next;
	size_t numRows;
	size_t numCols;

	void setSize(size_t newRows, size_t newCols) {
		this->numRows = newRows;
		this->numCols = newCols;
		arrr2d* current = this;
		for (size_t i = 0; i < newRows; ++i) {
		current->row = new arrr();
		current->row->setSize(newCols);
		for (size_t j = 0; j < newCols; ++j) {
		current->row->push("");
		}
		if (i < newRows - 1) {
		current->next = new arrr2d();
		current = current->next;
		}
		}
	}

	arrr2d* getRow(size_t rowIndex) {
		arrr2d* current = this;
		if (rowIndex >= current->numRows) {
		throw out_of_range("Row index out of bounds");
		}
		for (size_t i = 0; i < rowIndex; ++i) {
		current = current->next;
		}
		return current;
	}

	void pushRow(arrr* newRow) {
		if (this == nullptr || newRow == nullptr) return;

		size_t newCols = (newRow->size > this->numCols) ? newRow->size : this->numCols;
		// Resize existing rows if necessary
		arrr2d* current = this;

		if(current->numRows == 0){
		current->row = new arrr();
		current->numCols = newCols;
		current->row->setSize(0);
		current->row->size = 0;
		for (size_t i = 0; i < newRow->size; ++i) {///////////////////////////////////////
		current->row->push(newRow->get(i)->value);
		}
		this->numRows++; 

		return;
		}


		while (current != nullptr) {
		if (current->row == nullptr){ current->row = new arrr();}

		current->row->setSize(newCols);
		current->row->size = newCols;
		current->numCols = newCols;
		current = current->next;
		}
		// Add new row
		current = this;
		while (current->next != nullptr) {
		current = current->next;
		}
		current->next = new arrr2d();
		current->next->row = new arrr(); // Create a new arrr
		current->next->row->setSize(0);
		current->next->row->size = 0;
		// Copy values from newRow to the new row
		for (size_t i = 0; i < newRow->size; ++i) {///////////////////////////////////////
		current->next->row->push(newRow->get(i)->value);
		}

		current->next->numCols = newCols;
		current->next->numRows = 1; // Each arrr2d represents one row

		this->numRows++; 
	}


	void pushCol(arrr* newCol) {
		arrr2d* current = this;
		size_t rowIndex = 0;
		while (current != nullptr) {
		if (rowIndex < newCol->size) {
		if (numCols >= current->row->size) {
		current->row->push(newCol->get(rowIndex)->value);
		} else {
		current->row->get(numCols)->value = newCol->get(rowIndex)->value;
		}
		} else {
		if (numCols >= current->row->size) {
		current->row->push("");
		} else {
		current->row->get(numCols)->value = "";
		}
		}
		current = current->next;
		rowIndex++;
		}
		numCols++;
	}


	string& at(size_t rowIndex, size_t colIndex) {
		return getRow(rowIndex)->row->get(colIndex)->value;	
	}

	void pushArray(arrr2d* otherArray) {
		size_t newNumCols = this->numCols + otherArray->numCols;
		size_t newNumRows = max(this->numRows, otherArray->numRows);
		arrr2d* newArray = new arrr2d();
		newArray->setSize(newNumRows, newNumCols);

		for (size_t i = 0; i < newNumRows; ++i) {
		for (size_t j = 0; j < this->numCols; ++j) {
		if (i < this->numRows) {
		try{
		newArray->at(i, j) = this->at(i, j);
		}
		catch(...){newArray->at(i, j) = "";}
		} else {
		newArray->at(i, j) = "";
		}
		}

		for (size_t j = 0; j < otherArray->numCols; ++j) {
		if (i < otherArray->numRows) {
			try{
		newArray->at(i, this->numCols + j) = otherArray->at(i, j);
		}
		catch(...){newArray->at(i, j) = "";}
		} else {
		newArray->at(i, this->numCols + j) = "";
		}
		}
		}

		// Replace the current array with the new merged array
		this->row = newArray->row;
		this->next = newArray->next;
		this->numRows = newArray->numRows;
		this->numCols = newArray->numCols;

	}

	void print(){
		for (int i = 0; i < this->numRows; ++i) {
			for (int j = 0; j < this->numCols; ++j) {
				try{
				cout << this->at(i,j) << " ";
				if(this->at(i,j) != ""){cout<<"\t";}
				}
				catch(...){cout<<"\t";}
			}
			cout << endl;
		}
		}


};

string readFileContentToWstring(const string& filename) { //throws errors omg!!!!!!
	ifstream File(filename, ios::binary);
	if (!File.is_open()) {
	throw runtime_error("Failed to open file: " + filename);
	}
	return string(istreambuf_iterator<char>(File), 
	istreambuf_iterator<char>());
}


					arrr2d* tables = new arrr2d;
					string name = "";
					size_t tuplesLimit = 1000;
					namespace fs = filesystem;



void readJSONSchemaIntoStructures(const string& filename) {
	try {
		string jsonContent = readFileContentToWstring(filename);
		json jsonData = json::parse(jsonContent);

		name = jsonData["name"];
		tuplesLimit = jsonData["tuples_limit"];

		const auto& structure = jsonData["structure"];
		size_t maxColumns = 2;

		for (const auto& [tableName, columns] : structure.items()) {
		maxColumns = max(maxColumns, columns.size() + 2);}
		tables->setSize(structure.size(), maxColumns);

		size_t tableIndex = 0;
		for (const auto& [tableName, columns] : structure.items()) {
			tables->at(tableIndex, 0) = tableName;
			tables->at(tableIndex, 1) = tableName + "_pk";  // Add primary key column

			for (size_t i = 0; i < columns.size(); ++i) {
				tables->at(tableIndex, i + 2) = columns[i];
			}

			for (size_t i = columns.size() + 2; i < maxColumns; ++i) {
				tables->at(tableIndex, i) = "";
			}

			tableIndex++;
		}
	} catch (const exception& e) {
		throw runtime_error("Error parsing JSON: " + string(e.what()));
	}
}

string getTableVal(string column, string table, size_t indx) {

	fs::path tablePath = fs::path(name) / table;

	if (!fs::exists(tablePath) || !fs::is_directory(tablePath)) {
		cout << "#GTV# Error: Table not found or not a directory: " << table << endl;
		throw runtime_error("Missing table");
	}

	string trueVal;
	size_t currowi = 0;
	for (const auto& fileEntry : fs::directory_iterator(tablePath)) {
		if (fs::is_regular_file(fileEntry) && fileEntry.path().extension() == ".csv") {
			ifstream csvFile(fileEntry.path());
			if (csvFile.is_open()) {
				string line;
				arrr* headersf = new arrr();
				headersf->setSize(0);
				getline(csvFile, line);
				istringstream headerStream(line);
				string header;
				while (getline(headerStream, header, ',')) {
					headersf->push(header);	
				}

				int columnIndex = -1;
				for (int i = 0; i < headersf->size; ++i) {
					if (headersf->get(i + 1)->value == column) {
					columnIndex = i+1;
					break;
					}
				}

				while (getline(csvFile, line)) {
					if(currowi == indx){
					istringstream linefStream(line);
					int curcoli = 0;
					while (getline(linefStream, trueVal, ',')) {
					if(columnIndex==curcoli){
					return trueVal;
					}
					curcoli++;
					}
					}
					currowi++;
				}
			}
		}
	}

	throw runtime_error("Missing value");
}

string tocharints(string& str){
	string result;
	for (char ch:str){
	result+= to_string(static_cast<size_t>(ch));
	}
	return result;
}

void obliterateDirectory(const string& path) {
try {
for (const auto& entry : fs::directory_iterator(path)) {
fs::remove_all(entry.path());
}
fs::remove(path);
} catch (const fs::filesystem_error& e) {
cerr << "Oops! An error: " << e.what() << '\n';
}
}

void createDatabaseStructure(const string& schemaName, arrr2d* tables, size_t tuplesLimit) {
	try{obliterateDirectory(schemaName);}
	catch(...){};
	
	fs::path schemaPath = schemaName;
	fs::create_directory(schemaPath);
	
	for (size_t i = 0; i < tables->numRows; ++i) {
		string tableName = tables->at(i, 0);
		fs::path tablePath = schemaPath / tableName;
		fs::create_directory(tablePath);
		
		// Create primary key sequence file
		ofstream pkSequenceFile(tablePath / (tableName + "_pk_sequence.txt"));
		pkSequenceFile << tocharints(tableName) + string(to_string(tuplesLimit).length()+1, '0');
		pkSequenceFile.close();
		
		// Create lock file
		ofstream lockFile(tablePath / (tableName + "_lock.txt"));
		lockFile << "0";
		lockFile.close();
		
		ofstream csvFile(tablePath / "1.csv");
		if (!csvFile.is_open()) {
			throw runtime_error("Failed to create CSV file for table: " + tableName);
		}
		
		for (size_t j = 1; j < tables->numCols; ++j) {
			csvFile << tables->at(i, j);
			if (j < tables->numCols - 1) csvFile << ",";
		}
		csvFile << "\n";
		csvFile.close();
	}
}

bool acquireTableLock(const fs::path& lockFilePath) {
	fstream lockFile(lockFilePath, ios::in | ios::out);
	if (!lockFile) {
	lockFile.open(lockFilePath, ios::out);
	lockFile << "0";
	lockFile.close();
	lockFile.open(lockFilePath, ios::in | ios::out);
	}

	string lockStatus;
	getline(lockFile, lockStatus);

	if (lockStatus == "0") {
	lockFile.seekp(0);
	lockFile << "1";
	lockFile.close();
	return true;
	}

	lockFile.close();
	return false;
}


void releaseTableLock(const fs::path& lockFilePath) {
	ofstream lockFile(lockFilePath, ios::out | ios::in);
	lockFile.seekp(0);
	lockFile << "0";
	lockFile.close();
}

void iterate(string& number) { //awesome
	int i = number.length() - 1;
	
	while (i >= 0) {
		if (number[i] < '9') {
			number[i]++;
			return;
		} else {
			number[i] = '0';
			i--;
		}
	}
	
	if (i < 0) {
		number = "1" + number;
	}
}

void insertNewRecord(const string& tableName, arrr*& record) { //mid
fs::path tablePath = fs::path(name) / tableName;
	fs::path lockFilePath = tablePath / (tableName + "_lock.txt");
	fs::path pkSequenceFilePath = tablePath / (tableName + "_pk_sequence.txt");
if(!acquireTableLock(lockFilePath)){
	throw runtime_error("CSV file for writing is locked: " + lockFilePath.string());
}

	size_t fileNumber = 1;
	string fileName;

	do {
		fileName = to_string(fileNumber) + ".csv";
		fs::path filePath = tablePath / fileName;
		ifstream countFile(filePath);
		size_t lineCount = count(istreambuf_iterator<char>(countFile), istreambuf_iterator<char>(), '\n');

		if (lineCount < tuplesLimit) {
			
			ofstream csvFile(filePath, ios::app);
			if(lineCount==0){
				string firstName = "1.csv";
				fs::path fileFirstPath = tablePath / firstName;
				ifstream firstFile(fileFirstPath);
				string line;
				getline(firstFile,line);
				csvFile << line<<"\n";
				//if (i < record->size - 1) csvFile << ",";
				continue;
			}
			ifstream pkSequenceFile(pkSequenceFilePath);
			string currentPk;
			pkSequenceFile >> currentPk;
			pkSequenceFile.close();

			iterate(currentPk);

			ofstream pkSequenceFileOut(pkSequenceFilePath);
			pkSequenceFileOut << currentPk;
			pkSequenceFileOut.close();

			if (!csvFile.is_open()) {
				releaseTableLock(lockFilePath);
				throw runtime_error("Failed to open CSV file for writing: " + filePath.string());
			}

			csvFile << currentPk << ",";
			
			
			
			for (size_t i = 0; i < record->size; ++i) {
				csvFile << record->get(i)->value;
				if (i < record->size - 1) csvFile << ",";
			}
			csvFile << "\n";
			csvFile.close();

			releaseTableLock(lockFilePath);
			return;
		}

		fileNumber++;
	} while (true);
}

arrr* stringtoArrr(string data){ //also awesome
	istringstream dataStr(data);
	string value;
	arrr* result = new arrr;
	result->setSize(0);
	while(getline(dataStr,value,',')){
	result->push(value);
	}
	return result;
}

void trim(string& s) { 
s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) {
return !isspace(ch);
}));
s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
return !isspace(ch);
}).base(), s.end());
}

bool strToBool(string str){
	if(str == "1"){return true;}
	else {return false;}
}

string boolToStr(bool bl){
	if(bl){return "1";}
	else{return "0";}
}

bool evaluateWhereClause(const string& line, arrr* headers, const string& expression, size_t indx) {
if (expression.empty()) return true;

istringstream exprStream(expression);
 arrr* tokens = new arrr;
tokens->setSize(0);
string token;
while (exprStream >> token) tokens->push(token);

arrr* values = new arrr;
arrr* operators = new arrr;

for (size_t i = 0; i < tokens->size; ++i) {
	if (tokens->get(i)->value == "AND" || tokens->get(i)->value == "OR") {
	operators->push(tokens->get(i)->value);
	} 

	 else {
	string leftValue = tokens->get(i)->value;
	string op = tokens->get(++i)->value;
	string rightValue = tokens->get(++i)->value;

	if (leftValue[0] == '\'' && leftValue.back() == '\'') {
	leftValue = leftValue.substr(1, leftValue.length() - 2);
	} else {
	try {
	leftValue = getTableVal(leftValue.substr(leftValue.find('.') + 1), leftValue.substr(0, leftValue.find('.')), indx);
	} catch (...) {
	leftValue = "N\\A LEFT";
	}
	}


	if (rightValue[0] == '\'' && rightValue.back() == '\'') {
	rightValue = rightValue.substr(1, rightValue.length() - 2);
	} else {
	try {
	rightValue = getTableVal(rightValue.substr(rightValue.find('.') + 1), rightValue.substr(0, rightValue.find('.')), indx);
	} catch (...) {
	rightValue = "N\\A RIGHT";
	}
	}


	values->push(boolToStr(op == "=" ? (leftValue == rightValue) : false));
	}

	while (values->size > 1) {
	bool right = strToBool(values->get(values->size - 1)->value); values->pop();
	bool left = strToBool(values->get(values->size - 1)->value); values->pop();
	string op = operators->get(operators->size - 1)->value; operators->pop();
	values->push(boolToStr(op == "AND" ? (left && right) : (left || right)));
	}
}

return strToBool(values->get(values->size - 1)->value);
}


arrr2d* select(const string& schemaName, const string& columns, const string& tableNames, const string& whereClause) { //uncanny incredibles skull

arrr2d* result = new arrr2d();

arrr2d* actualresult = new arrr2d();
actualresult->setSize(1, 1);

result->setSize(1, 1);  // Initialize
arrr* columnList = new arrr();
arrr* evalCL = new arrr();
arrr* evalLL = new arrr();
evalCL->setSize(1);
evalLL->setSize(1);
columnList->setSize(1);
istringstream columnStream(columns);
string column;


//cout<<"1";
columnList->push("");
while (getline(columnStream, column, ',')) {
trim(column);
columnList->push(column);
//cout<<"pushed col:"<<column<<"; ";
}

columnList->push("");
	
	result->setSize(columnList->size-1, 0);
	istringstream tableStream(tableNames);
	string tableName;
	size_t k=0;
	size_t rowIndex = 0;
	//cout<<tableNames;
while (getline(tableStream, tableName, ',')) {
	trim(tableName);
	//cout<<"Cur t:"<<tableName<<endl;
	rowIndex = 0;
	fs::path tablePath = fs::path(schemaName) / tableName;
	fs::path lockFilePath = tablePath / (tableName + "_lock.txt");
	if (!acquireTableLock(lockFilePath)) {
	throw runtime_error("CSV file is locked: " + lockFilePath.string());
	}

	
	//cout << "Table found: " << tableName << endl;
	if (!fs::exists(tablePath) || !fs::is_directory(tablePath)) {
	//cout << "Table not found: " << tableName << endl;
	continue;
	}
	//cout<<"3";
	for (const auto& fileEntry : fs::directory_iterator(tablePath)) {
	if (fs::is_regular_file(fileEntry) && fileEntry.path().extension() == ".csv") {
	ifstream csvFile(fileEntry.path());
	if (csvFile.is_open()) {
	string line;
	getline(csvFile, line);  // Read header

	//cout<<"GETTING HEAD\n";
	arrr* headers = new arrr();
	headers->setSize(0);
	istringstream headerStream(line);
	string header;
	while (getline(headerStream, header, ',')) {
	headers->push(header);
	} //tbh dc
	
	
	while (getline(csvFile, line)) {
			
			arrr* resRow = new arrr();
			resRow->setSize(0);
			resRow->size = 0;
			if (evaluateWhereClause(line, headers, whereClause, rowIndex)) { 
			istringstream lineStream(line);
			string value;
			size_t columnIndex = 0;
			//cout<<"6";
			while (getline(lineStream, value, ',')) { 
			
			for (size_t j = 1; j < columnList->size+1; ++j) {
			//	cout<<"8";
			string requestedTable = columnList->get(j)->value.substr(0, columnList->get(j)->value.find('.'));
			//cout<<"S req t"<<requestedTable<<" \t";
			string requestedColumn = columnList->get(j)->value.substr(columnList->get(j)->value.find('.') + 1);
			//cout<<"9 req c"<<requestedColumn<<" \n";;
			try{
			if ((requestedTable == tableName) && (headers->get(columnIndex)->value == requestedColumn)) { 
			resRow->push(value);
			}else{resRow->push("");}}
			catch(exception& error){resRow->push("");}
			}

			columnIndex++;
			}
			}

			result->pushRow(resRow);
		
			rowIndex++;
			//resRow->popAll();
			delete resRow;
		}
	csvFile.close();
	//cout<<"C closing csv...\n";
	}
	
	}
	
	}
	//cout<<"Finished a table...\n";
	//result->print();
}
//cout<<"R";
//cout<<actualresult->numCols<<" "<<actualresult->numRows;
actualresult->print();
return actualresult;//lol why

}


void deleteRecords(const string& tableName, const string& whereClause) {
	fs::path tablePath = fs::path(name) / tableName;
	fs::path lockFilePath = tablePath / (tableName + "_lock.txt");

	if (!acquireTableLock(lockFilePath)) {
	throw runtime_error("CSV file is locked: " + lockFilePath.string());
	}

	size_t fileNumber = 1;
	string fileName;

	do {
	fileName = to_string(fileNumber) + ".csv";
	fs::path filePath = tablePath / fileName;

	if (!fs::exists(filePath)) {
	break;
	}

	ifstream inputFile(filePath);
	ofstream tempFile(filePath.string() + ".tmp");

	if (!inputFile.is_open() || !tempFile.is_open()) {
	releaseTableLock(lockFilePath);
	throw runtime_error("Failed to open files for deletion: " + filePath.string());
	}

	string line;
	getline(inputFile, line);
	tempFile << line << "\n";

	arrr* headers = new arrr();
	istringstream headerStream(line);
	string header;
	while (getline(headerStream, header, ',')) {
	headers->push(header);
	}
	size_t indx = 0;
	bool hasContent = false;
	while (getline(inputFile, line)) {
	if (!evaluateWhereClause(line, headers, whereClause, indx)) {
	tempFile << line << "\n";
	hasContent = true;
	indx++;
	}
	}


	inputFile.close();
	tempFile.close();

	fs::remove(filePath);

	if (hasContent) {
	fs::rename(filePath.string() + ".tmp", filePath);
	} else {
	fs::remove(filePath.string() + ".tmp");
	cout << "Removed empty file: " << filePath << "\n";
	}

	fileNumber++;
	} while (true);

	releaseTableLock(lockFilePath);
}


void initializeDatabase(){ //dummy ah data
arrr* data = new arrr;
data = stringtoArrr("Fido,dog");
insertNewRecord("Pets", data);
 data = stringtoArrr("Whiskers,cat");
insertNewRecord("Pets", data);
 data = stringtoArrr("Bubbles,fish");
insertNewRecord("Pets", data);
 data = stringtoArrr("Fido,dog");
insertNewRecord("Pets", data);
 data = stringtoArrr("Slither,snake");
insertNewRecord("Pets", data);
 data = stringtoArrr("Whiskers,cat");
insertNewRecord("Pets", data);
 data = stringtoArrr("Hoppy,rabbit");
insertNewRecord("Pets", data);
 data = stringtoArrr("apple,red,sweet");
insertNewRecord("Fruits", data);
 data = stringtoArrr("lemon,yellow,sour");
insertNewRecord("Fruits", data);
 data = stringtoArrr("grape,green,sour");
insertNewRecord("Fruits", data);
 data = stringtoArrr("banana,yellow,sweet");
insertNewRecord("Fruits", data);
 data = stringtoArrr("a,b,c,d");
insertNewRecord("b2", data);
 data = stringtoArrr("e,f,g,h");
insertNewRecord("b2", data);
 data = stringtoArrr("a,b,c,d");
insertNewRecord("b2", data);

	return;
}


string subp(string query) { //mesglen mesg
try {
//freopen("output.txt", "w", stdout); //who the hell needs to rewrite all the code amirite?
stringstream buffer;
streambuf* old_stdout = cout.rdbuf(buffer.rdbuf());



string choi="";
arrr* arr = new arrr;
cout << endl;
ifstream File("schema.json", ios::binary);
File.open("schema.json");
	if (!File.is_open()) {
	throw runtime_error("Failed to open file: " );
	}
	 readJSONSchemaIntoStructures("schema.json");
/*
cout << tables->numCols;
cout << "Pushed values: ";
for (int i = 0; i < tables->numRows; ++i) {
for (int j = 0; j < tables->numCols; ++j) {
cout << tables->at(i,j) << " "<<i<<"+"<<j<<" ";
}
cout << endl;
}*/

if(query=="newdb"){
createDatabaseStructure(name, tables, tuplesLimit);
//initializeDatabase();
return "\nnewdb was created";
}
/*cout<<"\n Create a new database? Y/N\n";
getline(cin, choi);
if(choi=="y"||choi=="Y"){
	createDatabaseStructure(name, tables, tuplesLimit);
}
	else if (choi=="n"||choi=="N"){}
	else{cout<<"invalid input"; return -1;}

cout<<"\n Initialize the database with default values? Y/N\n";
choi="";
getline(cin, choi);
if(choi=="y"){
	
}
	else if (choi=="n"){}
	else{cout<<"invalid input"; return -1;}
*/
//printAllTablesAndValues("scheme1");

/*
string query = "";

for (int i = 0; i < argc; i++){
	query = query + " " +argv[i];
	
	query = query.substr(1);
}*/

//while (true) {
//cout << "Enter your query (or 'quit' to exit): ";
//getline(cin, query);
//cout << "\nQUERY:" <<query<<endl<<endl;
if (query == "quit"||query == "exit") {
return "1";
}
/*
if (query == "b") {
query = "SELECT Pets.name FROM Pets";
}
if (query == "c") {
query = "SELECT Pets.name,Pets.type FROM Pets";
}
if (query == "d") {
query = "SELECT Pets.name,Pets.type FROM Pets WHERE Pets.name = 'Whiskers' OR Pets.name = 'Fido'";
}
if (query == "e") {
query = "SELECT Pets.name,Pets.type FROM Pets WHERE Pets.name = 'Whiskers' AND Pets.name = 'Fido'";
}
if (query == "f") {
query = "SELECT Pets.name,Pets.type FROM Pets,b2 WHERE b2.e1 = 'a'";
}
if (query == "g") {
query = "SELECT Pets.name,Pets.type,Fruits.name FROM Pets,Fruits WHERE Fruits.taste = 'sour' OR Fruits.color = 'yellow'";
}*/

if (query.substr(0, 6) == "SELECT") {
size_t fromPos = query.find("FROM");
size_t wherePos = query.find("WHERE"); //must have spaces

if (fromPos == string::npos) {
cout << "Invalid query: Missing FROM clause\n";
return "Invalid query: Missing FROM clause\n";
}

string columns = query.substr(7, fromPos - 8);
string tableNames = query.substr(fromPos + 5, (wherePos != string::npos ? wherePos - fromPos - 6 : string::npos));
string whereClause = (wherePos != string::npos ? query.substr(wherePos + 6) : "");

 trim(columns);
 trim(tableNames);
 trim(whereClause);

arrr2d* result = new arrr2d;
result = select(name, columns, tableNames, whereClause);


}



if (query.substr(0, 11) == "INSERT INTO") {

arrr* values = new arrr;
values->setSize(0);
string tableName = "";
size_t tableNameStart = 12;
size_t tableNameEnd = query.find("VALUES");

if (tableNameEnd == string::npos) {
cout << "Invalid query: Missing VALUES clause\n";
return "Invalid query: Missing VALUES clause\n";
}

tableName = query.substr(tableNameStart, tableNameEnd - tableNameStart);
 trim(tableName);

size_t valuesStart = query.find("(", tableNameEnd);
size_t valuesEnd = query.find(")", valuesStart);

if (valuesStart == string::npos || valuesEnd == string::npos) {
cout << "Invalid query: Malformed VALUES clause\n";
return "-1";
}

string valuesString = query.substr(valuesStart + 1, valuesEnd - valuesStart - 1);
istringstream valueStream(valuesString);
string value;

while (getline(valueStream, value, ',')) {
 trim(value);
if (value.front() == '\'' && value.back() == '\'') {
value = value.substr(1, value.length() - 2);
}
values->push(value);
}
//values->print();
 insertNewRecord(tableName,values);
 return "insterted";
}

if (query.substr(0, 11) == "DELETE FROM") {
string tableName;
string whereClause;

if (query.substr(0, 11) != "DELETE FROM") {
cout << "Invalid query: Must start with DELETE FROM\n";
return "Invalid query: Must start with DELETE FROM\n";
}

size_t tableNameStart = 12;
size_t wherePos = query.find("WHERE");

if (wherePos == string::npos) {
cout << "Invalid query: Missing WHERE clause\n";
return "Invalid query: Missing WHERE clause\n";
}

tableName = query.substr(tableNameStart, wherePos - tableNameStart - 1);
 trim(tableName);

whereClause = query.substr(wherePos + 6);
 trim(whereClause);

if (tableName.empty() || whereClause.empty()) {
cout << "Invalid query: Malformed DELETE statement\n";
return "-1";
}

cout << "\nTable: " << tableName << "\nWHERE clause: " << whereClause << "\n";

deleteRecords(tableName, whereClause);

}


//
//delete tables;



//cout << "Program completed successfully. " << endl;

cout.rdbuf(old_stdout);
string result = buffer.str();
return result;
}
catch (const exception& error) {
cout << "\nError: " << error.what() << endl;
}

return "normal";
}
