#ifndef SIMPLEREGEX_H
#define SIMPLEREGEX_H
#pragma once
#include <iostream>

class SimpleRegex
{
public:
    static bool Match(char* text, std::int64_t startIndex, std::int64_t endIndex, char* pattern, std::int64_t patternLength);
};


#endif // SIMPLEREGEX_H
