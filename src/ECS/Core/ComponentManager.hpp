#ifndef COMPONENTMANAGER_HPP_
#define COMPONENTMANAGER_HPP_

#include <unordered_map>
#include <memory>
#include <typeindex>
#include <stdexcept>
#include <array>
#include <cassert>
#include "Types.hpp"

class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void entityDestroyed(Entity entity) = 0;
};

template<typename T>
class ComponentArray : public IComponentArray {
public:
    void insertData(Entity entity, T&& component) {
        assert(_entityToIndex.find(entity) == _entityToIndex.end() && "Component added twice.");
        size_t newIndex = _size;
        _entityToIndex[entity] = newIndex;
        _indexToEntity[newIndex] = entity;
        _components[newIndex] = std::forward<T>(component);
        _size++;
    }

    void removeData(Entity entity) {
        assert(_entityToIndex.find(entity) != _entityToIndex.end() && "Removing non-existent component.");
        size_t indexOfRemoved = _entityToIndex[entity];
        size_t indexOfLast = _size - 1;
        _components[indexOfRemoved] = std::move(_components[indexOfLast]);
        Entity entityOfLast = _indexToEntity[indexOfLast];
        _entityToIndex[entityOfLast] = indexOfRemoved;
        _indexToEntity[indexOfRemoved] = entityOfLast;
        _entityToIndex.erase(entity);
        _indexToEntity.erase(indexOfLast);
        _size--;
    }

    T& getData(Entity entity) {
        assert(_entityToIndex.find(entity) != _entityToIndex.end() && "Component not found.");
        return _components[_entityToIndex[entity]];
    }

    bool hasData(Entity entity) {
        return _entityToIndex.find(entity) != _entityToIndex.end();
    }

    void entityDestroyed(Entity entity) override {
        if (_entityToIndex.find(entity) != _entityToIndex.end()) {
            removeData(entity);
        }
    }

private:
    std::array<T, MAX_ENTITIES> _components;
    std::unordered_map<Entity, size_t> _entityToIndex;
    std::unordered_map<size_t, Entity> _indexToEntity;
    size_t _size = 0;
};

class ComponentManager {
public:
    template<typename T>
    void registerComponent() {
        const char* typeName = typeid(T).name();
        assert(_componentTypes.find(typeName) == _componentTypes.end() && "Component type registered twice.");
        _componentTypes[typeName] = _nextComponentType;
        _componentArrays[typeName] = std::make_shared<ComponentArray<T>>();
        _nextComponentType++;
    }

    template<typename T>
    ComponentType getComponentType() {
        const char* typeName = typeid(T).name();
        assert(_componentTypes.find(typeName) != _componentTypes.end() && "Component not registered.");
        return _componentTypes[typeName];
    }

    template<typename T>
    void addComponent(Entity entity, T&& component) {
        getComponentArray<T>()->insertData(entity, std::forward<T>(component));
    }

    template<typename T>
    void removeComponent(Entity entity) {
        getComponentArray<T>()->removeData(entity);
    }

    template<typename T>
    T& getComponent(Entity entity) {
        return getComponentArray<T>()->getData(entity);
    }

    template<typename T>
    bool hasComponent(Entity entity) {
        auto componentArray = getComponentArray<T>();
        return componentArray->hasData(entity);
    }

    void entityDestroyed(Entity entity);

private:
    std::unordered_map<const char*, ComponentType> _componentTypes{};
    std::unordered_map<const char*, std::shared_ptr<IComponentArray>> _componentArrays{};
    ComponentType _nextComponentType{};

    template<typename T>
    std::shared_ptr<ComponentArray<T>> getComponentArray() {
        const char* typeName = typeid(T).name();
        return std::static_pointer_cast<ComponentArray<T>>(_componentArrays[typeName]);
    }
};

#endif
