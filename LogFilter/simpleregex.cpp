#include "simpleregex.h"

bool SimpleRegex::Match(char *text, long startIndex, long endIndex, char *pattern, int patternLength)
{
    bool charsNotEqual = true;
    long j;
    for (long i = startIndex; charsNotEqual && i <= endIndex; i++)
    {
        charsNotEqual = text[i] != pattern[0];

        for (j = 0; !charsNotEqual && j < patternLength && i <= endIndex; j++, i++)
        {
            charsNotEqual = text[i] != pattern[j];
        }
    }

    return !charsNotEqual;
}
