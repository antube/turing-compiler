#pragma once

#include <fstream>

struct Instruction
{
    /* What to write to tape
     * -1 : Leave as is
     *  0 : Write a Zero
     *  1 : Write a One
    */
    signed char Output;

    /* Which direction to move read/write head
     * -1 : Move Left
     *  0 : Don't Move
     *  1 : Move Right
     *  2 : Halt
    */
    signed char Movement;

    //Next card to execute
    unsigned long long NextState;


    void read(std::ifstream &infile)
    {
        infile.read((char*)&Output, sizeof(signed char));
        infile.read((char*)&Movement, sizeof(signed char));
        infile.read((char*)&NextState, sizeof(unsigned long long));
    }


    void write(std::ofstream &outfile)
    {
        outfile.write((char*)&Output, sizeof(signed char));
        outfile.write((char*)&Movement, sizeof(signed char));
        outfile.write((char*)&NextState, sizeof(unsigned long long));
    }
};