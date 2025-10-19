cmake_minimum_required(VERSION 3.10)

project(CReadUbx)

#includet
include_directories(include)
file(GLOB SOURCES "src/*.c")

#libcurl
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SOURCES})


target_link_libraries(${PROJECT_NAME} ${CURL_LIBRARIES})

# Set compatibility for Windows and Linux
if(WIN32)
    message(STATUS "Configuring for Windows")
    # Add any Windows-specific configurations here
elseif(UNIX)
    message(STATUS "Configuring for Linux")
    # Add any Linux-specific configurations here
endif()