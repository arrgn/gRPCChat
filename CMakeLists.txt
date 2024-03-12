cmake_minimum_required(VERSION 3.8)

project(grpcchat)

# set vcpkg's toolchain file for cmake
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

# set vcpkg target triplet, cmake generator platform, vcpkg_installed folder for windows
if (WIN32)
    set(VCPKG_TARGET_TRIPLET "x64-windows")
    set(CMAKE_GENERATOR_PLATFORM x64)
    set(HOST_OS "x64-windows")
endif (WIN32)

# set vcpkg_installed foldex for linux
if (UNIX)
    set(HOST_OS "x64-linux")
endif (UNIX)

# targets
add_executable(server "server.cpp" "chat.grpc.pb.h" "chat.grpc.pb.cc" "chat.pb.h" "chat.pb.cc")
add_executable(client "client.cpp" "chat.grpc.pb.h" "chat.grpc.pb.cc" "chat.pb.h" "chat.pb.cc")

# set CXX standart
set_property(TARGET server PROPERTY CXX_STANDART 17)
set_property(TARGET client PROPERTY CXX_STANDART 17)

set(INSTALLED_PATH "${CMAKE_SOURCE_DIR}/vcpkg_installed/${HOST_OS}")

# libs necessary for gRPC
set(c-ares_DIR "${INSTALLED_PATH}/share/c-ares")
set(absl_DIR "${INSTALLED_PATH}/share/absl")
set(upb_DIR "${INSTALLED_PATH}/share/upb")
set(re2_DIR "${INSTALLED_PATH}/share/re2")

# set zlib and pkgconf executable for windows
if (WIN32)
    set(ZLIB_LIBRARY "${INSTALLED_PATH}/lib/zlib.lib")
    set(PKG_CONFIG_EXECUTABLE "${INSTALLED_PATH}/tools/pkgconf/pkgconf.exe")
endif (WIN32)

# set zlib and pkgconf executable for unix
if (UNIX)
    set(ZLIB_LIBRARY "${INSTALLED_PATH}/lib/libz.a")
    set(PKG_CONFIG_EXECUTABLE "${INSTALLED_PATH}/tools/pkgconf/pkgconf")
endif (UNIX)

# protobuf
find_package(Protobuf CONFIG REQUIRED HINTS "${INSTALLED_PATH}/share/protobuf")

# gRPC
find_package(gRPC CONFIG REQUIRED HINTS "${INSTALLED_PATH}/share/grpc")
target_link_libraries(server PRIVATE gRPC::grpc++)
target_link_libraries(client PRIVATE gRPC::grpc++)

# absl
find_package(absl CONFIG REQUIRED HINTS "${INSTALLED_PATH}/share/absl")
target_link_libraries(server PRIVATE absl::strings absl::flags absl::flags_parse absl::strings)
target_link_libraries(client PRIVATE absl::strings absl::flags absl::flags_parse absl::strings)

# copy DLLs and PDBs if windows
if (WIN32)
    # set DLLs and PDBs for copy
    set(DLL_ROOT_PATH "${INSTALLED_PATH}")
    set(DLL_PATH "${DLL_ROOT_PATH}/debug/bin")
    set(DLL_TO_COPY "${DLL_PATH}/libprotobufd.dll" "${DLL_PATH}/cares.dll" "${DLL_PATH}/re2.dll" "${DLL_PATH}/abseil_dll.dll" "${DLL_ROOT_PATH}/bin/zlib1.dll")
    set(PDB_TO_COPY "${DLL_PATH}/libprotobufd.pdb" "${DLL_PATH}/cares.pdb" "${DLL_PATH}/re2.pdb" "${DLL_PATH}/abseil_dll.pdb")

    # copy DLLs and PDBs
    add_custom_command(
        TARGET server
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${DLL_TO_COPY} $<TARGET_FILE_DIR:server>
    )
    add_custom_command(
        TARGET server
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${PDB_TO_COPY} $<TARGET_FILE_DIR:server>
    )
endif (WIN32)