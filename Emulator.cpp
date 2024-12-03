/* 
NAME : Manshi Prajapati
ROLL NUMBER: 2302CS08
DECLARATION OF AUTHORSHIP: I HEREBY DECLARE THAT THE SOURCE CODE I AM SUBMITTING IS ENTIRELY MY OWN ORIGINAL WORK EXCEPT WHERE OTHERWISE INDICATED.
*/
#include<bits/stdc++.h>
using namespace std;
    int Main_Memory[1<<24];
    int Total_Number=0;
    int Stack_Limit=1<<23;
    int Program_Counter=0,Stack_Pointer=0,Register_A=0,Register_B=0;
    vector<int> File;

   
    // Define a temporary vector with the mnemonics and push them into the global vector
         vector<string> Mnemonics = {
        "ldc", "adc", "ldl", "stl", "ldnl", "stnl", "add", "sub", "shl", "shr",
        "adj", "a2sp", "sp2a", "call", "return", "brz", "brlz", "br", "HALT"
      };   
    
    void executeOpcode(int Op_Code, int operand) {
    if (Op_Code == 0) {
        Register_B = Register_A;
        Register_A = operand;
    } else if (Op_Code == 1) {
        Register_A += operand;
    } else if (Op_Code == 2) {
        Register_B = Register_A;
        Register_A = Main_Memory[Stack_Pointer + operand];
    } else if (Op_Code == 3) {
        Main_Memory[Stack_Pointer + operand] = Register_A;
        Register_A = Register_B;
    } else if (Op_Code == 4) {
        Register_A = Main_Memory[Register_A + operand];
    } else if (Op_Code == 5) {
        Main_Memory[Register_A + operand] =Register_B;
    } else if (Op_Code == 6) {
        Register_A = Register_B + Register_A;
    } else if (Op_Code == 7) {
        Register_A = Register_B - Register_A;
    } else if (Op_Code == 8) {
        Register_A = Register_B << Register_A;
    } else if (Op_Code == 9) {
        Register_A = Register_B >> Register_A;
    } else if (Op_Code == 10) {
        Stack_Pointer = Stack_Pointer + operand;
    } else if (Op_Code == 11) {
        Stack_Pointer = Register_A;
        Register_A = Register_B;
    } else if (Op_Code == 12) {
        Register_B = Register_A;
        Register_A = Stack_Pointer;
    } else if (Op_Code == 13) {
        Register_B = Register_A;
        Register_A = Program_Counter;
        Program_Counter = operand - 1;
    } else if (Op_Code == 14) {
        Program_Counter = Register_A;
        Register_A = Register_B;
    } else if (Op_Code == 15) {
        if (Register_A == 0) {
            Program_Counter = Program_Counter + operand;
        }
    } else if (Op_Code == 16) {
        if (Register_A < 0) {
            Program_Counter = Program_Counter + operand;
        }
    } else if (Op_Code == 17) {
        Program_Counter = Program_Counter + operand;
    } else {
        cout << "Invalid opcode. Incorrect machine code. Aborting";
        exit(0);
    }
}
pair<long, bool> read_operand (string &operand)
{
    if (!(int)operand.size()) {
        return {0, 0};
    }
    int len = (int)operand.size();
    char *str = (char *)malloc (len *  sizeof(char));
    for (int i = 0; i < (int)operand.size(); i++) {
        str[i] = operand[i];
    }
    for (int i = (int)operand.size(); i < strlen(str); i++) {
        str[i] = '\0';
    }
    char *end;
    long int num;
    num = strtol(str, &end, 10);
    if (!*end) {
        return {num, 1};
    }
    num = strtol(str, &end, 16);
    if (!*end) {
        return {num, 1};
    }
    return {-1, 0};
}
void Dump(){
    int Base_Address;
    cout<<"Base address: ";
    string Operand;
    cin>>Operand;
    pair<long,bool>it=read_operand (Operand);
    Base_Address=it.first;
    string Offset;
    cin>>Offset;
    pair<long,bool>itt=read_operand (Offset);
    for(int i=Base_Address;i<Base_Address+itt.first;i+=4){
        cout << hex << uppercase << setw(8) << setfill('0') << i << " "
     << setw(8) << setfill('0') << Main_Memory[i] << " "
     << setw(8) << setfill('0') << Main_Memory[i+1] << " "
     << setw(8) << setfill('0') << Main_Memory[i+2] << " "
     << setw(8) << setfill('0') << Main_Memory[i+3] << endl;

    }
}
bool Argument(){
    if(Program_Counter>=(int)File.size()){
        cout<<"Segmentation Fault"<<endl;
        exit(0);
    }
    int Op_Code=0xFF&File[Program_Counter];
    int Operand=File[Program_Counter]>>8;
    cout<<Mnemonics[Op_Code];
    cout << "\t" << setw(8) << setfill('0') << hex << uppercase << Operand <<endl;
    if(Op_Code==18){
        Total_Number++;
        return 0;;
    }
    executeOpcode(Op_Code, Operand);
    Total_Number++;
    Program_Counter++;
    if(Stack_Pointer>Stack_Limit){
        cout<<"Stack Overflow"<<endl;
         exit(0);
    }
    return 1;
}
bool Process(){
     cout<<"Input: ";
     string Input;
     cin>>Input;
     if(Input=="-t"||Input=="-T"){
        bool check=Argument();
        cout << "A = " << setw(8) << setfill('0') << uppercase <<hex << Register_A
          << ", B = " << setw(8) << setfill('0') << uppercase << hex << Register_B
          << ", PC = " << setw(8) << setfill('0') << uppercase << hex << Program_Counter
          << ", SP = " <<setw(8) << setfill('0') << uppercase << hex << Stack_Pointer << endl;
        if(!check){
            return false;
        }
     }
     else if(Input=="-all"||Input=="-ALL"){
        while(Argument()){
             cout << "A = " << setw(8) << setfill('0') << uppercase <<hex <<Register_A
          << ", B = " << setw(8) << setfill('0') << uppercase << hex << Register_B
          << ", PC = " << setw(8) << setfill('0') << uppercase << hex << Program_Counter
          << ", SP = " <<setw(8) << setfill('0') << uppercase << hex << Stack_Pointer << endl;
        }
        return false;
     }
     else if(Input == "-dump" || Input == "-DUMP"){
        Dump();
    }
     else{
        cout<<"Invalid Input"<<endl;
        return true;
     }
     return true;
   }
int main(){

    string INPUT_FILE2="machineCode.O";
   int _temp;
    ifstream current_file(INPUT_FILE2,ios::binary);
    if(!current_file){
        cout<<"Error in opening file"<<endl;
        return 0;
    }
   // File=vector<int>(istream_iterator<int>(current_file),istream_iterator<int>());
    while(current_file.read((char*) &_temp, sizeof(int))){ 
    	File.push_back(_temp);
    }
    current_file.close();
   
    int index=0;
    for(auto it:File){
        Main_Memory[index++]=it;
    }
   cout << "-t for trace\n-dump for memory dump\n-all for all commands\nEnter with hyphen\n";
   while(Process()){
    continue;
   }
   cout<<"Total number of instructions executed : "<<Total_Number<<endl;
   return 0;
}