#ifndef DomainBehaviour_H_
#define DomainBehaviour_H_

#include "engine/BasicBehaviour.h"

#include <string>

namespace alica
{
    class DomainBehaviour : public BasicBehaviour
    {
    public:
        DomainBehaviour(std::string name);
        virtual ~DomainBehaviour();

    };
} /* namespace alica */

#endif /* DomainBehaviour_H_ */
