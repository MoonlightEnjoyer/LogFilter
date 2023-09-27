#ifndef SIMPLEREGEX_H
#define SIMPLEREGEX_H

class SimpleRegex
{
public:
    static bool Match(char* text, long startIndex, long endIndex, char* pattern, int patternLength);
};


#endif // SIMPLEREGEX_H
