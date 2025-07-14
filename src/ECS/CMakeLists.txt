set(ECS_SRC
    ${CMAKE_CURRENT_LIST_DIR}/Core/ComponentArrayBase.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Core/ComponentManager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Core/Coordinator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Core/EntityManager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Systems/SystemManager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Scene/SceneManager.cpp
)

target_sources(pulsar-engine-static PRIVATE ${ECS_SRC})

target_include_directories(pulsar-engine-static PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/Core
    ${CMAKE_CURRENT_LIST_DIR}/Systems
    ${CMAKE_CURRENT_LIST_DIR}/Scene
    ${CMAKE_CURRENT_LIST_DIR}/Components
)

target_include_directories(pulsar-engine PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/Core
    ${CMAKE_CURRENT_LIST_DIR}/Systems
    ${CMAKE_CURRENT_LIST_DIR}/Scene
    ${CMAKE_CURRENT_LIST_DIR}/Components
)
