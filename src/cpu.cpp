#include <bits/stdc++.h>
#include "memory.h"

using namespace std;

//note: declaring a variable has the register code 00
//general purpose registers (0001 - 1000)
map<string, int> general_purpose_registers {{"RR0", 0}, {"RR1", 0}, {"RR2", 0}, {"RR3", 0},
                                        {"RR4", 0}, {"RR5", 0}, {"RR6", 0}, {"RR7", 0}};

//Main Registers
map<string, int> main_registers {
        {"ACX", 0},     //accumulator (1001)
        {"PCX", 0},     //program counter (1010)
        {"ADX", 0},     //data register (1011)
        {"ABX", 0},     //base register (1100)
};

string IRX = "";        //instruction register (1101)


// virtual memory
int mem_size = 64 * 32;  //32kb memory module

Memory::Memory () { //constructor for memory to initialize empty addresses
    for (int i = 0; i < mem_size; i += 1) {
        string current_address = to_string(i);
        while (current_address.size() < 6) {
            current_address = "0" + current_address;
        }
        current_address = "0x" + current_address;
        set_data("", current_address);
    }
}

void Memory::allocate_data (string data, int line_start) { //locating free memory to store data
    for (int i = line_start; i < mem_size; i += 1) {
        string current_address = to_string(i);
        if (i > mem_size) {
            cerr << "OVERFLOW" << '\n';
            return;
        }
        while (current_address.size() < 6) {
            current_address = "0" + current_address;
        }
        current_address = "0x" + current_address;
        if (memory[current_address] == "") {
            set_data(data, current_address);
            break;
        }
    }
}

int Memory::locate_contiguous_memory (int instruction_count) { //locating free memory to store data
    int memory_count = 0, memory_address = 0;
    for (int i = 0; i < mem_size; i += 1) {
        string current_address = to_string(i);
        if (memory_count == instruction_count) {
            break;
        }
        if (i > mem_size) {
            cerr << "OVERFLOW" << '\n';
            return 0;
        }
        while (current_address.size() < 6) {
            current_address = "0" + current_address;
        }
        current_address = "0x" + current_address;
        if (memory[current_address] == "") {
            memory_count += 1;
            if (memory_count == 1) memory_address = i;
        } else {
            memory_count = 0;
        }
    }
    return memory_address;
}

void Memory::allocate_variable (string variable_name) {
    this->memory[variable_name] = "";
}

string Memory::get_variable_data (string variable_name) {
    return this->memory[variable_name];
}

string Memory::get_data (string address) {
    return this->memory[address];
}

void Memory::set_variable_data (string variable_name, string variable_data) {
    this->memory[variable_name] = variable_data;
}

void Memory::set_data (string data, string address) {
    this->memory[address] = data;
}

//Parsing
                        /*   00     01     10    11     100    101    110    111    1000   1001   1010 */
vector<string> instructions{"MOV", "ADD", "SUB", "MUL", "DIV", "MOD", "JIL", "JIG", "JIE", "JNE", "OUT"};                //available operations (opcode)
vector<string> registers{"RR0", "RR1", "RR2", "RR3", "RR4", "RR5", "RR6", 
                        "RR7", "ACX", "PCX", "ADX", "ABX"};   
vector<string> variable_names;

Memory memory;

string decimal_to_binary (int decimal, int type) {
    string binary = "";
    while (decimal > 0) {
        binary += to_string(decimal % 2);
        decimal >>= 1;
    }
    reverse(binary.begin(), binary.end());
    if (type == 1) {
        while (binary.size() < 16) {
            binary = "0" + binary;
        }
    } else {
        while (binary.size() < 4) {
            binary = "0" + binary;
        }
    }
    return binary;
}

int binary_to_decimal (string binary) {
    int counter = 1, decimal = 0;
    for (int i = binary.size() - 1; i >= 0; i -= 1) {
        if (binary[i] == '1') decimal += counter;
        counter <<= 1;
    }
    return decimal;
}

bool register_check (string data) {
    bool found = false;;
    for (int i = 0; i < registers.size(); i += 1) {
        if (registers[i] == data) {
            found = true;
            break;
        }
    }
    return found;
}

bool variable_check (string data) {
    bool found = false;;
    for (int i = 0; i < variable_names.size(); i += 1) {
        if (variable_names[i] == data) {
            found = true;
            break;
        }
    }
    return found;
}

void print_memory (int n) {

}

string parse (string instruction) {
    int current_index = 0;

    //get operation code from input and parse to its corresponding code according to the defined ISA
    string operation_code = "";
    for (current_index; current_index < 3; current_index += 1) {
        operation_code += instruction[current_index];
    }
    int instruction_index = -1;
    for (int i = 0; i < instructions.size(); i += 1) {
        if (instructions[i] == operation_code) {
            instruction_index = i;
            break;
        }
    }
    int jump_index = -1;
    string jump_size = "";;
    if (instruction_index == -1) {
        cerr << "Compilation error: You have entered an invalid operation." << '\n';
        return "";
    } 
    //have to parse branching differently because of an extra argument which is the size of the jump
    if (operation_code[0] == 'J') {
        jump_index = 1;
        int count = 0;
        int length = 2;
        for (int i = instruction.size() - 1; i > 0; i -= 1) {
            if (instruction[i] == ' ') break; 
            jump_size += instruction[i];
            length += 1;
        }
        reverse(jump_size.begin(), jump_size.end());
        while (length > 0) {
            instruction.pop_back();
            length -= 1;
        }
    }

    operation_code = decimal_to_binary(instruction_index, 2);

    //get register/variable from input and parse to its corresponding code according to the defined ISA
    string expression = "";
    current_index += 1;
    for (current_index; current_index < instruction.size(); current_index += 1) {
        if (instruction[current_index] == ',') break;
        expression += instruction[current_index];
    }
    int register_index = -1;
    for (int i = 0; i < registers.size(); i += 1) {
        if (registers[i] == expression) {
            register_index = i;
            break;
        }
    }

    if (register_index == -1) {
        mem_size -= 1;      //subtract from the memory size to make space for a variable
        memory.allocate_variable(expression);
        variable_names.push_back(expression);
        expression = "0000";
    } else {
        expression = decimal_to_binary(register_index + 1, 2);
    }
    
    //get the data and convert it to binary to store it to memory
    string data = (operation_code == "1010" ? "0" : "");
    current_index += 2;
    bool is_negative = false;
    for (current_index; current_index < instruction.size(); current_index += 1) {
        if (instruction[current_index] == ',') break;
        if (instruction[current_index] == '-') is_negative = true;      //determine the sign of the inputted value
        else data += instruction[current_index];
    }
    if (variable_check(data)) {
        int index = 0;
        for (int i = 0; i < variable_names.size(); i += 1) {
            if (variable_names[i] == data) {
                index = i;
                break;
            }
        }
        string parsed_instruction = "2" + operation_code + expression + decimal_to_binary(index, 1) + (jump_index != -1 ? " " + decimal_to_binary(stoi(jump_size) - 1, 1) : ""); 
        return parsed_instruction;
    } else if (register_check(data)) {
        int index = 0;
        for (int i = 0; i < registers.size(); i += 1) {
            if (registers[i] == data) {
                index = i;
                break;
            }
        }
        string parsed_instruction = "3" + operation_code + expression + decimal_to_binary(index + 1, 1) + (jump_index != -1 ? " " + decimal_to_binary(stoi(jump_size) - 1, 1) : "");  
        return parsed_instruction;
    }
    int value = stoi(data);
    data = decimal_to_binary(value, 1);
    string parsed_instruction = (is_negative ? "1" : "0") + operation_code + expression + data + (jump_index != -1 ? " " + decimal_to_binary(stoi(jump_size) - 1, 1) : ""); 
    return parsed_instruction;
}

string parse_address () {
    string address = to_string(main_registers["PCX"]);      
    while (address.size() < 6) {
        address = "0" + address;
    }
    address = "0x" + address;
    return address;
}

string parse_address_raw (int n) {      //for printing purposes
    string address = to_string(n);      
    while (address.size() < 6) {
        address = "0" + address;
    }
    address = "0x" + address;
    return address;
}

void execute_instruction (string operation_code, string expression, string data) {
    //place the data temporarily on a cpu register
    int value = IRX[0] - '0';
    if (value == 0 || value == 1) {
        //cout << data << '\n';
        general_purpose_registers["RR7"] = binary_to_decimal(data) * (IRX[0] == '1' ? -1 : 1);   
    } else if (value == 2) {
        string raw_data = memory.get_variable_data(variable_names[binary_to_decimal(data)]);
        general_purpose_registers["RR7"] = (raw_data[0] == '1' ? -1 : 1) * binary_to_decimal(raw_data.substr(1, 16)); 
    } else if (value == 3) {
        general_purpose_registers["RR7"] = (main_registers[registers[binary_to_decimal(data) - 1]]) * (IRX[0] == '1' ? -1 : 1);   
    }
    general_purpose_registers["RR6"] = 0;
    int register_code = binary_to_decimal(expression) - 1;

    if (operation_code == "0000") {
        if (IRX[0] == '1') {
            data = "1" + data;
        }
        if (expression == "0000") {
            memory.set_data(data, variable_names[general_purpose_registers["RR5"]]);
            general_purpose_registers["RR5"] += 1;
        } else {
            string REG = registers[register_code];
            main_registers[REG] = general_purpose_registers["RR7"];
        }

    } else if (operation_code == "0001") {

          //addition 
        string REG = registers[register_code];
        main_registers[REG] += general_purpose_registers["RR7"];

    } else if (operation_code == "0010") {

          //subtraction  
        string REG = registers[register_code];
        main_registers[REG] -= general_purpose_registers["RR7"];

    } else if (operation_code == "0011") {

        //multiplication
        string REG = registers[register_code];
        main_registers[REG] *= general_purpose_registers["RR7"];

    } else if (operation_code == "0100") {
        
        //division
        string REG = registers[register_code];
        main_registers[REG] /= general_purpose_registers["RR7"];
        
    } else if (operation_code == "0101") {
                
        //modulo
        string REG = registers[register_code];
        main_registers[REG] %= general_purpose_registers["RR7"];
        
    } else if (operation_code == "0110") {

        //jump if less than
        string REG = registers[register_code];
        main_registers["PCX"] += (main_registers[REG] < general_purpose_registers["RR7"] ? general_purpose_registers["RR3"] : 0);

    } else if (operation_code == "0111") {

        //jump if greater than
        string REG = registers[register_code];
        main_registers["PCX"] += (main_registers[REG] > general_purpose_registers["RR7"] ? general_purpose_registers["RR3"] : 0);

    } else if (operation_code == "1000") {
        
        //jump if equal
        string REG = registers[register_code];
        main_registers["PCX"] += (main_registers[REG] == general_purpose_registers["RR7"] ? general_purpose_registers["RR3"] : 0);

    } else if (operation_code == "1001") {

        //jump if not equal
        string REG = registers[register_code];
        main_registers["PCX"] += (main_registers[REG] != general_purpose_registers["RR7"] ? general_purpose_registers["RR3"] : 0);

    } else if (operation_code == "1010") {

        string REG = registers[register_code];
        cout << main_registers[REG] << '\n';

    }
}

void decode_instruction () {
    string operation_code = "", expression = "", data = "";
    for (int i = 1; i <= 4; i += 1) {
        operation_code += IRX[i];
    }
    for (int i = 5; i <= 8; i += 1) {
        expression += IRX[i];
    }
    for (int i = 9; i <= 24; i += 1) {
        data += IRX[i];
    }
    if (IRX.size() > 25) {
        string jump_size = "";
        for (int i = 25; i < IRX.size(); i += 1) {
            jump_size += IRX[i];
        }
        general_purpose_registers["RR3"] = binary_to_decimal(jump_size);
        //cout << jump_size << '\n';
    }
    execute_instruction(operation_code, expression, data);
}

//fetch from memory
void fetch_instruction () {
    string instruction = memory.get_data(parse_address());
    main_registers["PCX"] += 1;       //increment program counter
    IRX = instruction;  
    //cout << IRX << '\n';
    decode_instruction();
}

signed main() {
    fstream file("input.txt");
    if (!file.is_open()) {
        cerr << "Failed to open file." << '\n';
        return 1;
    }
    string buffer;
    int instruction_count = 0;
    vector<string> temporary_holder;
    while (getline(file, buffer)) {
        temporary_holder.push_back(buffer);
        instruction_count += 1;         //count the lines of instructions to be stored in memory 
    }
    //use RR1 as a counter for the number of lines of instructions from the input (total instructions = RR1 - PCX)
    main_registers["PCX"] = memory.locate_contiguous_memory(instruction_count), general_purpose_registers["RR1"] = main_registers["PCX"] + instruction_count; //store to memory the instructions
    
    //int n = main_registers["PCX"];
    
    bool invalid_argument = false;

    for (auto line : temporary_holder) {
        string data = parse(line);      //convert human readable instructions to binary
        if (data == "") {
            invalid_argument = true;
            break;
        }
        memory.allocate_data(data, main_registers["PCX"]);        //place the converted data to the located address(starting at first index of the contiguous memory)
    }

    if (invalid_argument) {
        return 0;
    }

    // for (int i = n; i < general_purpose_registers["RR1"]; i += 1) {
    //     cout << memory.get_data(parse_address_raw(i)) << '\n';;
    // }
    //start working on the instructions using fetch-decode-execute cycle
    while (main_registers["PCX"] < general_purpose_registers["RR1"]) {
        fetch_instruction();
    }
}