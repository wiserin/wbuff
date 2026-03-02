target_sources(WBuffer PRIVATE
               ${CMAKE_CURRENT_SOURCE_DIR}/constructors.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/insert.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/operators.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/simple_op.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp)

add_subdirectory(iterator)
