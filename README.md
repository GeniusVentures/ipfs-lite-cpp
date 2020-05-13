# Assumed folder structure
This projects needs grpc, so you have to download grpc repository first.

    git clone -b v1.28.1 https://github.com/grpc/grpc
    git clone -b develop ssh://git@gitlab.geniusventures.io:8487/GeniusVentures/ipfs-lite-cpp.git

Then folder structure as follows:

    .
    ├── grpc                             # grpc v1.28.0
    ├── ipfs-lite-cpp                    # current repo
        ├── ipfs-lite                    # sub folder
        ├── readme.md                    # readme

# Build on Windows
please refer grpc build requirements.

    cd ipfs-lite-cpp
    md .build
    cd .build
    cmake .. -G "Visual Studio 15 2017 Win64" -DBUILD_TESTING=OFF -DCMAKE_USE_OPENSSL=ON
    cmake --build . --config Release

