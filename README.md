
# generate c++ file from proto file
download [protoc](https://github.com/protocolbuffers/protobuf/releases/latest) and extract.
add protoc/bin folder to path environment variable.
Run following command on cmd window 

    cd ipfs-lite
    protoc -I=. --cpp_out=. ./ipfs_lite.proto

