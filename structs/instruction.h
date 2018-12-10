struct instruction
{
    /* Input from the tape
     * 0 : Read a Zero
     * 1 : Read a One
    */
    int Input;

    /* What to write to tape
     * -1 : Leave as is
     *  0 : Write a Zero
     *  1 : Write a One
    */
    int Output;

    //Next card to execute
    int NextState;

    /* Which direction to move read/write head
     * -1 : Move Left
     *  1 : Move Right
     *  0 : Stay Put
     * -1 > x > 1 : Halt
    */
    int Movement;
};