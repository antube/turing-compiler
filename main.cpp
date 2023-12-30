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
int Read(std::ifstream&, std::vector<std::string>&, int = 0);



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
        signed char base = 2;
        bool baseExist = false;


        // build the map of card poisitions
        int status = BuildCardMap(infile, cardIdentifierMap);

        if (status != -1) // Check the status for an error code
        {
            std::cout << "ERROR: duplicate card name at card : " << status << std::endl;
            return -1;
        }



        // read the input file
        while(!infile.eof())
        {
            //Create string to hold instruction
            std::vector<std::string> parsed;
            Read(infile, parsed, 2);

            


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
                    Read(infile, parsed, 2);

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

                //set flag that a base was specified
                baseExist = true;

                // I wait till the end of the file as it makes the equations similar on the machines end
            }
            // else bad keyword
            else
            {
                // say that there was a bad keyword
                std::cout << "INVALID keyword: " << count << std::endl;
                return -1;
            }
        }


        // write the base to the outfile
        outfile.seekp(0, outfile.end);
        outfile.write((char*)&base, sizeof(signed char));


        // close the input and output files
        infile.close();
        outfile.close();
    }

    return 0;
}


/*******************************************
 * Name : 
 * 
 * Description :
 *     
 * 
 * Arguments :
 *     
 * 
 * Return : 
 *      
*********************************************/
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


/*******************************************
 * Name : SecondTermParse
 * 
 * Description :
 *     
 * 
 * Arguments :
 *     
 * 
 * Return : 
 *      
*********************************************/
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



/*******************************************
 * Name : 
 * 
 * Description :
 *     
 * 
 * Arguments :
 *     
 * 
 * Return : 
 *      
*********************************************/
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



/*******************************************
 * Name : FourthTermParse
 * 
 * Description :
 *     
 * 
 * Arguments :
 *     
 * 
 * Return : 
 *      
*********************************************/
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




/*******************************************
 * Name : BuildCardMap
 * 
 * Description :
 *     Runs through the file ahead of time and builds a map of the cards
 *        and there position inrelation to each other returning to the caller
 *        an unordered map of names as keys and position of the card as value
 * 
 *        The card map will return the read position to the beginning of the file
 * 
 * Arguments :
 *                infile : &std::ifstream                        : A caller supplied ifstream to read from
 *     cardIdentifierMap : &std::unordered_map<std::string, int> : The caller supplied unordered map to write to
 * 
 * Return : 
 *      int : the exit status code : -1 if succesful ; otherwise it will be the card position which is problamatic or another error code
*********************************************/
int BuildCardMap(std::ifstream &infile, std::unordered_map<std::string, int> &cardIdentifierMap)
{
    // store the current position in the stream
    int initialStreamPos = infile.tellg();

    // check if there was problem with the stream
    if (initialStreamPos == -1)
        return -2;


    // seek to the beggining of the file to ensure proper positioning
    infile.seekg(0, infile.beg);

    // create count and set to zero
    int count = 0;

    // iterate through the file
    while (!infile.eof())
    {
        // create a parsed list
        std::vector<std::string> parsed;

        // read line from file and store it in parsed
        Read(infile, parsed, 2);

        // check if this is a card line
        if (parsed[0] == "CARD")
        {
            // if so check for a similarly named card in the map
            if (cardIdentifierMap.find(parsed[1]) == cardIdentifierMap.end())
            {
                // if there is no similarly named card add the card and increase the card count
                cardIdentifierMap[parsed[1]] = count;
                count++;
            }
            else
            {
                // otherwise there is a duplicate
                // return the card count as an error
                return count;
            }
        }
    }

    // return to the beggining of the file
    infile.seekg(initialStreamPos, infile.beg);

    // return success
    // this function has a a weird success return as it is possible for the failure to be 0
    //     as in the first card
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
 *     parsed : &std::vector<std::string> : 
 *       line : &std::string              : holds a string of text to be parsed
 * 
 * Return : 
 *      void
*********************************************/
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

    // if the term length is not equal to zero
    if (term.length() != 0)
    {
        // then the term can be pushed to the back of the list
        //    as the term is cleared out at the beggining of a run (above)
        //    a non empty term means a term was completed but not added
        parsed.push_back(term);
        term.clear();
    }
}



/*******************************************
 * Name : Sort
 * 
 * Description :
 *     Sorts a vector of instructions in ascending order by the input member
 * 
 * Arguments :
 *     ins : &std::vector<Instruction> : instructions to sort
 * 
 * Return : 
 *      int : exit status
 *             0 = success
*********************************************/
int Sort(std::vector<Instruction> &ins)
{
    int count = 0; // create and intialize the cout variable, to used to count how many items have been moved in an iteration
    
    // check if the count is not equal to zero
    while (count != 0)
    {
        count = 0; // reset the count to zero to ensure I actually know if sorting has been happening

        // iterate through the list 
        for (int i = 0; i < ins.size() - 2; i++)
        {
            // if the current instruction input is greater than the next instruction
            if (ins[i].Input > ins[i + 1].Input)
            {
                // switch the current and next instruction
                Instruction temp = ins[1];
                ins[i] = ins[i + 1];
                ins[i + 2] = temp;
                count++;
            }
        }
    }

    // return success
    return 0;
}



/*******************************************
 * Name : Read
 * 
 * Description :
 *     Reads a line from a text file and parses the text. Once parsed the read will check how many
 *        items are were return from the parser and compare to the min. If it is less than the specified min
 *        it will continue until either the min is achieved or an eof is achieved.
 * 
 * Arguments :
 *     infile : &std::ifstream            : The text file to read from
 *     parsed : &std::vector<std::string> : A vector of strings containing the parsed text from the line read in
 *        min :  int                      : The minnimium number of parsed items 
 * 
 * Return : 
 *      int : The exit status
 *             0 = success
 *            -1 = end of file achieved
*********************************************/
int Read(std::ifstream &infile, std::vector<std::string> &parsed, int min = 0)
{
    //Create string to hold instruction
    std::string line;

    // clear the parsed vector
    parsed.clear();

    // Check if the file is at the end
    if (infile.eof())
        return -1;

    //Read from input file into char array
    std::getline(infile, line);

    Parse(parsed, line); // call parse on the line

    // if there are fewer than the min in parsed read the next line
    if (parsed.size() < min)
        return Read(infile, parsed, min);

    // return success
    return 0;
}