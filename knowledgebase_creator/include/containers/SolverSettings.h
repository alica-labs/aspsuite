#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <stdlib.h>
#include <string.h>

namespace kbcr
{
/**
 * Class to encapsulate settings for the Solver
 */
class SolverSettings
{
public:
    /**
     * Constructor
     * @param argString string containing the params
     */
    SolverSettings(std::string name, std::string argString);
    virtual ~SolverSettings();

    /**
     * Parsed params for solver const char*
     */
    std::vector<char const*> args;
    /**
     * Parsed params for solver vector<string>
     */
    std::vector<std::string> argumentStrings;
    /**
     * Not parsed param string
     */
    std::string argString;
    /**
     * name of the setting
     */
    std::string name;

private:
    /**
     * Extracts both settings vectors from argString
     */
    void extractSettingsVector();
};

} /* namespace kbcr */
