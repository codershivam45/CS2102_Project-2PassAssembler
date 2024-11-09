/**************************************************************/

                /*  Author - Shivam Singh;
                Roll Number - 2301CS51
                    B.Tech 2nd Year,
                Computer Science and Engineering
                Indian Institute of Technology Patna (IIT-P)  */

/**************************************************************/
#include <bits/stdc++.h>
#include <fstream>
#define int long long
using namespace std;



map<string, string> mnemonic = {
    {"00", "ldc"}, {"01", "adc"}, {"02", "ldl"}, {"03", "stl"}, {"04", "ldnl"}, {"05", "stnl"}, {"06", "add"}, {"07", "sub"}, {"08", "shl"}, {"09", "shr"}, {"0A", "adj"}, {"0B", "a2sp"}, {"0C", "sp2a"}, {"0D", "call"}, {"0E", "return"}, {"0F", "brz"}, {"10", "brlz"}, {"11", "br"}, {"12", "halt"}, {"", "data"}, {"", "set"}
    };
int A,B,SP,PC,countPC;
int const N=1<<24;
int memory[N];
bool stopPrint=0;
vector<string> instructions;
void executeInstruction(int opcode, int n)
{
    switch (opcode)
    {
    case 0: // ldc
        B = A;
        A = n;
        break;
    case 1: // adc
        // B=A;
        // cout << A << " " << n << endl;
        A += n;
        
        break;
    case 2: // ldl
        B = A;
        if (SP + n >= N || SP + n < 0)
        {
            cout << "Segmentation fault: Out of range memory access\n";
            exit(0);
        }
        A = memory[SP + n];
        break;
    case 3: // stl
        if (SP + n >= N || SP + n < 0)
        {
            cout << "Segmentation fault: Out of range memory access\n";
            exit(0);
        }
        memory[SP + n] = A;
        A = B;
        break;
    case 4: // ldnl
        if (A + n >= N || A + n < 0)
        {
            cout << "Segmentation fault: Out of range memory access\n";
            exit(0);
        }
        A = memory[A + n];
        break;
    case 5: // stnl
        if (A + n >= N || A + n < 0)
        {
            cout << "Segmentation fault: Out of range memory access\n";
            exit(0);
        }
        memory[A + n] = B;
        break;
    case 6: // add
        A = B + A;
        break;
    case 7: // sub
        A = B - A;
        break;
    case 8: // shl
        A = B << 1;
        break;
    case 9: // shr
        A = B >> 1;
        break;
    case 10: // adj
        SP = SP + n;
        break;
    case 11: // a2sp
        SP = A;
        A = B;
        break;
    case 12: // sp2a
        B = A;
        A = SP;
        break;
    case 13: // call
        B = A;
        A = PC;
        PC = PC + n;
        break;
    case 14: // return1
        PC = A;
        A = B;
        break;
    case 15: // brz
        if (A == 0)
            PC = PC + n;
        break;
    case 16: // brlz
        if (A < 0)
            PC = PC + n;
        break;
    case 17: // br
        PC = PC + n;
        break;
    case 18: // halt
        PC = countPC;
        break;
    default:
        cout << "Invalid opcode: " << opcode << "\n";
        exit(0);
    }
}
string binaryToHex(const string &binStr)
{
    // Convert binary string to an integer and format it as an 8-character hex string
    unsigned int binValue = bitset<32>(binStr).to_ulong();
    char hexStr[9];
    snprintf(hexStr, sizeof(hexStr), "%08X", binValue); // Use %08X to ensure 8 characters in uppercase
    return string(hexStr);
}
int hextoint(const string & hexStr)
{
        // Convert hex string to integer
        int num = stoi(hexStr, nullptr, 16);

        // Check if the number is negative in 24-bit two's complement
        if (num >= (1 << 23))
        {
            // Adjust for negative values
            num -= (1 << 24);
        }

        return num;
}
int hextoint32(const string & hexStr)
{
        // Convert hex string to integer
        int num = stoll(hexStr, nullptr, 16);

        // Check if the number is negative in 24-bit two's complement
        if (num >= (1L << 31))
        {
            // Adjust for negative values
            num -= (1L << 32);
        }

        return num;
}


string intToHex32(int number)
{
    char buffer[9];                  // 8 characters for 32 bits + 1 for the null terminator
    sprintf(buffer, "%08X", number); // %08X formats as 8-character uppercase hex, padded with 0s
    return string(buffer);
}
// Function to trim leading and trailing whitespace
string trim(const string &str)
{
    size_t start = str.find_first_not_of(" \t\n\v\f\r");
    size_t end = str.find_last_not_of(" \t\n\v\f\r");

    return (start == string::npos) ? "" : str.substr(start, end - start + 1);
}
void printInstruction(string opCode,string operand){
    cout<<mnemonic[opCode]<<" ";
    // print instruction
    if (mnemonic[opCode] == "add" || mnemonic[opCode] == "sub" || mnemonic[opCode] == "shl" || mnemonic[opCode] == "shr" || mnemonic[opCode] == "a2sp" || mnemonic[opCode] == "sp2a" || mnemonic[opCode] == "return" || mnemonic[opCode]== "halt" ){
        cout<<endl;
        return ;
    }
    cout<<operand<<endl;
}
void performInstruction(){
    
    string instruction=instructions[PC];
    if(instruction==" "){
        PC++;
        return ;
    }
    // cout<<instruction<<endl;
    string opCode=instruction.substr(6);
    string operand=instruction.substr(0,6);
    if(mnemonic.count(opCode)==0){
        cout<<opCode<<endl;
        cout<<"Invalid instruction : this instruction doesn't exists ";
        exit(0);
    }
    if(stopPrint==false){
        printf("PC=%08X    %s\t", PC, instructions[PC].c_str());
        printInstruction(opCode, operand);
    }
    if(opCode=="0D" || opCode=="0F" || opCode=="10" || opCode=="11"){
        executeInstruction(hextoint(opCode), hextoint(operand)-PC-1);
    }else{
        executeInstruction(hextoint(opCode), hextoint(operand));
       
    }
    PC++;
}
void printState(){
    printf("A=%08X \t B=%08X \t SP=%08X \t PC=%08X\n", A, B, SP, PC);
}
signed main(signed argc, char * argv[]){

    cout << "Welcome to the Emulator Command Help\n"
              << "Available commands:\n"
              << "  -t       : Single or Multiple trace operation\n"
              << "  -f       : Full program trace\n"
              << "  -bd      : Memory dump\n"
              << "  -data    : Show data at specific location\n"
              << "  -ad      : Execute all instructions and then memory dump\n"
              << "  -u       : Show future instructions\n\n"
              << "Enter a command , or type any other command to quit.\n";
    A = B = PC = SP = 0;

    FILE * machineFile;

    // Open binary file specified as command-line argument
    machineFile=fopen(argv[1],"r");

    if (!machineFile)
    {
        cerr << "Error: Could not open file " << argv[1] << endl;
        return 1;
    }

    // char line[9];
    // string line;
    
    int num = 0, flag = 0;

    countPC=0;
    // cout<<countPC<<endl;
    // cout<<"f"<<endl;
    char line[256];
    while (fgets(line,256,machineFile))
    {
        // cout << line;
        // cout<<line;
        string lineStr=trim(line);
        if(trim(line)==""){
            instructions.push_back(" ");
            memory[countPC]=0;
            countPC++;
            continue;
        }
        lineStr=binaryToHex(lineStr);
        instructions.push_back(lineStr);
        memory[countPC]=hextoint32(lineStr);
        // cout<<" m "<<memory[countPC]<<" "<<countPC<<endl;
        
        countPC++;
        // cout <<" countpc :"<<countPC<<endl;
    }
    for(auto &l: instructions){
        // cout<<l<<endl;
    }
    // cout<<countPC<<endl;
    string input;
    string command;
    int number;
    while (PC < countPC){

        getline(cin, input);
        stringstream inputStream(input);
        inputStream >> command ;

        if(command== "-t"){
            // cout<<command<<endl;
            if(inputStream.eof()){
                
                performInstruction();
                printState();

            }else{

                inputStream>>number;
                for(int i=0;i<number ;i++){
                    performInstruction();
                    printState();
                }
            }
        }else if(command=="-f"){
            while(PC<countPC){
                performInstruction();
                printState();
            }
        }else if(command=="-u"){
           int currentPC=PC;

           for(int i=0 ;i< 10 && PC!=countPC;i++){
               string instruction = instructions[PC];

               string opCode = instruction.substr(6);
               string operand = instruction.substr(0, 6);

               printf("PC=%08X    %s\t", (unsigned int)PC, instructions[PC].c_str());
               printInstruction(opCode, operand);
               PC++;
           }
           PC=currentPC;
        }else if(command=="-bd"){

            // Print the memory content in group of four
            for (int i = 0; i < countPC; i++)
            {
                printf("%08x\t", i); // Print the current memory address
                for (int j = 0; j < 4; j++)
                {

                    printf("%08X\t", memory[i]); // Print memory content at this address
                    i++;
                    if (i >= countPC)
                        break;
                }
                printf("\n");
                i--; // Adjust 'i' to not skip an index in the outer loop
            }
        }else if(command=="-data"){

            int number;
            inputStream>>hex>>number;

            if (num < 0 || num >= N)
            {
                cout << "Warning: Out of bound memory fetch\n";
            }
            else
            {
                printf("[%08X] = %08X\n", number, memory[number]);
            }
        }else if(command == "-ad"){
            stopPrint = 1;
            cout << "DATA SEGMENT : \n";
            while (PC < countPC)
            {
                performInstruction();
                // printState();
            }
           for (int i = 0; i < countPC; i+=0)
           {
               printf("%08x\t", i);
               for (int j = 0; j < 4; j++)
               {
                   printf("%08X ", memory[i]);
                   i++;
                   if (countPC < i)
                       break;
               }
               printf("\n");
           }
        }else{
            cout << "wrong_command" << endl;
            exit(0);
        }
    }

    cout<<"All instructions have been runned \n";

    
    return 0;
}