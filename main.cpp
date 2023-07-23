#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

#include "structs/card.h"



int SecondTermParse(signed char&, const std::string&);
int ThirdTermParse(unsigned long long&, const std::string&, const std::unordered_map<std::string, int>&);
int FourthTermParse(signed char&, const std::string&);

void Parse(std::vector<std::string>&, const std::string&);
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
            else if (parsed[0] == "INS" && parsed.size() == 5)
            {
                if (parsed.at(1) == "0")
                {
                    if (SecondTermParse(car.Ins0.Output, parsed[2]) == -5)
                    {
                        std::cout << "Error bad second term at input 0 in card " << count << std::endl;
                        return -1;
                    }
                    
                    if (ThirdTermParse(car.Ins0.NextState, parsed[3], cardIdentifierMap) == -2)
                    {
                        std::cout << "Error bad third term at input 0 in card " << count << std::endl;
                        return -1;
                    }

                    if (FourthTermParse(car.Ins0.Movement, parsed[4]) == -5)
                    {
                        std::cout << "Error bad fourth term at input 0 in card " << count << std::endl;
                        return -1;
                    }
                }
                else if (parsed.at(1) == "1")
                {
                    if (SecondTermParse(car.Ins1.Output, parsed[2]) == -5)
                    {
                        std::cout << "Error bad second term at input 1 in card " << count << std::endl;
                        return -1;
                    }

                    if (ThirdTermParse(car.Ins1.NextState, parsed.at(3), cardIdentifierMap) == -2)
                    {
                        std::cout << "Error bad third term at input 1 in card " << count << std::endl;
                        return -1;
                    }

                    if (FourthTermParse(car.Ins1.Movement, parsed.at(4)) == -5)
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



int SecondTermParse(signed char & val, const std::string &term)
{
    if (term == "0")
        val = 0;
    else if (term == "1")
        val = 1;
    else if (term == "|")
        val = -1;
    else
        return -5;

    return 0;
}



int ThirdTermParse(unsigned long long &val, const std::string &term, const std::unordered_map<std::string, int> &cardIdentifiers)
{
    if (term == "-")
    {
        val = -1;
        return 0;
    }

    if (cardIdentifiers.find(term) != cardIdentifiers.end())
    {
        val = cardIdentifiers.at(term);
    }
    else
    {
        std::cout << "Bad card identifier: " << term << std::endl;
        return -2;
    }

    return 0;
}



int FourthTermParse(signed char &val, const std::string &term)
{
    if (term == "<")
        val = -1;
    else if (term == "|")
        val = 0;
    else if (term == ">")
        val = 1;
    else if (term == "^")
        val = 2;
    else
        return -5;

    return 0;
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
void Parse(std::vector<std::string> &parsed, const std::string &line)
{
    // create a map of chars to represent the delinators
    std::unordered_map<char, int> delinators({{' ', 1},{':', 1},{'\t', 1},{'#', 2}});

    //The current string
    std::string term;

    // iterate through the line one character at a time
    for (const char &c : line)
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
