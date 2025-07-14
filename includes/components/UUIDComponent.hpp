#include "../components.hpp"

#ifndef UUIDCOMPONENT_HPP_
#define UUIDCOMPONENT_HPP_

class UUIDComponent : public AComponent {

public:
    UUIDComponent() = default;
    ~UUIDComponent() = default;

    std::string getName(void) const { return "uuid"; }
    ComponentType getType() const { return UUID; }

    void setUUID(std::string newUuid) {
        uuid = newUuid;
    }

    std::string getUUID() {
        return uuid;
    }

    std::string toString() const { return "UUIDComponent"; }
private:
    std::string uuid;
};

#endif