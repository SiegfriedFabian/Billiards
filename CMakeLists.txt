cmake_minimum_required(VERSION 3.16)
project(SymplecticBilliards)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(glad src/glad.c)
include_directories(include)

# imgui library, details can be found here https://github.com/ocornut/imgui/
add_library(imgui include/imgui/imgui.cpp include/imgui/imgui_draw.cpp include/imgui/imgui_tables.cpp
        include/imgui/imgui_widgets.cpp include/imgui/imgui_impl_glfw.cpp include/imgui/imgui_impl_opengl3.cpp include/imgui/imgui_demo.cpp)

if(APPLE)
    add_library(glfw SHARED IMPORTED)
    set_target_properties(glfw PROPERTIES IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/lib-universal/libglfw.3.dylib")
endif()

if(WIN32)
    add_library(glfw STATIC IMPORTED)
    set_property(TARGET glfw PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/lib-windows/glfw3.lib")
endif()

# Link libraries that all targets share to all targets
link_libraries(glfw ${GL_LIBRARY} glad imgui)

add_executable(SymplecticBilliards src/symplectic.cpp src/shaderClass.cpp src/shape.cpp src/SymplecticBilliard.cpp)
add_executable(4th src/4th.cpp src/shaderClass.cpp src/shape.cpp src/Trajectory.cpp src/FourthBilliard.cpp)

# Define shaders directory next to the executable '4th'
set(SHADERS_DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")

# Find shader files
file(GLOB SHADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.vert"
                       "${CMAKE_CURRENT_SOURCE_DIR}/src/*.frag")

# Create a custom target that always runs
add_custom_target(CopyShaders ALL
                  COMMAND ${CMAKE_COMMAND} -E echo "Copying shaders..."
                  BYPRODUCTS ${SHADERS_DEST_DIR})

# Ensure the shaders directory exists
add_custom_command(TARGET CopyShaders PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_DEST_DIR})

# Copy shaders if they are updated
foreach(SHADER_FILE ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME)
    add_custom_command(TARGET CopyShaders PRE_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SHADER_FILE} ${SHADERS_DEST_DIR}/${SHADER_NAME}
                       DEPENDS ${SHADER_FILE})
endforeach()

# Link libraries (you should have your link_libraries or target_link_libraries commands here)

# ... (any other target_link_libraries or add_custom_command calls)

# Make the '4th' target depend on the 'CopyShaders' target
# This ensures shaders are copied before the '4th' executable is built
add_dependencies(4th CopyShaders)
add_dependencies(SymplecticBilliards CopyShaders)
