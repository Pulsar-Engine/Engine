#ifndef COMPONENTARRAYBASE_HPP_
#define COMPONENTARRAYBASE_HPP_

#include "Types.hpp"

class ComponentArrayBase {
public:
    virtual ~ComponentArrayBase() = default;
    virtual void entityDestroyed(Entity entity) = 0;
};

#endif
