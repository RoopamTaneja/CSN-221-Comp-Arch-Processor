/* Assembler code fragment for RiSC */
#include <stdio.h>
#include <string.h>
#define MAXLINELENGTH 1000
char *readAndParse(FILE *inFilePtr, char *lineString,
                   char **labelPtr, char **opcodePtr, char **arg0Ptr,
                   char **arg1Ptr, char **arg2Ptr)
{
    /* read and parse a line
    note that lineString must point to allocated memory, so that *labelPtr,
    *opcodePtr, and *argXPtr won’t be pointing to readAndParse’s memory
    note also that *labelPtr, *opcodePtr, and *argXPtr
    point to memory locations in lineString.
    When lineString changes, so will *labelPtr, *opcodePtr, and *argXPtr.
    function returns NULL if at end-of-file */
    char *statusString, *firsttoken;
    statusString = fgets(lineString, MAXLINELENGTH, inFilePtr);
    if (statusString != NULL)
    {
        firsttoken = strtok(lineString, " \t\n");
        if (firsttoken == NULL || firsttoken[0] == '#')
        {
            return readAndParse(inFilePtr, lineString, labelPtr,
                                opcodePtr, arg0Ptr, arg1Ptr, arg2Ptr);
        }
        else if (firsttoken[strlen(firsttoken) - 1] == ':')
        {
            *labelPtr = firsttoken;
            *opcodePtr = strtok(NULL, " \n");
            firsttoken[strlen(firsttoken) - 1] = '\0';
        }
        else
        {
            *labelPtr = NULL;
            *opcodePtr = firsttoken;
        }
        *arg0Ptr = strtok(NULL, ", \t\n");
        *arg1Ptr = strtok(NULL, ", \t\n");
        *arg2Ptr = strtok(NULL, ", \t\n");
    }
    return (statusString);
}
int isNumber(char *string)
{
    /* return 1 if string is a number */
    int i;
    return ((sscanf(string, "%d", &i)) == 1);
}
main(int argc, char *argv[])
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char *label, *opcode, *arg0, *arg1, *arg2;
    char lineString[MAXLINELENGTH + 1];
    if (argc != 3)
    {
        printf("error:usage: %s<assembly - code - file> <machine - code - file>\n ", argv[0]);
        exit(1);
    }
    inFileString = argv[1];
    outFileString = argv[2];
    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL)
    {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL)
    {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }
    /* here is an example for how to use readAndParse to read a line from inFilePtr */
    if (readAndParse(inFilePtr, lineString, &label, &opcode,
                     &arg0, &arg1, &arg2) == NULL)
    {
        /* reached end of file */
    }
    else
    {
        /* label is either NULL or it points to
        a null-terminated string in lineString
        that has the label. If label is NULL,
        that means the label field didn’t exist.
        Same for opcode and argX. */
    }

    /* this is how to rewind file ptr to start reading from beginning of file */
    rewind(inFilePtr);

    /* after a readAndParse,you may want to do the following to test each opcode */
    if (!strcmp(opcode, "add"))
    {
        /* do whatever you need to do for opcode “add” */
    }
}
