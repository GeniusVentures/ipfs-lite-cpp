# Assume folder structure
this projects needs grpc, so you have to download grpc repository first.

    git clone -b v1.28.1 https://github.com/grpc/grpc
    git clone -b develop ssh://git@gitlab.geniusventures.io:8487/GeniusVentures/ipfs-lite-cpp.git

Then folder structure as follows:

    .
    ├── grpc                             # grpc v1.28.0
    ├── ipfs-lite-cpp                    # current repo
        ├── ipfs-lite                    # sub folder
        ├── readme.md                    # readme
