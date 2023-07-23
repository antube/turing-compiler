#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <memory>
#include <unordered_map>

#include "structs/card.h"



#define INSTRUCTION_LENGTH 16



int SecondTermParse(std::string&);
int ThirdTermParse(std::string&, std::unordered_map<std::string, int>&);
int FourthTermParse(std::string&);

void Parse(std::vector<std::string>&, std::string&);
void BuildCardMap(std::ifstream&, std::unordered_map<std::string, int>&);



int main(int argc, char *argv[])
{
    // If there are not 2 arguments (the main command and the file to interpret) print an error
    if(argc != 2)
    {
        std::cout << "Error invalid number of arguments" << std::endl;
    }
    else
    {
        // create the file input stream
        std::ifstream infile(argv[1]);
        
        // test whether the input file stream opened correctly
        if (!infile)
        {
            // return an error and close the program if it didn't open correctly
            std::cout << "ERROR: Source File open failed" << std::endl;
            return -1;
        }


        // create the output file name (just the input files name with an o appended)
        std::string name(argv[1]);
        name.push_back('o');

        // create the output file stream
        std::ofstream outfile (name, std::ios::binary | std::ios::trunc);
        
        // test to see if the output file stream opened correctly
        if (!outfile)
        {
            // return an error and close the program if it didn't open correctly
            std::cout << "ERROR: Creation of object file " << name << " failed" << std::endl;
            return -1;
        }

        Card car;
        std::unordered_map<std::string, int> cardIdentifierMap;
        int count = 0;

        BuildCardMap(infile, cardIdentifierMap);

        // read the input file
        while(!infile.eof())
        {
            //Create string to hold instruction
            std::string line;
            std::vector<std::string> parsed;

            //Read from input file into char array
            std::getline(infile, line);

            Parse(parsed, line);

            if (parsed.size() < 2)
                continue;
            

            if (parsed[0] == "CARD" && parsed.size() == 2)
            {
                if (count != 0)
                    car.write(outfile);

                count++;
            }
            else if (parsed.at(0) == "INS" && parsed.size() == 5)
            {
                if (parsed.at(1) == "0")
                {

                    car.Ins0.Output = SecondTermParse(parsed.at(2));
                    if (car.Ins0.Output == -5)
                    {
                        std::cout << "Error bad second term at input 0 in card " << count << std::endl;
                        return -1;
                    }
                    
                    car.Ins0.NextState = ThirdTermParse(parsed.at(3), cardIdentifierMap);
                    if (car.Ins0.NextState == -2LL)
                    {
                        std::cout << "Error bad third term at input 0 in card " << count << std::endl;
                        return -1;
                    }

                    car.Ins0.Movement = FourthTermParse(parsed.at(4));
                    if (car.Ins0.Movement == -5)
                    {
                        std::cout << "Error bad fourth term at input 0 in card " << count << std::endl;
                        return -1;
                    }
                }
                else if (parsed.at(1) == "1")
                {
                    car.Ins1.Output = SecondTermParse(parsed.at(2));
                    if (car.Ins1.Output == -5)
                    {
                        std::cout << "Error bad second term at input 1 in card " << count << std::endl;
                        return -1;
                    }

                    car.Ins1.NextState = ThirdTermParse(parsed.at(3), cardIdentifierMap);
                    if (car.Ins1.NextState == -2LL)
                    {
                        std::cout << "Error bad third term at input 1 in card " << count << std::endl;
                        return -1;
                    }

                    car.Ins1.Movement = FourthTermParse(parsed.at(4));
                    if (car.Ins1.Movement == -5)
                    {
                        std::cout << "Error bad fourth term at input 0 in card " << count << std::endl;
                        return -1;
                    }
                }
                else
                {
                    std::cout << "ERROR bad input designator in card " << count << std::endl;
                    return -1;
                }
            }
            else
            {
                std::cout << "INVALID keyword card: " << count << std::endl;
                return -1;
            }
        }

        car.write(outfile);

        infile.close();
        outfile.close();
    }

    return 0;
}



int SecondTermParse(std::string &term)
{
    if (term == "0")
        return 0;
    else if (term == "1")
        return 1;
    else if (term == "|")
        return -1;

    return -5;
}



int ThirdTermParse(std::string &term, std::unordered_map<std::string, int> &cardIdentifiers)
{
    if (term == "-")
    {
        return -1;
    }
    
    int id = -1;

    try
    {
        id = cardIdentifiers.at(term);
    }
    catch(...)
    {
        std::cout << "Bad card identifier: " << term << std::endl;
        return -2;
    }
    
    return id;
}



int FourthTermParse(std::string &term)
{
    if (term == "<")
        return -1;
    else if (term == "|")
        return 0;
    else if (term == ">")
        return 1;
    else if (term == "^")
        return 2;

    return -5;
}



void BuildCardMap(std::ifstream &infile, std::unordered_map<std::string, int> &cardIdentifierMap)
{
    infile.seekg(0, infile.beg);

    int count = 0;

    while (!infile.eof())
    {
        std::string line;
        std::getline(infile, line);

        std::vector<std::string> parsed;

        Parse(parsed, line);

        if (parsed.size() < 2)
            continue;

        if (parsed[0] == "CARD")
        {
            cardIdentifierMap[parsed[1]] = count;
            count++;
        }
    }

    infile.seekg(0, infile.beg);
}


/*******************************************
 * Name : Parse
 * 
 * Description :
 *     Calls the Parser and removes any unnessecary elements such as colons.
 *   Colons are used for visually breaking up the program for easier reading
 *   but are unnessecary for the actual parsing
 * 
 * Arguments :
 *     line : std::string : holds a string of text to be parsed
 * 
 * Return : 
 *      std::shared_ptr<std::vector<string>> : A shared pointer to a vector of
 *    strings which hold the results of the parsing
*/
void Parse(std::vector<std::string> &parsed, std::string &line)
{
    // create a map of chars to represent the delinators
    std::unordered_map<char, int> delinators({{' ', 1},{':', 1},{'\t', 1},{'#', 2}});

    //The current string
    std::string term;

    // iterate through the line one character at a time
    for (char &c : line)
    {
        // true if the current character is not among the delinators
        bool notDelinator = true;

        // check if the current character is a delinator
        if (delinators.find(c) != delinators.end())
        {
            // if the current character is a delinator set the not delinator to false 
            notDelinator = false;

            // if the delinator is a class two delinator ignore everything after it
            if (delinators[c] == 2)
                break;

            // if the term is not of length 0 push it to the back of the parsed vector and clear term
            if (term.length() != 0)
            {
                parsed.push_back(term);
                term.clear();
            }
        }

        // If the current character is not a delinator add it to the string
        if (notDelinator)
            term.push_back(c);
    }

    if (term.length() != 0)
    {
        parsed.push_back(term);
        term.clear();
    }
}
