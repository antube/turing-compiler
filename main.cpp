#include <iostream>
#include <fstream>
#include "structs/card.h"

#define INSTRUCTION_LENGTH 16

int ReadMovement(char);
instruction ReadInstruction(char*);

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << "" << std::endl;
    }
    else
    {
        //File Input Stream
        std::ifstream tmfile;
        try
        {
            //Open File Stream based for input on user specified file
            tmfile.open(argv[1], std::ios::in);
        }
        catch(...)
        {
            std::cout << "ERROR: Source File open failed" << std::endl;
        }

        //Output file stream
        std::ofstream tmofile;
        try
        {
            //Open ouput file stream as a binary file and truncate all contents
            tmofile.open((argv[1] + 'o'), std::ios::binary | std::ios::out | std::ios::trunc);
        }
        catch(...)
        {
            std::cout << "ERROR: Creation of object file failed" << std::endl;
        }

        int LineIndex = 0;

        //While not at end of file
        while(!tmfile.eof())
        {
            //Create char array size of Instruction
            char* line = new char[INSTRUCTION_LENGTH];

            //Read from input file into char array
            tmfile.read(line, INSTRUCTION_LENGTH);

            //
            LineIndex++;

            //If beggining of line is a '#' then move on to next line
            if(line[0] == '#') continue;

            //
            instruction inst;

            try
            {
                //
                inst = ReadInstruction(line);
            }
            catch(const char* msg)
            {
                //Return Message
                std::cout << "Issue at Line " << LineIndex << ": " << msg << std::endl;
                break;
            }

            //
            tmofile.write((char*) &inst, sizeof(instruction));
        }
    }   
    return 0;
}

instruction ReadInstruction(char* line)
{
    //
    instruction inst;

    //
    if(line[4] < 48 || line[4] > 49)
        //
        throw "Issue at Col 5";
    else
        //
        inst.Input = line[4] - 48;
    
    //
    if(line[7] < 48 || line[7] > 49)
        //
        throw "Issue at Col 8";
    else
        //
        inst.Output = line[7] - 48;
    
    //
    if(line[10] < 48 || line[10] > 57)
        //
        throw "Issue at Col 11";
    else
        //
        inst.NextState = line[10] - 48;

    try
    {
        inst.Movement = ReadMovement(line[13]);
    }
    catch(const char* msg)
    {
        throw msg;
    }

    return inst;
}

int ReadMovement(char move)
{
    switch(move)
    {
        //Left : -1
        case '<':
            return -1;
        //Right : 1
        case '>':
            return 1;
        //Stay : 0
        case '_':
            return 0;
        //Halt : -1 > x > 1
        case '-':
            return -2;
        //Default : 
        default:
            throw "Invalid Tape Movement";
            break;
    }

    return 2;
}