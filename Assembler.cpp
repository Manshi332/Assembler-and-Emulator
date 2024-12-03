/* 
NAME : Manshi Prajapati
ROLL NUMBER: 2302CS08
DECLARATION OF AUTHORSHIP: I HEREBY DECLARE THAT THE SOURCE CODE I AM SUBMITTING IS ENTIRELY MY OWN ORIGINAL WORK EXCEPT WHERE OTHERWISE INDICATED.
*/
#include<bits/stdc++.h>
using namespace std;



//Structure to store details for listing file generation
struct ListingDetails {
    string address;      // The address (program counter value) in the listing file
    string machineCode;  // The corresponding machine code (in hex format)
    string statement;    // The statement or source code associated with the machine code
};

//Structure to store details of a program line, associated with the program counter (PC)
struct LineDetails {
    int programCounter;   // The program counter value corresponding to this line in the program
    string label;         // The label associated with the line (if any)
    string instruction;   // The mnemonic/instruction (e.g., 'ldc', 'add', etc.)
    string operand;       // The operand used with the instruction (e.g., value, offset)
    string previousOperand; // The operand used in the previous instruction (for comparison)
};

// Containers to store different information related to errors, warnings, lines, and listings
vector<pair<int,string>> WARNING_RECORDS;          // List to store all warnings encountered
vector<pair<int,string>> ERROR_RECORDS;              // List to store all errors encountered
vector<ListingDetails> listingEntries;       // List to store the generated listing file entries
vector<LineDetails> lineRecords;             // List to store program line information
vector<std::string> machineCodeList;         // List to store machine codes in 8-bit hexadecimal format
vector<string> readLines; 		// stores each line 

vector<pair<string, pair<int, int>>> symbolTable;  // {label, {address, line number where label is defined}}
vector<pair<string, vector<int>>> labelReferences;  // {label, {list of line numbers where the label is referenced}}
vector<pair<string, pair<string, int>>> opcodeTable; // {mnemonic, {opcode, operand type}}
vector<pair<int, string>> commentLines;            // {line number, comment associated with the line}
vector<pair<string, string>> variableAssignments;   // {variable (label), associated value}

void ADD_ERROR(int lineNum, string errorMsg) {
    int lineStorage = lineNum;  // Store the line number in a temporary variable
    string errorStorage = errorMsg;  // Store the error message in a temporary variable

    ERROR_RECORDS.push_back({lineStorage, errorStorage});  // Push back to ERROR_RECORDS
}

// Function to add a warning with a specific line number and warning message to the warning records
void ADD_WARNING(int lineNum, string warningMsg) {
    int warningLine = lineNum;  // Store the line number in a temporary variable
    string warningMessage = warningMsg;  // Store the warning message in a temporary variable

    WARNING_RECORDS.push_back({warningLine, warningMessage});  // Push back to WARNING_RECORDS
}

	// Function to print all stored errors to the console with their line numbers
    void PRINT_ERRORS(){
        cout<<"Errors: "<<endl;
        for(auto & it: ERROR_RECORDS){
            cout<<"Line number: "<<it.first<<" Error: "<<it.second<<endl;
        }
    }
	 // Function to print all stored warnings to the console with their line numbers
    void PRINT_WARNINGS(){
        cout<<"Warnings: "<<endl;
        for(auto &it: WARNING_RECORDS){
             cout<<"Line number: "<<it.first<<" Warning : "<<it.second<<endl;
        }
    }



void Opcode_Initialisation() {
    
    opcodeTable = {
        {"data", {"", 1}}, {"ldc", {"00", 1}}, {"adc", {"01", 1}}, {"ldl", {"02", 2}},
        {"stl", {"03", 2}}, {"ldnl", {"04", 2}}, {"stnl", {"05", 2}}, {"add", {"06", 0}},
        {"sub", {"07", 0}}, {"shl", {"08", 0}}, {"shr", {"09", 0}}, {"adj", {"0A", 1}},
        {"a2sp", {"0B", 0}}, {"sp2a", {"0C", 0}}, {"call", {"0D", 2}}, {"return", {"0E", 0}},
        {"brz", {"0F", 2}}, {"brlz", {"10", 2}}, {"br", {"11", 2}}, {"HALT", {"12", 0}},
        {"SET", {"", 1}}
    };
   /* 
    The third argument indicates the required operand type:
    - type 0 : No operand is necessary
    - type 1 : A specific value must be provided as an operand
    - type 2 : An offset value should be used as the operand
*/


}

// Function to extract substring starting from a given index in the word
string extractSubstringFromIndex(const string& word, size_t startIndex) {
    return word.substr(startIndex);
}

vector<string> GET_WORDS(string currentLine, int locationCounter) {  
    // Return empty vector if line is empty
    if (currentLine.empty()) return {};

    vector<string> result;  // Parsed words
    stringstream now(currentLine);  // Stream to extract words
    string word;

    // Process each word in the line
    while (now >> word) {
        if (word.empty()) continue;  // Skip empty words
        if (word[0] == ';') break;  // Stop at comment marker ';'

        string tempWord = word;  // Temporary variable to store the word

        // Handle ':' not separated from the word
        auto i = tempWord.find(':');
        if (i != string::npos && tempWord.back() != ':') {  
            result.push_back(tempWord.substr(0, i + 1));  // Add part before ':'
            tempWord = extractSubstringFromIndex(tempWord, i + 1);  // Update word after ':'
        }

        // Handle ';' attached to word
        if (tempWord.back() == ';') {  
            tempWord.pop_back();  // Remove ';'
            result.push_back(tempWord);  // Add word without ';'
            break;  // Stop parsing
        }

        result.push_back(tempWord);  // Add word to result
    }

    // Extract comment from line if present
    string comment = "";
    for (int i = 0; i < (int)currentLine.size(); ++i) {
        if (currentLine[i] == ';') {
            int j = i + 1;  // Start after ';'
            while (j < currentLine.size() && currentLine[j] == ' ') ++j;  // Skip spaces

            // Append characters to comment
            for (; j < currentLine.size(); ++j) {
                comment += currentLine[j];
            }
            break;  // Stop extraction
        }
    }

    // Store comment with location if found
    if (!comment.empty()) {
        commentLines.push_back({locationCounter, comment});
    }
    
    return result;  // Return parsed words
}




class CHECKER {
public:
    // Check if the character is an alphabet (a-z or A-Z)
bool isAlphabet(char ch) {
    // Convert character to lowercase for uniformity (handling both upper and lower case)
    char lowerCh = tolower(ch);  
    char start = 'a', end = 'z';  
    
    // Check if the character is within the range of 'a' and 'z'
    bool isAlpha = (lowerCh >= start && lowerCh <= end);  
    
    return isAlpha;  // Return the result
}

// Check if the character is a digit (0-9)
bool isDigit(char ch) {

    char start = '0', end = '9';  
    
    // Check if the character is within the range of '0' and '9'
    bool isNum = (ch >= start && ch <= end);  
    
    return isNum;  // Return the result
}

    // Validate if the label is correct
    // A valid label must start with an alphabet and can contain digits, alphabets, and underscores
    bool isValidLabel(string label) {  
        string s=label;
        bool valid = true;
        // The first character must be an alphabet
        valid &= isAlphabet(s[0]);
        // Every character in the label must be either a digit, alphabet, or an underscore
        for (char ch : s) {
            valid &= (isDigit(ch) || isAlphabet(ch) || (ch == '_'));
        }
        
        return valid;  // Return whether the label is valid
    }
    // Check if the string represents a decimal number (all characters should be digits)
    bool isDecimal(string number) {
        string s=number;
        bool valid = true;
        for (char ch : s) {
            valid &= isDigit(ch);  // Check if each character is a digit
        }
        return valid;  // Return whether the number is a valid decimal
    }
    // Determines if a string is a valid octal number (should start with '0' and contain digits between '0' and '7')
    bool isOctal(const string& str) {
    if (str.length() < 2 || str[0] != '0') return false;  // Must be at least 2 chars and start with '0'

    // Check that each character is within '0' to '7'
    for (char c : str) {
        if (c < '0' || c > '7') return false;
    }
    return true;
}

// Determines if a string is a valid hexadecimal number (should start with '0x' and contain valid hexadecimal characters)
bool isHexadecimal(const string& str) {
    size_t len = str.length();  // Store the length of the string
    if (len < 3) return false;  // Minimum length check
    string prefix = str.substr(0, 2);  // Extract the prefix "0x"
    if (prefix != "0x") return false;  // Check if it starts with "0x"

    // Check for valid hex characters after "0x"
    for (size_t i = 2; i < len; ++i) {
        char ch = tolower(str[i]);
        if (!isDigit(ch) && (ch < 'a' || ch > 'f')) return false;  // Must be 0-9 or a-f
    }
    return true;
}
};
CHECKER checker;

// Reverse a string in place
void reverseString(string& str) {
    int start = 0;
    int end = str.size() - 1;
    while (start < end) {
        swap(str[start], str[end]);  // Swap characters at start and end
        ++start;
        --end;
    }
}
 // Convert octal to decimal string
string octalToDecimal(string octal) {
    int decimalResult = 0;
    int power = 1;  // Variable to track the current power of 8

    // Convert each digit from right to left
    for (int i = octal.size() - 1; i >= 0; --i) {
        // Extract current digit
        char currentDigit = octal[i];

        // Convert current digit to decimal and compute its contribution to the result
        int digitDecimal = currentDigit - '0';  // Convert character to integer
        decimalResult += digitDecimal * power;  // Add its contribution to the result

        // Update power for the next digit
        power *= 8;
    }

    // Return result as string
    return to_string(decimalResult);
}


// Convert hexadecimal to decimal string
string hexadecimalToDecimal(string hexadecimal) {
    int decimalResult = 0;
    int powerOf16 = 1;  // Variable to store the power of 16 for each digit

    // Convert each hex digit from right to left
    for (int i = hexadecimal.size() - 1; i >= 0; --i) {
        char hexDigit = hexadecimal[i];
        int decimalValue = (hexDigit >= '0' && hexDigit <= '9') ? hexDigit - '0' : (hexDigit >= 'A' && hexDigit <= 'F') ? hexDigit - 'A' + 10 : 0;

        decimalResult += decimalValue * powerOf16;  // Add the converted decimal value to the result
        powerOf16 *= 16;  // Multiply powerOf16 by 16 for the next hex digit
    }

    return to_string(decimalResult);  // Return the decimal result as a string
}

// Convert decimal to 8-bit hexadecimal string
string decimalToHexadecimal(int decimal) {
    unsigned int decimalNum = decimal;
    string hexResult = "";
    
    // Convert to hex (8 bits)
    for (int i = 0; i < 8; ++i) {
        int remainder = decimalNum % 16;
        char hexChar = (remainder <= 9) ? char(remainder + '0') : char(remainder - 10 + 'A');  // Get the hex character

        hexResult += hexChar;  // Add hex digit to result
        decimalNum /= 16;  // Divide the number by 16 for the next iteration
    }

    reverse(hexResult.begin(), hexResult.end());  // Reverse the result to get correct order
    return hexResult;
}



// Helper function to add a new label to the symbol table
void addNewLabel(string label, int program_counter, int location_counter) {
    symbolTable.push_back({label, {program_counter, location_counter}});
}

// Process labels and check for errors
void PROCESS_AND_VALIDATE_LABEL(string label, int location_counter, int program_counter) {
    // Return if label is empty
    if (label.empty()) return;

    // Validate label format
    bool isValid = checker.isValidLabel(label);
    if (!isValid) {
        ADD_ERROR(location_counter, "Bogus Label name");  // Report invalid label error
    } else {
        bool labelExists = false;

        // Check if label is in the symbol table
        for (auto &entry : symbolTable) {
            if (entry.first == label) {
                if (entry.second.first != -1) {
                    ADD_ERROR(location_counter, "Duplicate label definition");  // Report duplicate error
                    labelExists = true;
                } else {
                    entry.second = {program_counter, location_counter};  // Update if label is defined later
                }
                break;  // Stop searching after finding the label
            }
        }

        // If label does not exist, add a new entry using addNewLabel function
        if (!labelExists) {
            addNewLabel(label, program_counter, location_counter);
        }
    }
}



string Operand_Processor(string operand, int lineCounter) {
    string processedOperand = "";  // Stores the processed operand

    // Check if operand is a valid label
    if (checker.isValidLabel(operand)) {
        bool labelFoundInReferences = false;

        // Search if the label already exists in labelReferences
        for (auto &labelRef : labelReferences) {
            if (labelRef.first == operand) {
                // If label is found, append current lineCounter to its reference list
                labelRef.second.push_back(lineCounter);
                labelFoundInReferences = true;
                break;
            }
        }
        
        // If label not found in labelReferences, add a new entry
        if (!labelFoundInReferences) {
            labelReferences.push_back({operand, {lineCounter}});
        }

        bool labelExistsInSymbolTable = false;

        // Check if operand exists in the symbolTable
        for (auto &symbol : symbolTable) {
            if (symbol.first == operand) {
                labelExistsInSymbolTable = true;
                break;
            }
        }

        // If operand (label) is not found in symbolTable, add it with a placeholder (-1 program counter)
        if (!labelExistsInSymbolTable) {
            symbolTable.push_back({operand, {-1, lineCounter}});  // Label used but not defined
        }

        // Return the operand as-is if it's a valid label
        return operand;
    }

    // If operand is not a valid label, process as numeric value (octal, hexadecimal, or decimal)
    string unsignedOperand = operand, sign = "";

    // Check if operand has a sign (+ or -)
    if (unsignedOperand[0] == '-' || unsignedOperand[0] == '+') {
        sign = unsignedOperand[0];  // Store the sign
        unsignedOperand = unsignedOperand.substr(1);  // Remove sign for processing
    }
    processedOperand += sign;  // Add the sign back to the result

    // Handle different operand formats: octal, hexadecimal, and decimal
    if (checker.isOctal(unsignedOperand)) {
        // Convert octal operand to decimal (removing leading '0')
        processedOperand +=octalToDecimal(unsignedOperand.substr(1));
    } else if (checker.isHexadecimal(unsignedOperand)) {
        // Convert hexadecimal operand to decimal (removing leading '0x')
        processedOperand += hexadecimalToDecimal(unsignedOperand.substr(2));
    } else if (checker.isDecimal(unsignedOperand)) {
        // If operand is already decimal, append it directly
        processedOperand += unsignedOperand;
    } else {
        // If operand format is invalid, return an empty string
        processedOperand = "";
    }

    return processedOperand;  // Return processed operand (or empty string if invalid)
}



//Finding errors related to Mnemonics
void Process_Mnemonic(string mnemonic, string &operand, int lineCounter, int progCounter, int extraTokens, bool &isValid) {
    if (mnemonic.empty()) return;  // If mnemonic is empty, nothing to process

    bool mnemonicFound = false;  // Flag to check if mnemonic is in opcode table
    int operandType = -1;  // To store the type of the mnemonic for operand handling

    // Search for the mnemonic in opcodeTable
    for (const auto &opcodeEntry : opcodeTable) {
        if (opcodeEntry.first == mnemonic) {  // If mnemonic is found
            mnemonicFound = true;
            operandType = opcodeEntry.second.second;  // Get the mnemonic type
            break;
        }
    }

    // If mnemonic is not found, log an error
    if (!mnemonicFound) {
        ADD_ERROR(lineCounter, "Invalid mnemonic");
    } else {
        // Check if operand is present
        bool hasOperand = !operand.empty();  // Flag to check if operand exists

        // If mnemonic requires an operand (operandType > 0)
        if (operandType > 0) {
            if (!hasOperand) {
                ADD_ERROR(lineCounter, "Missing operand");  // Log error if operand is missing
            } else if (extraTokens > 0) {
                ADD_ERROR(lineCounter, "Extra content at end of line");  // Log error for extra content
            } else {
                // Process the operand (validate it)
                string processedOperand = Operand_Processor(operand, lineCounter);
                if (processedOperand.empty()) {
                    ADD_ERROR(lineCounter, "Invalid operand format");  // Log error if operand is invalid
                } else {
                    operand = processedOperand;  // Update operand if valid
                    isValid = true;
                }
            }
        } else if (operandType == 0 && hasOperand) {
            ADD_ERROR(lineCounter, "Unexpected operand");  // Log error if operand should not be present
        } else {
            isValid = true;  // Set flag to indicate no issues
        }
    }
}



// Function to check for undefined labels after processing all lines
void CHECK_UNDEFINED_LABELS() {
    for (const auto& symTableEntry : symbolTable) {
        bool isLabelUsed = false;

        // Check if the label is referenced in any instruction
        for (const auto& labelRef : labelReferences) {
            if (labelRef.first == symTableEntry.first) {
                isLabelUsed = true;
                break;  // Label is used, exit loop
            }
        }

        // Handle undefined labels
        if (symTableEntry.second.first == -1) {
            // Report errors for each line referring to this undefined label
            for (const auto& labelRef : labelReferences) {
                if (labelRef.first == symTableEntry.first) {
                    for (int line : labelRef.second) {
                        ADD_ERROR(line, "no such label");  // Add error for each usage of the undefined label
                    }
                    break;
                }
            }
        } else if (!isLabelUsed) {
            // If the label is declared but never used, add a warning
            WARNING_RECORDS.push_back({symTableEntry.second.second, "Label declared but not used"});
        }
    }
}

// Perform the first pass of the assembler to process lines and check for label and operand errors
void PASS_1(const vector<string>& inputLines) {
    int lineCounter = 0, progCounter = 0;

    // Process each line in the input
    for (const string& currentLine : inputLines) {
        ++lineCounter;

        // Parse the current line into components
        auto parsedWords = GET_WORDS(currentLine, lineCounter);

        if (parsedWords.empty()) continue;  // Skip empty lines

        string label = "";
        string  mnemonic = "";
        string operand = "";
        int wordIndex = 0, totalWords = parsedWords.size();

        // Process label if present
        if (parsedWords[wordIndex].back() == ':') {
            label = parsedWords[wordIndex];
            label.pop_back();
            ++wordIndex;
        }

        // Process mnemonic if it exists
        if (wordIndex < totalWords) {
            mnemonic = parsedWords[wordIndex];
            ++wordIndex;
        }

        // Process operand if it exists
        if (wordIndex < totalWords) {
            operand = parsedWords[wordIndex];
            ++wordIndex;
        }

        // Process label errors
        PROCESS_AND_VALIDATE_LABEL(label, lineCounter, progCounter);

        bool isOperandValid = false;
        string originalOperand = operand;

        // Process mnemonic and operand, check for errors
        Process_Mnemonic(mnemonic, operand, lineCounter, progCounter, totalWords - wordIndex, isOperandValid);

        // Temporary variables for line details to store in lineRecords
        int recordProgCounter = progCounter;
        string recordLabel = label;
        string recordMnemonic = mnemonic;
        string recordOperand = operand;
        string recordOriginalOperand = originalOperand;

        // Record current line details for the second pass
        lineRecords.push_back({recordProgCounter, recordLabel, recordMnemonic, recordOperand, recordOriginalOperand});

        // Increment program counter if operand is valid
        progCounter += isOperandValid;

        // Handle "SET" instructions
        if (isOperandValid && mnemonic == "SET") {
            if (label.empty()) {
                ADD_ERROR(lineCounter, "label (or variable) name missing");
            } else {
                // Temporary variables for variable assignments
                string varLabel = label;
                string varOperand = operand;

                variableAssignments.push_back({varLabel, varOperand});
            }
        }
    }

    // Check for undefined labels after processing all lines
    CHECK_UNDEFINED_LABELS();
}



// Insert program counter, machine code, and source line details into listingEntries
void add_in_list(int progCounter, string code, string lbl, string instr, string opr) {
    // Store the passed parameters in temporary variables
    int pcTemp = progCounter;
    string codeTemp = code;
    string labelTemp = lbl;
    string mnemonicTemp = instr;
    string operandTemp = opr;

    // Modify mnemonic and label if not empty
    if (!mnemonicTemp.empty()) mnemonicTemp += " ";  // Add space after mnemonic if not empty
    if (!labelTemp.empty()) labelTemp += ": ";       // Add colon after label if not empty
// Create temporary variables for concatenating label and mnemonic
string tempMnemonic = mnemonicTemp;  // Initialize tempMnemonic with mnemonicTemp
string tempLabel = labelTemp;        // Initialize tempLabel with labelTemp

// Modify mnemonic if not empty
if (!tempMnemonic.empty()) {
    string tempMnemonicWithSpace = tempMnemonic + " ";  // Add space after mnemonic
    tempMnemonic = tempMnemonicWithSpace;  // Store the result in tempMnemonic
}

// Modify label if not empty
if (!tempLabel.empty()) {
    string tempLabelWithColon = tempLabel + ": ";  // Add colon after label
    tempLabel = tempLabelWithColon;  // Store the result in tempLabel
}
    // Combine label, mnemonic, and operand into the full statement
    string fullStatement = labelTemp + mnemonicTemp + operandTemp;

    // Add hex program counter, machine code, and full statement to listingEntries
    listingEntries.push_back({decimalToHexadecimal(pcTemp), codeTemp, fullStatement});
}


// Generating machine codes and building the listing vector
void PASS_2() {
    // Iterate through each line record
    for (auto it : lineRecords) {
        string label = it.label, mnemonic = it.instruction, operand = it.operand, prevOperand = it.previousOperand;
        int program_counter = it.programCounter, type = -1;
        string opcode = "";

        // Retrieve opcode and type from opcodeTable
        for (const auto& it : opcodeTable) {
            if (it.first == mnemonic) {
                opcode = it.second.first;
                type = it.second.second;
                break;
            }
        }

        string machineCode = "        ";  // Default empty machine code

        if (type == 2) {  // Offset-based instruction (e.g., branch)
            int offset = -1;
            bool found = false;

            // Search symbolTable for label to calculate offset
            for (const auto& sym : symbolTable) {
                if (sym.first == operand) {
                    offset = sym.second.first - (program_counter + 1);
                    found = true;
                    break;
                }
            }

            if (!found) offset = stoi(operand);  // Convert operand to integer if label not found
            machineCode = decimalToHexadecimal(offset).substr(2) + opcode;  // Append offset in hex
        }
        else if (type == 1 && mnemonic != "data" && mnemonic != "SET") {  // Value-based instruction
            int value = -1;
            bool found = false;

            // Search symbolTable for label's value
            for (const auto& sym : symbolTable) {
                if (sym.first == operand) {
                    value = sym.second.first;
                    found = true;
                    break;
                }
            }

            if (!found) value = stoi(operand);  // Convert operand if label not found
            machineCode = decimalToHexadecimal(value).substr(2) + opcode;  // Append value in hex

            // Check if operand is in variableAssignments for SET operation
            auto it = find_if(variableAssignments.begin(), variableAssignments.end(),
                [&operand](const std::pair<std::string, std::string>& item) { 
                    return item.first == operand; 
                });

            if (it != variableAssignments.end())  // Use assigned value for SET operation
                machineCode = decimalToHexadecimal(stoi(it->second)).substr(2) + opcode;
        }
        else if (type == 0)  // No operand instruction (e.g., HALT)
            machineCode = "000000" + opcode;
        else if (type == 1 && (mnemonic == "data" || mnemonic == "SET"))  // Directly convert operand to hex
            machineCode = decimalToHexadecimal(stoi(operand));

        machineCodeList.emplace_back(machineCode);  // Add to machine code list

        // Add line details to listing
        add_in_list(program_counter, machineCode, label, mnemonic, prevOperand);
    }
}


// Write errors and warnings to a .log file
void WRITE_ERRORS_AND_WARNINGS_TO_FILE() {
    ofstream logFile("logfile.log");  // Create log file
    cout << "Error log file created." << endl;
    cout << (logFile.is_open() ? "Log file opened successfully." : "Failed to open log file.") << endl;

   // Sort the error records if there are any present
if (!ERROR_RECORDS.empty()) 
    sort(ERROR_RECORDS.begin(), ERROR_RECORDS.end());  // Sort the errors by line number or relevant order

// Sort the warning records if there are any present
if (!WARNING_RECORDS.empty()) 
    sort(WARNING_RECORDS.begin(), WARNING_RECORDS.end());  // Sort the warnings by line number or relevant order

    // If there are no errors, notify the user and list warnings if any
    if (ERROR_RECORDS.size() == 0) {
        logFile << "No errors found!" << endl;
        
        // Check for warnings and log them
        for (const auto& warn : WARNING_RECORDS)
            logFile << "Line: " << warn.first << " WARNING: " << warn.second << endl;
    }
    else {
        // Otherwise, list all errors in order
        for (const auto& err : ERROR_RECORDS)
            logFile << "Line: " << err.first << " ERROR: " << err.second << endl;
    }

    // Close log file when done
    if (logFile.is_open()) logFile.close();
}


int main() {
   ifstream cinfile;  // Create an input file stream
    cinfile.open("TestCase2.txt");  // Open the file  for reading

    // Check if file opening failed
    if (cinfile.fail()) {
        cout << "Input file doesn't exist" << endl;  // Print error message if file is not found
        exit(0);  // Exit the program
    }

    string curLine;
    // Read each line from the file until end of file is reached
    while (getline(cinfile, curLine)) {
        readLines.push_back(curLine);  // Add each line to the readLines vector
    }

    cinfile.close();  // Close the file after reading all lines
   Opcode_Initialisation();
   PASS_1(readLines);
   WRITE_ERRORS_AND_WARNINGS_TO_FILE();
   if(ERROR_RECORDS.empty()){
    PASS_2();
   // Write listing info to .lst file and machine code to .o binary file

    ofstream coutList("listfile.lst");  // Open .lst file
    for (auto entry : listingEntries) {
        // Write address, code, and statement
        coutList << entry.address << " " << entry.machineCode << " " << entry.statement << endl;
    }
    coutList.close();  // Close .lst file
    cout << "Listing (.lst) file generated" << endl;

    ofstream coutMCode("machineCode.o", ios::binary | ios::out);  // Open .o file in binary mode
    for (auto code : machineCodeList) {
        if (code.empty() || code == "        ") continue;  // Skip empty codes

        // Convert hex string to unsigned int for binary output
        unsigned int machineCode = static_cast<unsigned int>(stoi(hexadecimalToDecimal(code)));

        // Write binary data to .o file
        coutMCode.write(reinterpret_cast<const char*>(&machineCode), sizeof(unsigned int));
    }
    coutMCode.close();  // Close .o file
    cout << "Machine code (.o) file generated" << endl;



   }
   else{
    PRINT_ERRORS();
   }
   readLines.clear();
   listingEntries.clear();
   machineCodeList.clear();

   return 0;
}
