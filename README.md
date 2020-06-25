# Download thirdparty project
   
    git clone ssh://git@gitlab.geniusventures.io:8487/GeniusVentures/thirdparty.git --recursive 
    cd thirdparty
    git checkout develop

Then folder structure as follows:

    .
    └── thirdparty                          # geniustokens thirdparty
       ├── grpc                             # grpc latest version (current v1.28.1)
       ├── leveldb                          # leveldb latest version
       ├── libp2p                           # libp2p cross-compile branch
       └── ipfs-lite-cpp                    # current repo
            ├── ipfs-lite                   # sub folder
            ├── readme.md                   # readme
            └── CMakeList.txt               # CMake file
 
# Build on Windows
I used visual studio 2017 to compile ipfs-lite-cpp project.
1. download Prebuilt-Boost libraries for windows
2. download OpenSSL and install
3. build ipfs_lite using following commands:

    cd ipfs-lite-cpp
    md .build
    cd .build
    cmake ../build/Windows -G "Visual Studio 15 2017 Win64" \
        -DBUILD_TESTING=OFF \
        -DBOOST_ROOT="C:/local/boost_1_70_0" \
        -DBOOST_INCLUDE_DIR="C:/local/boost_1_70_0" \
        -DBOOST_LIBRARY_DIR="C:/local/boost_1_70_0/lib64-msvc-14.1" \
        -DOPENSSL_ROOT_DIR="C:/Program Files/OpenSSL-Win64" -DCMAKE_BUILD_TYPE=Release
    cmake --build . --config Release

if you are going to build and test , then use following commands

    cmake ../build/Windows -G "Visual Studio 15 2017 Win64" \
        -DTESTING=ON \
        -DBOOST_ROOT="C:/local/boost_1_70_0" \
        -DBOOST_INCLUDE_DIR="C:/local/boost_1_70_0" \
        -DBOOST_LIBRARY_DIR="C:/local/boost_1_70_0/lib64-msvc-14.1" \
        -DOPENSSL_ROOT_DIR="C:/Program Files/OpenSSL-Win64" -DCMAKE_BUILD_TYPE=Release
    cmake --build . --config Release
    cd ipfs-lite-cpp
    ctest -C Release

To run sepecifi test with detailed log, you can use following commands.

    ctest -C Release -R <test_name> --verbose

To run all tests and display log for failed tests, you can use following commands.

    ctest -C Release --output-on-failure



    