#include "asp_commons/AnnotatedValVec.h"
#include "asp_commons/ASPQuery.h"
#include <Configuration.h>

namespace reasoner
{

AnnotatedValVec::AnnotatedValVec(long id, std::vector<Clingo::SymbolVector> values, std::shared_ptr<ASPQuery> query)
{
    this->id = id;
    this->values = values;
    this->query = query;
    extractResponse();
}

AnnotatedValVec::~AnnotatedValVec() {}

void AnnotatedValVec::extractResponse()
{
    for (int i = 0; i < values.size(); i++) {
        this->variableQueryValues.push_back(std::vector<std::string>());
        this->factQueryValues.push_back(std::vector<std::string>());
        for (auto val : values.at(i)) {
            std::stringstream ss;
            ss << val;
            std::string tmp = ss.str();
            tmp = supplementary::Configuration::trim(tmp);
            this->factQueryValues.at(i).push_back(tmp);
            tmp = tmp.substr(0, tmp.size() - 1);
            tmp = tmp.erase(0, tmp.find("(") + 1);
            this->variableQueryValues.at(i).push_back(tmp);
        }
    }
}

} /* namespace reasoner */
