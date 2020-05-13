/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#ifdef BAZEL_BUILD
#include "examples/protos/ipfs_lite.grpc.pb.h"
#else
#include "ipfs_lite.grpc.pb.h"
#endif

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
// Model messages

using ipfs_lite::AddParams;
using ipfs_lite::Block;
using ipfs_lite::Link;
using ipfs_lite::NodeStat;
using ipfs_lite::Node;

// Request and Response messages

using ipfs_lite::AddFileRequest;
using ipfs_lite::AddFileResponse;

using ipfs_lite::GetFileRequest;
using ipfs_lite::GetFileResponse;

using ipfs_lite::AddNodeRequest;
using ipfs_lite::AddNodeResponse;

using ipfs_lite::AddNodesRequest;
using ipfs_lite::AddNodesResponse;

using ipfs_lite::GetNodeRequest;
using ipfs_lite::GetNodeResponse;

using ipfs_lite::GetNodesRequest;
using ipfs_lite::GetNodesResponse;

using ipfs_lite::RemoveNodeRequest;
using ipfs_lite::RemoveNodeResponse;

using ipfs_lite::RemoveNodesRequest;
using ipfs_lite::RemoveNodesResponse;

using ipfs_lite::ResolveLinkRequest;
using ipfs_lite::ResolveLinkResponse;

using ipfs_lite::TreeRequest;
using ipfs_lite::TreeResponse;

using ipfs_lite::DeleteBlockRequest;
using ipfs_lite::DeleteBlockResponse;

using ipfs_lite::HasBlockRequest;
using ipfs_lite::HasBlockResponse;
using ipfs_lite::GetBlockSizeRequest;

using ipfs_lite::GetBlockSizeResponse;
using ipfs_lite::PutBlockRequest;
using ipfs_lite::PutBlockResponse;
using ipfs_lite::PutBlocksRequest;

using ipfs_lite::PutBlocksResponse;

using ipfs_lite::AllKeysRequest;

using ipfs_lite::AllKeysResponse;

using ipfs_lite::HashOnReadRequest;

using ipfs_lite::HashOnReadResponse;

using ipfs_lite::IpfsLite;

// Logic and data behind the server's behavior.
class IpfsLiteServiceImpl final : public IpfsLite::Service {

};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  IpfsLiteServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();

  return 0;
}
