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

#ifdef BAZEL_BUILD
#include "examples/protos/ipfs_lite.grpc.pb.h"
#else
#include "ipfs_lite.grpc.pb.h"
#endif

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

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

class IpfsLiteClient {
 public:
  IpfsLiteClient(std::shared_ptr<Channel> channel)
      : stub_(IpfsLite::NewStub(channel)) {}

  
 private:
  std::unique_ptr<IpfsLite::Stub> stub_;
};

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint specified by
  // the argument "--target=" which is the only expected argument.
  // We indicate that the channel isn't authenticated (use of
  // InsecureChannelCredentials()).
  std::string target_str;
  std::string arg_str("--target");
  if (argc > 1) {
    std::string arg_val = argv[1];
    size_t start_pos = arg_val.find(arg_str);
    if (start_pos != std::string::npos) {
      start_pos += arg_str.size();
      if (arg_val[start_pos] == '=') {
        target_str = arg_val.substr(start_pos + 1);
      } else {
        std::cout << "The only correct argument syntax is --target=" << std::endl;
        return 0;
      }
    } else {
      std::cout << "The only acceptable argument is --target=" << std::endl;
      return 0;
    }
  } else {
    target_str = "localhost:50051";
  }
  IpfsLiteClient ipfsliter(grpc::CreateChannel(
      target_str, grpc::InsecureChannelCredentials()));

  std::cout << "Greeter received: " << std::endl;

  return 0;
}
