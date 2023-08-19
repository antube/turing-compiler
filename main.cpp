#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <regex>

#include "structs/card.h"



int FirstTermParse(signed char&, const std::string&);
int SecondTermParse(signed char&, const std::string&);
int ThirdTermParse(unsigned long long&, const std::string&, const std::unordered_map<std::string, int>&);
int FourthTermParse(signed char&, const std::string&);

void Parse(std::vector<std::string>&, const std::string&);
int BuildCardMap(std::ifstream&, std::unordered_map<std::string, int>&);
int Sort(std::vector<Instruction>&);
int read(std::vector<std::string>&, std::ifstream&, int);



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


        if (std::regex_match(name, std::regex(".*\x2Etur$")))
            name.push_back('b');
        else
            name = name + ".turb";


        // create the output file stream
        std::ofstream outfile (name, std::ios::binary | std::ios::trunc);
        
        // test to see if the output file stream opened correctly
        if (!outfile)
        {
            // return an error and close the program if it didn't open correctly
            std::cout << "ERROR: Creation of object file " << name << " failed" << std::endl;
            return -1;
        }

        // create the card identifier map,
        // this map takes the card name and maps it to a card position with the file
        std::unordered_map<std::string, int> cardIdentifierMap;
        int count = 0; // This is the 


        // build the map of card poisitions
        int status = BuildCardMap(infile, cardIdentifierMap);

        if (status >= 0) // Check the status for an error code
        {
            std::cout << "ERROR: duplicate card name at card : " << status << std::endl;
            return -1;
        }



        // read the input file
        while(!infile.eof())
        {
            //Create string to hold instruction
            std::vector<std::string> parsed;
            read(parsed, infile, 2);

            signed char base = 2;


            // If the key word CARD is specified 
            if (parsed[0] == "CARD" && parsed.size() == 2)
            {
                // create a vector of instructions to build
                std::vector<Instruction> instructions;

                // iterate through all the instructions for this card
                for (int i = 0; i < base; i++)
                {
                    // instruction to build
                    Instruction ins;

                    // read a line and parse it
                    read(parsed, infile, 2);

                    // check that this is actually a instruction with the proper syntax
                    if (parsed[0] == "INS" && parsed.size() == 5)
                    {
                        // parse the input for the first term, not used for output but is nessecary for sorting later as it is
                        //     not garinteed the developer will put the instructions in a given order.
                        //     also check for error output from the parser.
                        if (FirstTermParse(ins.Input, parsed[1]) == -5)
                        {
                            // if there is an error return that an error has occured
                            std::cout << "Error bad first term for input in card " << count << std::endl;
                            return -1;
                        }

                        // parse second term, the output to the tape
                        //     also check for error output from the parser
                        if (SecondTermParse(ins.Output, parsed[2]) == -5)
                        {
                            // if there is an error return that an error has occured
                            std::cout << "Error bad second term at input 0 in card " << count << std::endl;
                            return -1;
                        }
                        
                        // parse the third term, the next card
                        //     also check for error output from the parser
                        if (ThirdTermParse(ins.NextState, parsed[3], cardIdentifierMap) == -2)
                        {
                            // if there is an error return an error has occured
                            std::cout << "Error bad third term at input 0 in card " << count << std::endl;
                            return -1;
                        }

                        // parse the fourth term, the movement of the tape
                        //     also check for error output from the parser
                        if (FourthTermParse(ins.Movement, parsed[4]) == -5)
                        {
                            // if there is an error return error has occured
                            std::cout << "Error bad fourth term at input 0 in card " << count << std::endl;
                            return -1;
                        }

                        instructions.push_back(ins);
                    }
                    // the instruction seems to be malformed
                    else
                    {
                        // return an error
                        std::cout << "ERROR bad instruction at card " << count << std::endl;
                        return -1;
                    }
                }

                // if the instructions vector is for some reason not the proper size there is a problem
                if (instructions.size() != base)
                {
                    // return that a problem has occured
                    std::cout << "ERROR bad instruction count at card " << count << std::endl;
                    return -1;
                }

                // sort the list of instructions in ascending order and check for an error.
                if (Sort(instructions) != 0)
                {
                    // if an error has occured I am not sure how it got here HELP ME
                    std::cout << "ERROR not sure how I got here please debug for developer" << std::endl;
                    return -1;
                }


                // iterate through the instructions vector
                for (Instruction ins : instructions)
                {
                    // writing out to the outfile as I go
                    ins.write(outfile);
                }

                // increment the counter for error handling
                count++;
            }
            // check for the base keyword
            else if (parsed[0] == "BASE" && parsed.size() == 2)
            {
                if (count != 0)
                {
                    std::cout << "ERROR the base keyword was not at the beginning" << std::endl;
                    return -1;
                }

                // parse the base identifier as the base is also sort of specified by the first term in an instruction I am reusing code
                if (FirstTermParse(base, parsed[1]) == -5)
                {
                    // error bad base
                    std::cout << "ERROR bad base" << std::endl;
                    return -1;
                }

                // write the base to file
                outfile.write((char*)&base, sizeof(signed char));
            }
            // else bad keyword
            else
            {
                // say that there was a bad keyword
                std::cout << "INVALID keyword: " << count << std::endl;
                return -1;
            }
        }


        // close the input and output files
        infile.close();
        outfile.close();
    }

    return 0;
}


int FirstTermParse(signed char &val, const std::string &term)
{
    // convert the string to a unsigned int value and assign into the val variable
    val = stoul(term);

    // if the val is not within the valid range of 0 and 127
    if (val < 0 || val > 127)
    {
        // throw an error
        val = -1;
        return -5;
    }

    // otherwise return success
    return 0;
}


int SecondTermParse(signed char &val, const std::string &term)
{
    // check if the term is the leave as is identifier
    if (term == "|")
    {
        // assign the corisponding value
        val = -1;
    }
    else
    {
        // try and convert the term in an unsigned int value, I want this to blow up if it is not an unsigned value
        val = stoul(term);

        // check if the value is out of range
        if (val < 0 || val > 127)
        {
            // if so return an error
            val = -1;
            return -5;
        }
    }

    // oterwise return success
    return 0;
}



int ThirdTermParse(unsigned long long &val, const std::string &term, const std::unordered_map<std::string, int> &cardIdentifiers)
{
    // check if this is the no card specified identifier
    if (term == "-")
    {
        // if so return no error and set the value to -1
        val = -1;
        return 0;
    }

    // check for the card in the card identifiers
    if (cardIdentifiers.find(term) != cardIdentifiers.end())
    {
        // get the position of the card and assign it to the val
        val = cardIdentifiers.at(term);
    }
    else
    {
        // otherwise no card was found by that identifier and therefore return error
        std::cout << "Bad card identifier: " << term << std::endl;
        return -2;
    }

    // return success
    return 0;
}



int FourthTermParse(signed char &val, const std::string &term)
{
    // check if the term is the move left idetifier
    if (term == "<")
        // assign the value -1 to val
        val = -1;
    // if the term is the do not move idetifier
    else if (term == "|")
        // assign the value 0 to val
        val = 0;
    // if the term is the move right identifier
    else if (term == ">")
        // assign the value 1 to val
        val = 1;
    // if the term is the halt identifier
    else if (term == "^")
        // assign 2 to val
        val = 2;
    else
        // otherwise return an error
        return -5;

    // if succesful return success
    return 0;
}



int BuildCardMap(std::ifstream &infile, std::unordered_map<std::string, int> &cardIdentifierMap)
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
            if (cardIdentifierMap.find(parsed[1]) == cardIdentifierMap.end())
            {
                cardIdentifierMap[parsed[1]] = count;
                count++;
            }
            else
            {
                return count;
            }
        }
    }

    infile.seekg(0, infile.beg);

    return -1;
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


int Sort(std::vector<Instruction> &inss)
{
    int count = 0;
    while (count != 0)
    {
        count = 0;

        for (int i = 0; i < inss.size() - 2; i++)
        {
            if (inss[i].Input > inss[i + 1].Input)
            {
                Instruction temp = inss[1];
                inss[i] = inss[i + 1];
                inss[i + 2] = temp;
                count++;
            }
        }
    }

    return 0;
}



int read(std::vector<std::string> &parsed, std::ifstream &infile, int min)
{
    //Create string to hold instruction
    std::string line;

    // clear the parsed vector
    parsed.clear();

    //Read from input file into char array
    std::getline(infile, line);

    Parse(parsed, line); // call parse on the line

    // if there are fewer than 2 
    if (parsed.size() < min)
        read(parsed, infile, min);

    return 0;
}