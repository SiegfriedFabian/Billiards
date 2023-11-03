cmake_minimum_required(VERSION 3.16)
project(SymplecticBilliards)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
add_library(glad src/glad.c)
include_directories(include)
# imgui library, details can be found here https://github.com/ocornut/imgui/
add_library(imgui include/imgui/imgui.cpp include/imgui/imgui_draw.cpp include/imgui/imgui_tables.cpp
        include/imgui/imgui_widgets.cpp include/imgui/imgui_impl_glfw.cpp include/imgui/imgui_impl_opengl3.cpp include/imgui/imgui_demo.cpp)
if(APPLE)
        add_library(glfw SHARED IMPORTED)
        set_target_properties(glfw PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/lib-universal/libglfw.3.dylib)
endif()
if(WIN32)
        add_library(glfw STATIC IMPORTED)
        set_property(TARGET glfw PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/lib-windows/glfw3.lib)
endif()
# link libraries that all targets share to all targets
link_libraries(glfw ${GL_LIBRARY} glad imgui)
add_executable(SymplecticBilliards src/symplectic.cpp src/shaderClass.cpp src/shape.cpp src/SymplecticBilliard.cpp)
add_executable(4th src/4th.cpp src/shaderClass.cpp src/shape.cpp src/Trajectory.cpp src/FourthBilliard.cpp)
#add_executable(euclidean src/euclid.cpp src/shaderClass.cpp src/shape.cpp src/Trajectory.cpp src/EuclideanBilliard.cpp)

# add folder for shaders next to executable
add_custom_command(TARGET 4th POST_BUILD 
                    COMMAND ${CMAKE_COMMAND} -E make_directory
                    "$<TARGET_FILE_DIR:4th>/shaders")

# Copy shader files for 4th into 'shader' folder
file(GLOB SHADER_FILES
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.vert"
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.frag")

foreach(SHADER ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    add_custom_command(TARGET 4th POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy
                       ${SHADER} "$<TARGET_FILE_DIR:4th>/shaders/${SHADER_NAME}")
endforeach(SHADER)