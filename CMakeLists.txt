# Copyright 2018 gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# cmake "superbuild" file for C++ helloworld example.
# This build file demonstrates how to build the helloworld project
# and all its dependencies in a single cmake build (hence "superbuild")
# that is easy to build and maintain.
# cmake's ExternalProject_Add() is used to import all the sub-projects,
# including the "helloworld" project itself.
# See https://blog.kitware.com/cmake-superbuilds-git-submodules/

cmake_minimum_required(VERSION 3.5.1)

# Project
project(ipfs-lite-SuperBuild C CXX)

include(ExternalProject)

# Note: For all external projects, instead of using checked-out code, one could
# specify GIT_REPOSITORY and GIT_TAG to have cmake download the dependency directly,
# without needing to add a submodule to your project.

# Builds absl project from the git submodule.
ExternalProject_Add(absl
  PREFIX absl
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../grpc/third_party/abseil-cpp"
  CMAKE_CACHE_ARGS
        -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=TRUE
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/absl
)

# Builds c-ares project from the git submodule.
ExternalProject_Add(c-ares
  PREFIX c-ares
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../grpc/third_party/cares/cares"
  CMAKE_CACHE_ARGS
        -DCARES_SHARED:BOOL=OFF
        -DCARES_STATIC:BOOL=ON
        -DCARES_STATIC_PIC:BOOL=ON
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/c-ares
)

# Builds protobuf project from the git submodule.
ExternalProject_Add(protobuf
  PREFIX protobuf
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../grpc/third_party/protobuf/cmake"
  CMAKE_CACHE_ARGS
        -Dprotobuf_BUILD_TESTS:BOOL=OFF
        -Dprotobuf_WITH_ZLIB:BOOL=OFF
        -Dprotobuf_MSVC_STATIC_RUNTIME:BOOL=OFF
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/protobuf
)

# Builds zlib project from the git submodule.
ExternalProject_Add(zlib
  PREFIX zlib
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../grpc/third_party/zlib"
  CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/zlib
)

# the location where protobuf-config.cmake will be installed varies by platform
if (WIN32)
  set(_FINDPACKAGE_PROTOBUF_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/protobuf/cmake")
else()
  set(_FINDPACKAGE_PROTOBUF_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/protobuf/lib/cmake/protobuf")
endif()
set(_FINDPACKAGE_PROTOBUF_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/protobuf/include")
set(_FINDPACKAGE_PROTOBUF_LIB_DIR "${CMAKE_CURRENT_BINARY_DIR}/protobuf/lib")

# if OPENSSL_ROOT_DIR is set, propagate that hint path to the external projects with OpenSSL dependency.
set(_CMAKE_ARGS_OPENSSL_ROOT_DIR "")
if (OPENSSL_ROOT_DIR)
  set(_CMAKE_ARGS_OPENSSL_ROOT_DIR "-DOPENSSL_ROOT_DIR:PATH=${OPENSSL_ROOT_DIR}")
endif()

# Builds gRPC based on locally checked-out sources and set arguments so that all the dependencies
# are correctly located.
ExternalProject_Add(grpc
  PREFIX grpc
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../grpc"
  CMAKE_CACHE_ARGS
        -DgRPC_INSTALL:BOOL=ON
        -DgRPC_BUILD_TESTS:BOOL=OFF
        -DgRPC_PROTOBUF_PROVIDER:STRING=package
        -DgRPC_PROTOBUF_PACKAGE_TYPE:STRING=CONFIG
        -DProtobuf_DIR:PATH=${_FINDPACKAGE_PROTOBUF_CONFIG_DIR}
        -DgRPC_ZLIB_PROVIDER:STRING=package
        -DZLIB_ROOT:STRING=${CMAKE_CURRENT_BINARY_DIR}/zlib
        -DgRPC_ABSL_PROVIDER:STRING=package
        -Dabsl_DIR:STRING=${CMAKE_CURRENT_BINARY_DIR}/absl/lib/cmake/absl
        -DgRPC_CARES_PROVIDER:STRING=package
        -Dc-ares_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR}/c-ares/lib/cmake/c-ares
        -DgRPC_SSL_PROVIDER:STRING=package
        ${_CMAKE_ARGS_OPENSSL_ROOT_DIR}
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/grpc
  DEPENDS c-ares protobuf zlib absl
)

# Builds leveldb
ExternalProject_Add(leveldb
  PREFIX leveldb
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../leveldb"
  CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/leveldb
)
# Builds Microsoft.GSL
ExternalProject_Add(Microsoft.GSL
  PREFIX Microsoft.GSL
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../GSL"
  CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/GSL
        -DGSL_TEST:BOOL=OFF
)
set(_FINDPACKAGE_GSL_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/GSL/include")
# Builds spdlog v1.4.2
ExternalProject_Add(spdlog
  PREFIX spdlog
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../spdlog"
  CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/spdlog
)
set(_FINDPACKAGE_SPDLOG_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/spdlog/lib/spdlog/cmake")
set(_FINDPACKAGE_SPDLOG_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/spdlog/include")

# Builds tsl_hat_trie 343e0dac54fc8491065e8a059a02db9a2b1248ab
ExternalProject_Add(tsl_hat_trie
  PREFIX tsl_hat_trie
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../hat-trie"
  CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/hat-trie
)
# Builds Boost.DI c5287ee710ad90f5286d0cc2b9e49b72d89267a6
ExternalProject_Add(Boost.DI
  PREFIX Boost.DI
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Boost.DI"
  CMAKE_CACHE_ARGS
        -DBOOST_DI_OPT_BUILD_TESTS:BOOL=OFF
        -DBOOST_DI_OPT_BUILD_EXAMPLES:BOOL=OFF
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/Boost.DI
)
set(_FINDPACKAGE_BOOST_DI_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/Boost.DI/lib/cmake/Boost.DI")


# BOOST library 
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost REQUIRED COMPONENTS date_time filesystem random) # header only libraries must not be added here

# Builds libp2p

ExternalProject_Add(libp2p
  PREFIX libp2p
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../libp2p"
  CMAKE_CACHE_ARGS
        ${_CMAKE_ARGS_OPENSSL_ROOT_DIR}
        -DCMAKE_USE_OPENSSL=ON
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/libp2p
        -DTESTING:BOOL=OFF 
        -DEXPOSE_MOCKS:BOOL=ON
        -DHUNTER_ENABLED:BOOL=OFF
        -DBUILD_EXAMPLES:BOOL=OFF
        -DProtobuf_DIR:PATH=${_FINDPACKAGE_PROTOBUF_CONFIG_DIR}
        -DProtobuf_INCLUDE_DIR:PATH=${_FINDPACKAGE_PROTOBUF_INCLUDE_DIR}
        -DProtobuf_LIBRARIES:PATH=${_FINDPACKAGE_PROTOBUF_LIB_DIR}
        -Dspdlog_DIR:PATH=${_FINDPACKAGE_SPDLOG_CONFIG_DIR}
        -DBoost.DI_DIR:PATH=${_FINDPACKAGE_BOOST_DI_CONFIG_DIR}
        -DBOOST_ROOT:PATH="${BOOST_ROOT}"
        -DBOOST_INCLUDE_DIR:PATH="${BOOST_INCLUDE_DIR}"
        -DBOOST_LIBRARY_DIR:PATH="${BOOST_LIBRARY_DIR}"
        -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}\ -I\ ${BOOST_INCLUDE_DIR}\ -I\ ${_FINDPACKAGE_GSL_INCLUDE_DIR}\ -I\ ${_FINDPACKAGE_SPDLOG_INCLUDE_DIR}\ -I\ \\\"${OPENSSL_ROOT_DIR}/include\\\"\ -I\ ${_FINDPACKAGE_PROTOBUF_INCLUDE_DIR}

  DEPENDS protobuf spdlog tsl_hat_trie Boost.DI 
)

# Build the helloworld projects itself using a CMakeLists.txt that assumes all the dependencies
# have already been installed.
# Even though helloworld is not really an "external project" from perspective of this build,
# we are still importing it using ExternalProject_Add because that allows us to use find_package()
# to locate all the dependencies (if we were building helloworld directly in this build we,
# we would have needed to manually import the libraries as opposed to reusing targets exported by
# gRPC and protobuf).
ExternalProject_Add(ipfs-lite
  PREFIX ipfs-lite
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ipfs-lite"
  BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/ipfs-lite"
  INSTALL_COMMAND ""
  CMAKE_CACHE_ARGS
        -DProtobuf_DIR:PATH=${_FINDPACKAGE_PROTOBUF_CONFIG_DIR}
        -Dc-ares_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR}/c-ares/lib/cmake/c-ares
        -DZLIB_ROOT:STRING=${CMAKE_CURRENT_BINARY_DIR}/zlib
        -Dabsl_DIR:STRING=${CMAKE_CURRENT_BINARY_DIR}/absl/lib/cmake/absl
        ${_CMAKE_ARGS_OPENSSL_ROOT_DIR}
        -DgRPC_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR}/grpc/lib/cmake/grpc
  DEPENDS protobuf grpc libp2p leveldb
#  DEPENDS protobuf grpc leveldb
)
