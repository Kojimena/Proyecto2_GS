cmake_minimum_required(VERSION 3.25)
project(Proyecto2_GS)

set(CMAKE_CXX_STANDARD 20)

add_executable(Proyecto2_GS src/main.cpp src/rc.h)

# Enable C++20 features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Link SDL2_image
find_library(SDL2_IMAGE_LIBRARY NAMES SDL2_image PATHS /opt/homebrew/lib)
target_link_libraries(${PROJECT_NAME} ${SDL2_IMAGE_LIBRARY})

# Link SDL2_ttf
find_library(SDL2_TTF_LIBRARY NAMES SDL2_ttf PATHS /opt/homebrew/Cellar/sdl2_ttf/)
target_link_libraries(${PROJECT_NAME} ${SDL2_TTF_LIBRARY})


file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        )

target_include_directories(${PROJECT_NAME}
        PRIVATE ${PROJECT_SOURCE_DIR}/include
        PUBLIC ${PROJECT_SOURCE_DIR}/src
        PUBLIC /opt/homebrew/include/SDL2/
        PUBLIC /opt/homebrew/Cellar/sdl2_ttf/2.0.15/include/SDL2/
        )



target_link_libraries(${PROJECT_NAME}
        ${SDL2_LIBRARIES}
        )