cmake_minimum_required(VERSION 3.8)

project(grpcchat)

set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

set(VCPKG_TARGET_TRIPLET "x64-windows")
set(CMAKE_GENERATOR_PLATFORM x64)

add_executable(server "server.cpp" "chat.grpc.pb.h" "chat.grpc.pb.cc" "chat.pb.h" "chat.pb.cc")
add_executable(client "client.cpp" "chat.grpc.pb.h" "chat.grpc.pb.cc" "chat.pb.h" "chat.pb.cc")

set_property(TARGET server PROPERTY CXX_STANDART 17)
set_property(TARGET client PROPERTY CXX_STANDART 17)

set(ZLIB_LIBRARY "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/lib/zlib.lib")
set(c-ares_DIR "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/share/c-ares")
set(absl_DIR "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/share/absl")
set(upb_DIR "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/share/upb")
set(re2_DIR "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/share/re2")
set(PKG_CONFIG_EXECUTABLE "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/tools/pkgconf/pkgconf.exe")

find_package(Protobuf CONFIG REQUIRED HINTS "./vcpkg_installed/x64-windows/share/protobuf")

find_package(gRPC CONFIG REQUIRED HINTS "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/share/grpc")
target_link_libraries(server PRIVATE gRPC::grpc++)
target_link_libraries(client PRIVATE gRPC::grpc++)

find_package(absl CONFIG REQUIRED HINTS "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/share/absl")
target_link_libraries(server PRIVATE absl::strings absl::flags absl::flags_parse absl::strings)
target_link_libraries(client PRIVATE absl::strings absl::flags absl::flags_parse absl::strings)

set(DLL_ROOT_PATH "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows")
set(DLL_PATH "${DLL_ROOT_PATH}/debug/bin")
set(DLL_TO_COPY "${DLL_PATH}/libprotobufd.dll" "${DLL_PATH}/cares.dll" "${DLL_PATH}/re2.dll" "${DLL_PATH}/abseil_dll.dll" "${DLL_ROOT_PATH}/bin/zlib1.dll")
set(PDB_TO_COPY "${DLL_PATH}/libprotobufd.pdb" "${DLL_PATH}/cares.pdb" "${DLL_PATH}/re2.pdb" "${DLL_PATH}/abseil_dll.pdb" "${DLL_ROOT_PATH}/bin/zlib1.pdb")

add_custom_command(
    TARGET server
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy ${DLL_TO_COPY} $<TARGET_FILE_DIR:server>
)