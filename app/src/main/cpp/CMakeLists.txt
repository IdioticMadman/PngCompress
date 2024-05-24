cmake_minimum_required(VERSION 3.22.1)

project("PngQuantize")

add_library(${CMAKE_PROJECT_NAME} SHARED native-lib.cpp)

find_library(IMAGE_QUANT imagequant_sys PATHS ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI} NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

find_library(PNG png PATHS ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI} NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(${CMAKE_PROJECT_NAME} android z log ${IMAGE_QUANT} ${PNG})
