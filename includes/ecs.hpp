#include <unordered_map>
#include <memory>
#include "const.hpp"
#include <string>

#ifndef ENTITY_HPP_
#define ENTITY_HPP_

class Entity {
    public:
        Entity(int id) { _id = id; };
        ~Entity() = default;

        std::size_t getId()
        {
            return (this->_id);
        }

        void addComponent(ComponentType type)
        {
            auto it = component_list.find(type);
            if (it != component_list.end()) {
                _components[type] = it->second();
            }
        }

        void showComponent()
        {
            for (auto it = _components.begin(); it != _components.end(); it++) {
                std::cout << it->second.get()->getName() << std::endl;
            }
        }

        void removeComponent(ComponentType type)
        {
            auto it = _components.find(type);
            if (it != _components.end()) {
                _components.erase(it);
            }
        }

        bool hasComponent(ComponentType type)
        {
            auto it = _components.find(type);
            if (it != _components.end()) {
                return true;
            }
            return false;
        }

        std::shared_ptr<IComponent> getComponent(ComponentType type)
        {
            auto it = _components.find(type);
            if (it != _components.end()) {
                return it->second;
            }
            return nullptr;
        }

        operator std::size_t() const
        {
            return _id;
        }

        std::string toString() const
        {
            std::string str = std::to_string(_id);
            for (auto it = _components.begin(); it != _components.end(); it++) {
                str = str + " " + it->second.get()->toString();
            }
            return str;
        }

    private:
        std::size_t _id;
        std::unordered_map<ComponentType, std::shared_ptr<IComponent>> _components;
        ComponentType type;
};

class EntityFactory {
    public:
        EntityFactory() = default;
        ~EntityFactory() = default;

        std::shared_ptr<Entity> createEntity()
        {
            std::shared_ptr<Entity> entity = std::make_shared<Entity>(this->_id);
            this->_id++;
            return entity;
        }

    private:
        std::size_t _id = 0;
};

#endif