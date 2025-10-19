cmake_minimum_required(VERSION 3.10)

project(CReadUbx)

#includet
include_directories(include)
file(GLOB SOURCES "src/*.c")

# Improve dependency detection on Windows/MSYS2 so configure succeeds first time where possible.
if(WIN32)
    message(STATUS "Configuring for Windows - attempting MSYS2/Mingw auto-detection")
    set(MSYS2_ROOT "C:/msys64")
    if(EXISTS "${MSYS2_ROOT}")
        set(_cmake_prefixes "")
        foreach(_triplet IN ITEMS mingw64 ucrt64)
            set(_p "${MSYS2_ROOT}/${_triplet}")
            if(EXISTS "${_p}/lib/cmake")
                list(APPEND _cmake_prefixes "${_p}/lib/cmake")
            endif()
            # If OpenSSL headers/libs are present under the triplet, set OPENSSL_ROOT_DIR so FindOpenSSL succeeds
            if(EXISTS "${_p}/include/openssl" AND NOT DEFINED OPENSSL_ROOT_DIR)
                set(OPENSSL_ROOT_DIR "${_p}" CACHE PATH "OpenSSL root (auto-detected)" FORCE)
            endif()
            # zlib
            if(EXISTS "${_p}/include/zlib.h" AND NOT DEFINED ZLIB_ROOT)
                set(ZLIB_ROOT "${_p}" CACHE PATH "zlib root (auto-detected)" FORCE)
            endif()
        endforeach()
        if(_cmake_prefixes)
            list(REMOVE_DUPLICATES _cmake_prefixes)
            foreach(p IN LISTS _cmake_prefixes)
                list(APPEND CMAKE_PREFIX_PATH "${p}")
            endforeach()
            message(STATUS "Added MSYS2 CMake prefixes: ${_cmake_prefixes}")
        else()
            message(STATUS "MSYS2 found but no triplet cmake prefixes located")
        endif()
    else()
        message(STATUS "MSYS2 root not found at ${MSYS2_ROOT}; normal find_package behavior will be used")
    endif()
endif()

# Prefer config-mode find_package (uses CURLConfig.cmake in CMAKE_PREFIX_PATH) so packaged curl finds its dependencies.
find_package(CURL CONFIG QUIET)
if(NOT TARGET CURL::libcurl)
    # fallback to module mode which sets CURL_LIBRARIES/CURL_INCLUDE_DIRS
    find_package(CURL REQUIRED)
endif()

add_executable(${PROJECT_NAME} ${SOURCES})

# Configure include directories for the target
if(TARGET CURL::libcurl)
    target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)
    # imported target contains include paths
elseif(DEFINED CURL_LIBRARIES AND CURL_LIBRARIES)
    target_include_directories(${PROJECT_NAME} PRIVATE ${CURL_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CURL_LIBRARIES})
elseif(DEFINED CURL_LIBRARY AND CURL_LIBRARY)
    # older FindCURL may set CURL_LIBRARY instead of CURL_LIBRARIES
    if(DEFINED CURL_INCLUDE_DIR)
        target_include_directories(${PROJECT_NAME} PRIVATE ${CURL_INCLUDE_DIR})
    elseif(DEFINED CURL_INCLUDE_DIRS)
        target_include_directories(${PROJECT_NAME} PRIVATE ${CURL_INCLUDE_DIRS})
    endif()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CURL_LIBRARY})
else()
    message(FATAL_ERROR "libcurl was found but no usable link variables were set. CURL_LIBRARIES/CURL_LIBRARY or CURL::libcurl must be available.")
endif()



add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/AJOMATKA.ubx
            ${CMAKE_BINARY_DIR}/AJOMATKA.ubx
    COMMENT "Copying AJOMATKA.ubx to the build folder"
)