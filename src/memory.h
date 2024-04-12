#ifndef MEMORY_H
#define MEMORY_H

#include <iostream>
#include <string>
#include <map>

using namespace std;

class Memory {
    private:
        map<string, string> memory;

    public:
        Memory();  // Constructor
        void allocate_data (string data, int line_start);
        void allocate_variable (string variable_name);
        int locate_contiguous_memory (int instruction_count);
        void set_data (string data, string address);
        void set_variable_data (string variable_name, string variable_data);
        string get_data(string address);
        string get_variable_data (string variable_name);
};

#endif  // MEMORY_H