#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#include <set>
#include "../Core/Types.hpp"

class Coordinator;

class System {
public:
    virtual ~System() = default; 
    std::set<Entity> entities;

    virtual void Update(float dt, Coordinator& coordinator) = 0;
};

#endif
