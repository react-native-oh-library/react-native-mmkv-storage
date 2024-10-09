# the minimum version of CMake.
cmake_minimum_required(VERSION 3.14)
set(CMAKE_VERBOSE_MAKEFILE on)
set(MMKV_SRC_DIR "${NODE_MODULES}/react-native-mmkv-storage/MMKV/Core")

file(GLOB rnoh_mmkv_SRC CONFIGURE_DEPENDS
 *.cpp
)
add_library(rnoh_mmkv_storage SHARED ${rnoh_mmkv_SRC})


target_include_directories(rnoh_mmkv_storage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(
  rnoh_mmkv_storage
  PRIVATE
  "${MMKV_SRC_DIR}"
  "${REANIMATED_SRC_DIR}/aes"
  "${REANIMATED_SRC_DIR}/aes/openssl"
  "${REANIMATED_SRC_DIR}/crc32"
  "${REANIMATED_SRC_DIR}/crc32/zlib"
)
add_subdirectory( "${MMKV_SRC_DIR}" Core)
target_link_libraries(rnoh_mmkv_storage PUBLIC rnoh)
target_link_libraries(rnoh_mmkv_storage PUBLIC libace_napi.z.so)
target_link_libraries(rnoh_mmkv_storage PUBLIC core)