#pragma once

#include <fstream>
#include "instruction.h"

struct Card
{
    Instruction Ins0; // with input 0
    Instruction Ins1; // with input 1


    void read(std::ifstream &infile)
    {
        Ins0.read(infile);
        Ins1.read(infile);
    }

    void write(std::ofstream &outfile)
    {
        Ins0.write(outfile);
        Ins1.write(outfile);
    }
};