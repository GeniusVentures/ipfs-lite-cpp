
# generate c++ file from proto file
download [protoc](https://github.com/protocolbuffers/protobuf/releases/latest) and extract.
add protoc/bin folder to path environment variable.
Run following command on cmd window 

    cd ipfs-lite
    protoc -I=C:/Users/user8/protoc-3.11.4-win64/include -I=. --cpp_out=. ./ipfs_lite.proto

