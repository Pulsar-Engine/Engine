#include "../components.hpp"

#ifndef VELOCITYCOMPONENT_HPP_
#define VELOCITYCOMPONENT_HPP_

class VelocityComponent : public AComponent {

public:
    VelocityComponent() = default;
    ~VelocityComponent() = default;

    std::string getName(void) const { return "velocity"; }
    ComponentType getType() const { return VELOCITY; }

    void setVelocity(int dx, int dy) { _dx = dx; _dy = dy; }
    std::pair<int, int> getVelocity() { return std::make_pair(_dx, _dy); }
    
    std::string toString() const { return "VelocityComponent"; }

private:
    int _dx; // Change in X
    int _dy; // Change in Y
};

#endif