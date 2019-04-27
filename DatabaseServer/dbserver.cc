// Server side of obiew.

#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include <google/protobuf/text_format.h>
#include <grpcpp/grpcpp.h>

#include "multi-paxos-service-impl.h"
#include "obiew-service-impl.h"
#include "time_log.h"

using google::protobuf::TextFormat;

std::unique_ptr<grpc::Server> InitializeService(
    const std::string& service_name, const std::string& server_address,
    grpc::Service* service) {
  grpc::ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance to communicate with clients. In this
  // case, it corresponds to an *synchronous* service.
  builder.RegisterService(service);
  // Finally assemble the server.
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  // Wait for the server to shutdown.
  TIME_LOG << service_name << " Listening On: " << server_address << std::endl;
  return std::move(server);
}
void StartService(grpc::Server* server) { server->Wait(); }

int main(int argc, char** argv) {
  // Set server address.
  if (argc <= 1) {
    std::cerr
        << "Usage: `./server \"my_addr:'<addr>' my_paxos:'<addr>' fail_rate:"
           "<double> replica:'<addr>' ... replica:'<addr>'\"`"
        << std::endl
        << "Like this:" << std::endl
        << "`./server \"my_addr:'0.0.0.0:8000' my_paxos:'0.0.0.0:9000' "
           "fail_rate:0.3 replica:'0.0.0.0:9000' replica:'0.0.0.0:9001' "
           "replica:'0.0.0.0:9002'\"`"
        << std::endl;
    return -1;
  }
  obiew::ServerConfig server_config;
  TextFormat::ParseFromString(std::string(argv[1]), &server_config);
  std::string server_cfg_str;
  TextFormat::PrintToString(server_config, &server_cfg_str);
  TIME_LOG << "Server Config:" << std::endl << server_cfg_str << std::endl;

  std::map<std::string, std::unique_ptr<obiew::MultiPaxos::Stub>> stubs;
  for (int i = 0; i < server_config.replica_size(); ++i) {
    const std::string& paxos_address = server_config.replica(i);

    stubs[paxos_address] = std::move(std::unique_ptr<obiew::MultiPaxos::Stub>(
        obiew::MultiPaxos::NewStub(grpc::CreateChannel(
            paxos_address, grpc::InsecureChannelCredentials()))));
    TIME_LOG << "Adding " << paxos_address << " to the Paxos stubs list."
             << std::endl;
  }

  obiew::PaxosStubsMap paxos_stubs_map(std::move(stubs));
  const std::string& my_obiew_address = server_config.my_addr();
  const std::string& my_paxos_address = server_config.my_paxos();

  obiew::ObiewServiceImpl obiew_service(&paxos_stubs_map, my_obiew_address,
                                        my_paxos_address);
  obiew::MultiPaxosServiceImpl multi_paxos_service(&paxos_stubs_map,
                                                   my_paxos_address);
  std::unique_ptr<grpc::Server> obiew_server =
      InitializeService("ObiewService", my_obiew_address, &obiew_service);
  std::unique_ptr<grpc::Server> multi_paxos_server = InitializeService(
      "MultiPaxosService", my_paxos_address, &multi_paxos_service);

  // Starts ObiewService in a detached thread.
  std::thread obiew_thread(StartService, obiew_server.get());
  // Starts MultiPaxosService in a detached thread.
  std::thread multi_paxos_thread(StartService, multi_paxos_server.get());
  assert(multi_paxos_service.Initialize().ok());
  obiew_thread.join();
  multi_paxos_thread.join();
  TIME_LOG << "Shutting down!" << std::endl;
  return 0;
}
