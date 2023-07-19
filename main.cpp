#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <memory>
#include <map>

#include "structs/card.h"



#define INSTRUCTION_LENGTH 16



int SecondTermParse(std::string&);
int ThirdTermParse(std::string&, std::map<std::string, int>&);
int FourthTermParse(std::string&);

void Parse(std::vector<std::string>&, std::string&);
void BuildCardMap(std::ifstream&, std::map<std::string, int>&);



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
        std::map<std::string, int> cardIdentifierMap;
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



int ThirdTermParse(std::string &term, std::map<std::string, int> &cardIdentifiers)
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



/****************************************
 * Name : Parser
 * 
 * Description : 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
*/
void Parser(std::vector<std::string> &parsed, std::string &line, std::vector<char> &delinators)
{
    //The current string
    std::string term;

    // iterate through the line one character at a time
    for (char &c : line)
    {
        // true if the current character is not among the delinators
        bool notDelinator = true;

        // iterate through the delinators
        for (char d : delinators)
        {
            // check if the current character is a delinator
            if (c == d)
            {
                // if the current character is a delinator set the not delinator to false 
                notDelinator = false;

                // if the term is not of length 0 push it to the back of the parsed vector and clear term
                if (term.length() != 0)
                {
                    parsed.push_back(term);
                    term.clear();
                }

                // not nessecary to continue checking
                break;
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
    char del[] = {' ', ':', '\t'};
    // create a shared pointer to a vector of chars to represent the 
    std::vector<char> delinators(del, del + sizeof(del) / sizeof(char));

    // call the Parser
    Parser(parsed, line, delinators);

    // iterate through the vector
    for (long unsigned int i = 0; i < parsed.size(); i++)
    {
        // search for the comment delinator and erase everything after it
        if (parsed.at(i).find_first_of('#') != std::string::npos)
        {
            // remove all parsed sections after the first instance of the comment delinator
            parsed.erase(parsed.begin() + i + 1, parsed.end());
            // remove comment delinator and everything after it from the string which it is found in
            parsed.at(i) = parsed.at(i).substr(0, parsed.at(i).find_first_of('#'));

            // if the remove operation cleared out the string completly remove it from the vector
            if (parsed.at(i).length() == 0)
            {
                // remove the string from the vector
                parsed.erase(parsed.begin() + i);
            }
        }
    }
}



void BuildCardMap(std::ifstream &infile, std::map<std::string, int> &cardIdentifierMap)
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