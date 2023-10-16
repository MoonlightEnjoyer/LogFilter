#include "simpleregex.h"
#include <iostream>

bool SimpleRegex::Match(char *text, std::int64_t startIndex, std::int64_t endIndex, char *pattern, std::int64_t patternLength)
{
    bool charsNotEqual = true;
    std::int64_t j;
    for (std::int64_t i = startIndex; charsNotEqual && i <= endIndex; i++)
    {
        charsNotEqual = text[i] != pattern[0];

        for (j = 0; !charsNotEqual && j < patternLength && i <= endIndex; j++, i++)
        {
            charsNotEqual = text[i] != pattern[j];
        }
    }

    return !charsNotEqual;
}
