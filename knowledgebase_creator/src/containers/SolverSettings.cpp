#include "containers/SolverSettings.h"

#include <QString>

namespace kbcr
{

SolverSettings::SolverSettings(std::string name, std::string argString)
{
    this->name = name;
    this->argString = argString;
    extractSettingsVector();
}

SolverSettings::~SolverSettings()
{
    for (auto arg : this->args) {
        free((char*) arg);
    }
}

void SolverSettings::extractSettingsVector()
{
    // Separate setting by commata
    if (this->argString.find(",") != std::string::npos) {
        size_t start = 0;
        size_t end = std::string::npos;
        std::string parsedParam = "";
        while (start != std::string::npos) {
            end = this->argString.find(",", start);
            if (end == std::string::npos) {
                parsedParam = QString(this->argString.substr(start, this->argString.length() - start).c_str()).trimmed().toStdString();
                this->argumentStrings.push_back(parsedParam);
                break;
            }
            parsedParam = QString(this->argString.substr(start, end - start).c_str()).trimmed().toStdString();
            start = this->argString.find(",", end);
            if (start != std::string::npos) {
                start += 1;
            }
            this->argumentStrings.push_back(parsedParam);
        }
    } else {
        this->argumentStrings.push_back(this->argString);
    }
    for (int i = 0; i < this->argumentStrings.size(); i++) { // add params to startParams
        this->args.push_back(strdup(this->argumentStrings[i].c_str()));
    }
    this->args.push_back(nullptr);
}

} /* namespace kbcr */
