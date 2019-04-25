#ifndef OBIEW_SERVICE_IMPL_H
#define OBIEW_SERVICE_IMPL_H

#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <utility>

#include <grpcpp/grpcpp.h>

#include "obiew.grpc.pb.h"
#include "paxos-stubs-map.h"
#include "time_log.h"

namespace obiew {

// Logic and data behind the server's behavior.
  class ObiewServiceImpl final : public Obiew::Service {
  public:
    ObiewServiceImpl(PaxosStubsMap* paxos_stubs_map, 
      const std::string& obiew_address,
      const std::string& my_paxos_address)
    : paxos_stubs_map_(paxos_stubs_map),
    obiew_address_(obiew_address),
    my_paxos_address_(my_paxos_address) {}

  // Register a user.
    grpc::Status Register(grpc::ServerContext* context, const RegisterRequest* request,
      RegisterResponse* response) override;

  // Log In.
    grpc::Status LogIn(grpc::ServerContext* context, const LogInRequest* request,
     LogInResponse* response) override;

  private:
    const std::string obiew_address_;
    const std::string my_paxos_address_;
    std::shared_mutex log_mtx_;
    PaxosStubsMap* paxos_stubs_map_;

    grpc::Status ForwardToCoordinator(
    grpc::ClientContext* cc, MultiPaxos::Stub* stub, const RegisterRequest& request,
    RegisterResponse* response);
    grpc::Status ForwardToCoordinator(
    grpc::ClientContext* cc, MultiPaxos::Stub* stub, const LogInRequest& request,
    LogInResponse* response);
    template <typename Request, typename Response>
    grpc::Status RequestFlow(const Request& request, Response* response);

  };

}  // namespace obiew

#endif