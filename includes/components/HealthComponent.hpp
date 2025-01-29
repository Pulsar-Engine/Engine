#include "../components.hpp"

#ifndef HEALTHCOMPONENT_HPP_
#define HEALTHCOMPONENT_HPP_

class HealthComponent : public AComponent {

public:
    HealthComponent(int hp = 100): _hp(hp) {  }
    HealthComponent(HealthComponent &h): _hp(h._hp) {  }
    ~HealthComponent() = default;

    std::string getName(void) const { return "health"; }
    ComponentType getType() const { return HEALTH; }
    
    void setHp(int hp) { _hp = hp; }
    int getHp(void) { return _hp; }
    
    void takeDamage(int damage = 0) { _hp -= damage; }

    std::string toString() const { return "HealthComponent"; }

private:
    int _hp;
};

#endif