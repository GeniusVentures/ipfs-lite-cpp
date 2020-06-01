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
using grpc::ServerReader;
using grpc::ServerWriter;
// Model messages

using sgns::AddParams;
using sgns::Block;
using sgns::Link;
using sgns::NodeStat;
using sgns::Node;

// Request and Response messages

using sgns::AddFileRequest;
using sgns::AddFileResponse;

using sgns::GetFileRequest;
using sgns::GetFileResponse;

using sgns::AddNodeRequest;
using sgns::AddNodeResponse;

using sgns::AddNodesRequest;
using sgns::AddNodesResponse;

using sgns::GetNodeRequest;
using sgns::GetNodeResponse;

using sgns::GetNodesRequest;
using sgns::GetNodesResponse;

using sgns::RemoveNodeRequest;
using sgns::RemoveNodeResponse;

using sgns::RemoveNodesRequest;
using sgns::RemoveNodesResponse;

using sgns::ResolveLinkRequest;
using sgns::ResolveLinkResponse;

using sgns::TreeRequest;
using sgns::TreeResponse;

using sgns::DeleteBlockRequest;
using sgns::DeleteBlockResponse;

using sgns::HasBlockRequest;
using sgns::HasBlockResponse;
using sgns::GetBlockSizeRequest;

using sgns::GetBlockSizeResponse;
using sgns::PutBlockRequest;
using sgns::PutBlockResponse;
using sgns::PutBlocksRequest;

using sgns::PutBlocksResponse;

using sgns::AllKeysRequest;

using sgns::AllKeysResponse;

using sgns::HashOnReadRequest;

using sgns::HashOnReadResponse;

using sgns::IpfsLite;

// Logic and data behind the server's behavior.
class IpfsLiteServiceImpl final : public IpfsLite::Service {

  Status AddFile(ServerContext* context, ServerReader<AddFileRequest>* reader, AddFileResponse* response) override {
    return Status::OK;
  }
  Status GetFile(ServerContext* context, const GetFileRequest* request, ServerWriter<GetFileResponse>* writer) override {
    return Status::OK;
  }
  Status HasBlock(ServerContext* context, const HasBlockRequest* request, HasBlockResponse* response) override {
    return Status::OK;
  }
  // DAGService
  //
  Status AddNode(ServerContext* context, const AddNodeRequest* request, AddNodeResponse* response) override {
    return Status::OK;
  }
  Status AddNodes(ServerContext* context, const AddNodesRequest* request, AddNodesResponse* response) override {
    return Status::OK;
  }
  Status GetNode(ServerContext* context, const GetNodeRequest* request, GetNodeResponse* response) override {
    return Status::OK;
  }
  Status GetNodes(ServerContext* context, const GetNodesRequest* request, ServerWriter<GetNodesResponse>* writer) override {
    return Status::OK;
  }
  Status RemoveNode(ServerContext* context, const RemoveNodeRequest* request, RemoveNodeResponse* response) override {
    return Status::OK;
  }
  Status RemoveNodes(ServerContext* context, const RemoveNodesRequest* request, RemoveNodesResponse* response) override {
    return Status::OK;
  }
  // Node provides a ResloveLink method and the Resolver methods Resolve and Tree
  //
  Status ResolveLink(ServerContext* context, const ResolveLinkRequest* request, ResolveLinkResponse* response) override {
    return Status::OK;
  }
  Status Tree(ServerContext* context, const TreeRequest* request, TreeResponse* response) override {
    return Status::OK;
  }
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
