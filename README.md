# Assumed folder structure
This projects needs grpc, so you have to download grpc repository first.

    git clone -b v1.28.1 https://github.com/grpc/grpc
    git clone -b develop ssh://git@gitlab.geniusventures.io:8487/GeniusVentures/ipfs-lite-cpp.git

Then folder structure as follows:

    .
    ├── thirdparty                       # grpc latest version (current v1.28.1)
    │   ├── grpc                         # grpc latest version (current v1.28.1)
    │   ├── leveldb                      # leveldb latest version
    │   └── libp2p                       # libp2p cross-compile branch
    └── ipfs-lite-cpp                    # current repo
        ├── ipfs-lite                    # sub folder
        ├── readme.md                    # readme
        └── CMakeList.txt                # CMake file

# Build on Windows
please refer grpc build requirements.

    cd ipfs-lite-cpp
    md .build
    cd .build
    cmake .. -G "Visual Studio 15 2017 Win64" -DBUILD_TESTING=OFF -DCMAKE_USE_OPENSSL=ON
    cmake --build . --config Release

