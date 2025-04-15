cmake_minimum_required(VERSION 3.5.0)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)
    MESSAGE(STATUS "Setting CMAKE_TOOLCHAIN_FILE in root : ${CMAKE_TOOLCHAIN_FILE}")
endif()  

project(ArUcoMarkers VERSION 0.1.0 LANGUAGES C CXX)

include(FetchContent)

if(NOT DEFINED CMAKE_CXX_STANDARD)
	set(CMAKE_CXX_STANDARD 17)
endif() 
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(LINUX)
    set(OpenCV_DIR "${VCPKG_INSTALLED_DIR}/x64-linux/share/opencv4")
    MESSAGE(STATUS "Setting OpenCV_DIR for Linux in : ${OpenCV_DIR}")
endif()
if(WIN32)
    set(OpenCV_DIR "${VCPKG_INSTALLED_DIR}/x64-windows/share/opencv4")
    MESSAGE(STATUS "Setting OpenCV_DIR for Windows in : ${OpenCV_DIR}")
endif()
find_package(OpenCV REQUIRED)
find_package(podofo CONFIG REQUIRED)

add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE ${OpenCV_LIBS})
target_link_libraries(app PRIVATE $<IF:$<TARGET_EXISTS:podofo_shared>,podofo_shared,podofo_static>)

