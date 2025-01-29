#include "../components.hpp"

#ifndef POSITIONCOMPONENT_HPP_
#define POSITIONCOMPONENT_HPP_

class PositionComponent : public AComponent {

public:
    PositionComponent() = default;
    ~PositionComponent() = default;

    std::string getName(void) const { return "position"; }
    ComponentType getType() const { return POSITION; }

    void setPosition(int x, int y) { _x = x; _y = y; }
    std::pair<int, int> getPosition() { return (std::make_pair(_x, _y)); }

    std::string toString() const {
        return "PositionComponent"; }
    int _x;
    int _y;
};

#endif