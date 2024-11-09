/**************************************************************/

/*  Author - Shivam Singh;
  Roll Number - 2301CS51
     B.Tech 2nd Year,
Computer Science and Engineering
Indian Institute of Technology Patna (IIT-P)  */

/**************************************************************/

#include <bits/stdc++.h>
using namespace std;

// Class to represent a Label in assembly code
class Label
{
public:
    string labelName; // Name of the label (e.g., "LOOP")
    int labelAddress; // Address associated with the label in memory
    int lineNumber;   // Line number in the source code where the label is defined
};

// Class to represent a Mnemonic in assembly code
class Mnemonic
{
public:
    string opCode; // Operation code or mnemonic (e.g., "MOV", "ADD")
    int type;      // Type of the mnemonic (could represent instruction category or addressing mode)
};

// Class to represent a single instruction in assembly code
class Instruction
{
public:
    string mnemonic;         // The mnemonic or opcode of the instruction (e.g., "ADD")
    vector<string> operands; // List of operands associated with the instruction (e.g., {"R1", "R2"})
    int lineNumber;          // Line number in the source code where the instruction appears
};

vector<string> MachineCode; // Final generated machine code as a sequence of strings
vector<string> ListingFile; // Final listing file, containing the assembly source and its metadata

map<string, int> declaredLabels; // All declared labels with their corresponding memory addresses
map<string, bool> UsedLabels;    // Tracks whether each declared label has been used

vector<Label> labels;             // Collection of all label objects defined in the code
vector<Instruction> instructions; // Collection of all instructions parsed from the assembly code

vector<pair<int, string>> Warning; // All warnings, stored as pairs of line number and warning message
vector<pair<int, string>> Errors;  // All errors, stored as pairs of line number and error message

map<string, string> symbolTable; // Symbol table for identifiers, mapping names to values or types
vector<string> lsContent;        // Raw content of the listing file as lines of text

// Initialize opTable with a lambda function
map<string, Mnemonic> opTable = []
{
    map<string, Mnemonic> m;
    // Type = 0 (no operand)
    m["add"] = {"06", 0};
    m["sub"] = {"07", 0};
    m["shl"] = {"08", 0};
    m["shr"] = {"09", 0};
    m["a2sp"] = {"0B", 0};
    m["sp2a"] = {"0C", 0};
    m["return"] = {"0E", 0};
    m["halt"] = {"12", 0};

    // Type = 1 (argument is given)
    m["data"] = {"", 1};
    m["ldc"] = {"00", 1};
    m["adc"] = {"01", 1};
    m["set"] = {"", 1};
    m["adj"] = {"0A", 1};

    // Type = 2 (offset to be given)
    m["ldl"] = {"02", 2};
    m["stl"] = {"03", 2};
    m["ldnl"] = {"04", 2};
    m["stnl"] = {"05", 2};
    m["call"] = {"0D", 2};
    m["brz"] = {"0F", 2};
    m["brlz"] = {"10", 2};
    m["br"] = {"11", 2};
    return m;
}();

// Function to trim leading and trailing whitespace
string trim(const string &str)
{
    size_t start = str.find_first_not_of(" \t\n\v\f\r");
    size_t end = str.find_last_not_of(" \t\n\v\f\r");

    return (start == string::npos) ? "" : str.substr(start, end - start + 1);
}
// Function to split the instruction to Mnemonic  && operands
Instruction splitInstruction(string &instruction)
{
    instruction = trim(instruction); // Remove any leading or trailing whitespace from the instruction
    Instruction code;                // Initialize an Instruction object to store the parsed mnemonic and operands
    if (instruction == "")
    {
        code.mnemonic = "";
        code.operands = {""};
        return code;
    }
    int idxSpace = instruction.find(" "); // Find the first space, which separates the mnemonic from operands
    if (idxSpace == -1)                   // If no space is found, the instruction only contains a mnemonic
    {
        idxSpace = instruction.size(); // Set idxSpace to the size of the string, so the whole string is treated as the mnemonic
    }
    code.mnemonic = instruction.substr(0, idxSpace); // Extract the mnemonic from the beginning up to the space
    for (char &c : code.mnemonic)
    {
        c = tolower(c);
    }
    string operand = "";                                    // Temporary string to hold each operand
    for (int i = idxSpace + 1; i < instruction.size(); i++) // Loop through the rest of the instruction
    {
        if (instruction[i] == ',') // If a comma is encountered, it signifies the end of an operand
        {
            if (trim(operand) != "") // Only add non-empty operands
            {
                code.operands.push_back(trim(operand)); // Add trimmed operand to the vector
            }
            operand = ""; // Reset the operand string for the next operand
        }
        else
        {
            operand += instruction[i]; // Append characters to build the operand
        }
    }
    if (trim(operand) != "") // Add the last operand if it's non-empty
    {
        code.operands.push_back(trim(operand));
    }
    return code; // Return the populated Instruction object
}

bool CheckifChar(char x)
{
    return ((x >= 'A' && x <= 'Z') || (x >= 'a' && x <= 'z'));
}
bool CheckifNum(char x)
{
    return (x >= '0' && x <= '9');
}
string intToHex32(int number)
{
    char buffer[9];                  // 8 characters for 32 bits + 1 for the null terminator
    sprintf(buffer, "%08X", number); // %08X formats as 8-character uppercase hex, padded with 0s
    return string(buffer);
}
bool isLabelNameValid(string &name)
{
    if (name.empty())
        return false;
    bool ans = true;
    ans &= CheckifChar(name[0]);
    // return ans;
    if (ans == false)
    {
        return ans;
    }
    for (int i = 0; i < name.size(); i++)
    {
        ans &= (CheckifChar(name[i]) || CheckifNum(name[i]) || name[i] == '_');
    }
    return ans;
}
bool isHexadecimal(string s)
{
    bool ok = true;
    if (s.size() < 3)
        return false;
    ok &= (s[0] == '0');
    ok &= (s[1] == 'x');
    for (int i = 2; i < s.size(); i++)
    {
        bool check = (s[i] >= '0' && s[i] <= '9');
        check |= (s[i] >= 'a' && s[i] <= 'f');
        ok &= check;
    }
    return ok;
}
bool isOctal(string s)
{
    if (s.size() < 2)
        return false;
    bool ok = true;
    ok &= (s[0] == '0');
    for (int i = 1; i < s.size(); i++)
    {
        ok &= (s[i] >= '0' && s[i] <= '7');
    }
    return ok;
}
bool isDecimal(string s)
{
    bool ok = true;
    if (s.empty())
        return false;
    // Allow optional '+' or '-' sign at the beginning
    int start = (s[0] == '+' || s[0] == '-') ? 1 : 0;
    if (start == 1 && s.size() == 1)
        return false; // only sign without digits is invalid

    for (int i = start; i < s.size(); i++)
    {
        bool check = (s[i] >= '0' && s[i] <= '9');
        ok &= check;
    }
    return ok;
}
bool isString(string s)
{

    bool ok = true;
    ok &= !(isHexadecimal(s));
    ok &= !(isOctal(s));
    ok &= !(isDecimal(s));

    return ok;
}
void detectError(vector<Label> &labels, vector<Instruction> &instructions)
{
    // Detect Error in label  :-Duplicate labels && valid label Name
    for (int i = 0; i < labels.size(); i++)
    {
        if (declaredLabels.count(labels[i].labelName) > 0)
        {
            Errors.push_back({labels[i].lineNumber, labels[i].labelName + " is redeclared here"});
        }
        else
        {
            if (isLabelNameValid(labels[i].labelName) == false)
            {
                Errors.push_back({labels[i].lineNumber, labels[i].labelName + " is an invalid  label Name"});
            }
            declaredLabels[labels[i].labelName] = labels[i].lineNumber;
        }
    }
    // Detect Error in instructions

    for (int i = 0; i < instructions.size(); i++)
    {
        // Detect unknown menumonics
        if (instructions[i].mnemonic == "" && instructions[i].operands[0] == "")
        {
            continue;
        }
        if (opTable.count(instructions[i].mnemonic) == 0)
        {
            Errors.push_back({instructions[i].lineNumber, instructions[i].mnemonic + " is not a valid mnemonic "});
            continue;
        }

        // Detect unknown labels or incorrect numbers of operands

        if (instructions[i].operands.size() == 1 && isString(instructions[i].operands[0]))
        {
            if (declaredLabels.count(instructions[i].operands[0]) == 0)
            {
                Errors.push_back({instructions[i].lineNumber, instructions[i].operands[0] + " is not  defined "});
            }
            else
            {
                UsedLabels[instructions[i].operands[0]] = true;
            }
        }
        // Detect valid hexadecimal , octal , decimal or not
        if (instructions[i].operands.size() == 1 && opTable[instructions[i].mnemonic].type && (instructions[i].mnemonic != "br" && instructions[i].mnemonic != "brz" && instructions[i].mnemonic != "brlz" && instructions[i].mnemonic != "call"))
        {
            string operand = instructions[i].operands[0];
            if (operand.substr(0, 2) == "0x" && !isHexadecimal(operand))
            {
                Errors.push_back({instructions[i].lineNumber, instructions[i].operands[0] + " is not a valid Hexadecimal Number "});
            }
            else if (!isOctal(operand) && operand.substr(0, 2) != "0x" && operand.substr(0, 1) == "0" && operand.size() > 1)
            {
                Errors.push_back({instructions[i].lineNumber, instructions[i].operands[0] + " is not a valid Octal Number "});
            }
            else if (!isDecimal(operand) && operand.substr(0, 2) != "0x" && operand.substr(0, 1) != "0" && (operand[0] == '+' || operand[0] == '-' || (operand[0] >= '0' && operand[0] <= '9')))
            {
                Errors.push_back({instructions[i].lineNumber, instructions[i].operands[0] + " is not a valid Decimal Number "});
            }
        }

        int requiredOperands = opTable[instructions[i].mnemonic].type;

        if (requiredOperands == 2)
        {
            requiredOperands = 1;
        }
        // detect errors :- No of operands
        if (instructions[i].operands.size() > requiredOperands)
        {
            Errors.push_back({instructions[i].lineNumber, "Expected " + to_string(requiredOperands) + " arguments found more arguments"});
        }
        if (instructions[i].operands.size() < requiredOperands)
        {
            Errors.push_back({instructions[i].lineNumber, "Expected " + to_string(requiredOperands) + " arguments found less  arguments"});
        }
    }

    for (auto &label : declaredLabels)
    {
        if (UsedLabels.count(label.first) == 0)
        {
            Warning.push_back({label.second, label.first + " is defined but never used"});
        }
    }
}
string hexToBinary(const string &hexStr)
{
    // Convert hex string to an integer, then to a 32-bit binary string
    unsigned int hexValue = stoul(hexStr, nullptr, 16);
    return bitset<32>(hexValue).to_string();
}
void displayError(FILE *&logFile)
{
    sort(Errors.begin(), Errors.end());
    cerr << "Failed to Assemble!!" << endl;
    fprintf(logFile, "Failed to Assemble!! \n");

    for (auto x : Errors)
    {
        cerr << "Error at Line " << x.first << " : " << x.second << endl;
        fprintf(logFile, "Error at Line %d : %s\n", x.first, x.second.c_str());
        // coutLog << "Error at Line " << x.first << " : " << x.second << endl;
    }
    exit(0);
}
void displayWarning(FILE *&logFile)
{
    if (Warning.empty())
    {
        return;
    }
    sort(Warning.begin(), Warning.end());
    cerr << "Warnings " << endl;
    fprintf(logFile, "Warnings \n");

    for (auto x : Warning)
    {
        cerr << "Warning at Line " << x.first << " : " << x.second << endl;
        fprintf(logFile, "Warning at Line %d : %s\n", x.first, x.second.c_str());
    }
}

void listLSFile(FILE *&listFile)
{
    fprintf(listFile, "Symbol Table \n");
    for (auto symbol : symbolTable)
    {
        fprintf(listFile, "%s %s \n", symbol.first.c_str(), symbol.second.c_str());
    }
    fprintf(listFile, "\n Machine Code \n");

    for (auto lsLine : lsContent)
    {
        if (lsLine == " ")
        {
            fprintf(listFile, " \n");
            continue;
        }
        fprintf(listFile, "%s \n", lsLine.c_str());
    }
}

void listMachineFile(FILE *&machineCodeFile)
{
    // fprintf(machineCodeFile)
    for (auto &machine : MachineCode)
    {
        fprintf(machineCodeFile, "%s \n", machine.c_str());
    }
}

void pass2()
{
    // pass 2: Symbol table and machine code
    // int n=0x23A;
    // cout << "Symbols" << endl;
    for (auto &label : labels)
    {
        // cout<<"symbols"<<endl;
        symbolTable[label.labelName] = intToHex32(label.labelAddress);
        // cout << label.labelName << " " << symbolTable[label.labelName] << endl;
    }

    // convert to machine code
    // cout << " machine code" << endl;
    for (auto &instruction : instructions)
    {
        if (instruction.mnemonic == "" && instruction.operands[0] == "")
        {
            lsContent.push_back(" ");
            MachineCode.push_back(" ");
            continue;
        }
        string machine = "";
        string opCode = opTable[instruction.mnemonic].opCode;
        string operand = "000000";
        if (instruction.mnemonic == "data" || instruction.mnemonic == "set")
        {
            opCode = "";
            // string operand ="";

            string op = instruction.operands[0];

            int value = 0;
            // cout<<op<<endl;
            // Determine the base based on the prefix
            // op.find("0")
            if (op.find("0x") == 0)
            {
                // Hexadecimal
                value = stoi(op, nullptr, 16);
            }
            else if (op[0] == '0' && op.size() > 1)
            {
                // Octal (leading zero)
                value = stoi(op, nullptr, 8);
            }
            else
            {
                // Decimal
                value = stoi(op, nullptr, 10);
            }

            if (value < 0)
            {
                value = (1 << 24) + value; // Adjust negative values to 24-bit two's complement
            }
            value = value & 0xFFFFFF;

            char buffer[9];                 // 6 characters for 24-bit hex + 1 for null terminator
            sprintf(buffer, "%08X", value); // Convert to uppercase hex
            operand = string(buffer);
        }
        // string operand="000000";

        if (instruction.operands.size() == 1 && instruction.mnemonic != "data" && instruction.mnemonic != "set")
        {
            if (isString(instruction.operands[0]))
            {
                operand = symbolTable[instruction.operands[0]].substr(2); // truncate upper 8 bit
            }
            else
            {
                string op = instruction.operands[0];

                int value = 0;
                // cout<<op<<endl;
                // Determine the base based on the prefix
                // op.find("0")
                if (op.find("0x") == 0)
                {
                    // Hexadecimal
                    value = stoi(op, nullptr, 16);
                }
                else if (op[0] == '0' && op.size() > 1)
                {
                    // Octal (leading zero)
                    value = stoi(op, nullptr, 8);
                }
                else
                {
                    // Decimal
                    value = stoi(op, nullptr, 10);
                }

                if (value < 0)
                {
                    value = (1 << 24) + value; // Adjust negative values to 24-bit two's complement
                }
                value = value & 0xFFFFFF;

                char buffer[7];                 // 6 characters for 24-bit hex + 1 for null terminator
                sprintf(buffer, "%06X", value); // Convert to uppercase hex
                operand = string(buffer);
            }
        }
        machine = operand + opCode;
        MachineCode.push_back(hexToBinary(machine));
        string pc = intToHex32(instruction.lineNumber);

        string lsLine = "";
        lsLine += pc + " " + machine + " " + instruction.mnemonic + " ";

        if (instruction.operands.size() == 1)
        {
            lsLine += instruction.operands[0];
        }
        lsContent.push_back(lsLine);
        // cout << lsLine << endl;
    }
}
int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        cerr << "Error : Two arguments are nedded ";
        return 1;
    }

    if (strlen(argv[1]) < 5 || strcmp(".asm", argv[1] + strlen(argv[1]) - 4))
    {
        cerr << "Error : Input file must be a file with .asm extension ";
        return 1;
    }

    FILE *asmFile = NULL;
    asmFile = fopen(argv[1], "r");

    if (asmFile == NULL)
    {
        cerr << "Error : Unable to open file " << argv[1] << endl;
        // return 1;
        exit(0);
    }
    string nameWithoutasm = "";
    int i = 0;
    while (argv[1][i] != '.')
    {
        nameWithoutasm += argv[1][i];
        i++;
    }
    string lgfile, lsfile, mcfile;
    lgfile = nameWithoutasm + "log.txt";
    lsfile = nameWithoutasm + ".l";
    mcfile = nameWithoutasm + ".o";
    FILE *logFile = fopen(lgfile.c_str(), "w");
    if (!asmFile)
    {
        cerr << "Error: Could not open file " << argv[1] << endl;
        return 1;
    }

    int maxLineLength = 100;

    char line[maxLineLength];

    vector<string> lines;
    while (fgets(line, maxLineLength, asmFile) != NULL)
    {

        string lineStr(line);

        // Trim whitespace from the line
        lineStr = trim(lineStr);

        // Only add the line if it is not empty after trimming
        // if (!lineStr.empty() )
        // {
        lines.push_back(lineStr);
        // }
    }

    int labelAddress = 0;
    int lineNumber = 1;
    for (int i = 0; i < lines.size(); i++)
    {
        // cout<<lines[i];
        if (lines[i] == "")
        {
            lineNumber++;
            // labelAddress += 1;
            continue;
        }

        int idxColon = lines[i].find(':');
        int idxSemiColon = lines[i].find(';');

        if (idxColon == -1)
        {
            idxColon = lines[i].size();
        }

        if (idxSemiColon == -1)
        {
            idxSemiColon = lines[i].size();
        }
        string label;
        string instruction;
        if (idxSemiColon <= idxColon)
        {
            instruction = lines[i].substr(0, idxSemiColon);
        }
        else
        {
            label = lines[i].substr(0, idxColon);
            instruction = lines[i].substr(idxColon + 1, idxSemiColon - idxColon - 1);
        }
        label = trim(label);
        instruction = trim(instruction);
        // cout << "label:" << label << endl
        //      << "ini:" << instruction << endl;
        if (label == "" && instruction == "")
        {
            lineNumber++;
            continue;
        }
        if (label != "")
        {
            labels.push_back({label, labelAddress, lineNumber});
        }
        if (label != "" || instruction != "")
        {
            Instruction code = splitInstruction(instruction);
            code.lineNumber = labelAddress;
            instructions.push_back(code);
        }
        // cout<<endl;
        labelAddress += 1;
        lineNumber++;
    }

    for (auto &i : labels)
    {
        // cout << i.labelName << " " << i.labelAddress << endl;
    }
    // cout << endl;

    for (auto &code : instructions)
    {
        // cout << "Mnemonics: " << code.mnemonic << " Operands:";
        for (auto &i : code.operands)
        {
            // cout << i << " ";
        }
        // cout << endl;
    }

    /**************************************************************/
    /*Error generation*/
    /**************************************************************/
    detectError(labels, instructions);

    /**************************************************************/
    /* Log File Generation*/
    /**************************************************************/
    if (Errors.size() > 0)
    {
        displayError(logFile);
    }
    if (Warning.size())
    {
        displayWarning(logFile);
    }

    FILE *listFile = fopen(lsfile.c_str(), "w");
    FILE *machineCodeFile = fopen(mcfile.c_str(), "w");
    pass2();

    /**************************************************************/
    /*Listing and Object File generation*/
    /**************************************************************/
    listLSFile(listFile);

    listMachineFile(machineCodeFile);

    cerr << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    cerr << "Compiled without errors!!!" << endl;
    cerr << "Logs generated in: " + nameWithoutasm + "log.txt" << endl;
    cerr << "Machine code generated in: " + nameWithoutasm + ".o" << endl;
    cerr << "Listing generated in: " + nameWithoutasm + ".l" << endl;
    cerr << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

    /**************************************************************/

    /*Cleanup and file closing*/
    fclose(asmFile);
    fclose(logFile);
    fclose(listFile);
    fclose(machineCodeFile);

    return 0;
}