#include <iostream>

#ifndef ICOMPONENT_HPP_
#define ICOMPONENT_HPP_

enum ComponentType {
    HEALTH,
    POSITION,
    UUID,
    VELOCITY,
    UNKNOWN,
};

class IComponent {
    public:
        virtual ~IComponent() = default;
        virtual std::string getName() const = 0;
        virtual ComponentType getType() const = 0;
        virtual std::string toString() const = 0;
};

class AComponent : public IComponent {
    public:
        AComponent() = default;
        virtual ~AComponent() = default;
        virtual std::string getName() const = 0;
        virtual ComponentType getType() const = 0;
        virtual std::string toString() const = 0;
};

#endif
