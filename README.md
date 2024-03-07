# gRPCChat
Study gRPC chat project

# How to run
This is a windows example using Visual Studio 17 2022 generator. The project is x64.
```
# Install vcpkg libraries
vcpkg install

# Generate proto files
.\vcpkg_installed\x64-windows\tools\protobuf\protoc.exe --cpp_out=. --grpc_out=. --plugin=protoc-gen-grpc=.\vcpkg_installed\x64-windows\tools\grpc\grpc_cpp_plugin.exe chat.proto

# Generate build files
cmake -S . -B .\build\ -G "Visual Studio 17 2022" -T host=x64 -A x64

# Build the project
cmake --build .\build\ --target ALL_BUILD --config Debug --

# Run the project
.\build\Debug\server.exe --port=50051
.\build\Debug\client.exe --host=localhost:50051 --username=user
````