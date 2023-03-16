cmake_minimum_required(VERSION 2.8)
project(AsrSpeechEngine)
set ( CMAKE_VERBOSE_MAKEFILE on )
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -g ")

find_package(OpenSSL REQUIRED)

message(STATUS "CMAKE_MODULES_PATH=${CMAKE_MODULES_PATH}")
message(STATUS "OPENSSL_INCLUDE_DIR=${OPENSSL_INCLUDE_DIR}")
message(STATUS "OPENSSL_LIBRARIES=${OPENSSL_LIBRARIES}")

# setting Flags
if(CMAKE_SYSTEM_NAME MATCHES "Linux")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-rpath,./lib  -Wl,-rpath,./third_lib -O0 -g")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLINUX_SDK_VER_MAIN=1 -DLINUX_SDK_VER_SUB=0 -DLINUX_SDK_VER_SVN=10000 --std=c++11 -Wl,-rpath,./lib  -Wl,-rpath,./third_lib -O0 -g")

#    link_directories(
#        ${CMAKE_SOURCE_DIR}/lib
#    )
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "/EHsc ${CMAKE_CXX_FLAGS}")
endif()

message(STATUS " operation system is ${CMAKE_SYSTEM} name ${CMAKE_SYSTEM_NAME}")

link_libraries(
TRTCEngine
dl
m
pthread
z
)

aux_source_directory(${CMAKE_SOURCE_DIR}/asr ASR_SRC_FILES)
aux_source_directory(${CMAKE_SOURCE_DIR}/common ASR_SRC_FILES)
aux_source_directory(${CMAKE_SOURCE_DIR}/business/usages ASR_SRC_FILES)
include_directories(${CMAKE_SOURCE_DIR}/asr/include)
include_directories(${CMAKE_SOURCE_DIR}/common/include)
include_directories(${CMAKE_SOURCE_DIR}/include)

if(MSVC)
    include_directories(${CMAKE_SOURCE_DIR}/asr/include/pthread_win)
    set_property(TARGET ${TARGET_NAME} APPEND PROPERTY LINK_FLAGS "/NODEFAULTLIB")
endif()

include(CMakeHelpers.cmake)
init_target(AsrSpeechEngine)
link_directories(${CMAKE_SOURCE_DIR}/lib)
build_executable (${TARGET_NAME} ${ASR_SRC_FILES} business/main.cpp)
link_boost ()
final_target ()

if(MSVC)
if (CMAKE_CL_64)
	target_link_libraries(${TARGET_NAME} libboost_date_time-vc142-mt-gd-x64-1_74 libboost_regex-vc142-mt-gd-x64-1_74 libboost_random-vc142-mt-gd-x64-1_74 pthreadVC2 libcrypto )
else()
	target_link_libraries(${TARGET_NAME} libboost_date_time-vc142-mt-gd-x32-1_74 libboost_regex-vc142-mt-gd-x32-1_74 libboost_random-vc142-mt-gd-x32-1_74 pthreadVSE2 libcrypto )
endif ()
else ()
    target_link_libraries(${TARGET_NAME} -lboost_chrono -lboost_system -lboost_random -lpthread -lcrypto )
endif()

target_link_libraries(${PROJECT_NAME} ssl crypto)




