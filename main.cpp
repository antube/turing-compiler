#include <iostream>
#include <fstream>
#include "structs/card.h"

#define INSTRUCTION_LENGTH 12


int main(int argc, char *argv[])
{
    std::ifstream tmfile;
    try
    {
        tmfile.open(argv[1], std::ios::in);
    }
    catch(...)
    {
        std::cout << "ERROR: Source File open failed" << std::endl;
    }

    std::ofstream tmofile;
    try
    {
        tmofile.open((argv[1] + 'o'), std::ios::binary | std::ios::out | std::ios::trunc);
    }
    catch(...)
    {
        std::cout << "ERROR: Creation of object file failed" << std::endl;
    }


    while(!tmfile.eof())
    {
        char* line = new char[INSTRUCTION_LENGTH];

        tmfile.read(line, INSTRUCTION_LENGTH);

        try
        {
            
        }
        catch(const char* msg)
        {
            std::cout << msg << std::endl;
        }
    }
    
    return 0;
}