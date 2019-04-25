#ifndef MULTI_PAXOS_SERVICE_IMPL_H
#define MULTI_PAXOS_SERVICE_IMPL_H

#include <string>
#include <vector>

#include <grpcpp/grpcpp.h>

#include "obiew.grpc.pb.h"
#include "paxos-stubs-map.h"
#include "time_log.h"

namespace obiew {

  class MultiPaxosServiceImpl final : public MultiPaxos::Service {
  public:
    // Construction method.
    MultiPaxosServiceImpl(
      PaxosStubsMap* paxos_stubs_map, 
      const std::string& my_paxos_address)
    : paxos_stubs_map_(paxos_stubs_map),
    my_paxos_address_(my_paxos_address) {}
    
    grpc::Status Initialize();

  // Register a user.
    grpc::Status Register(grpc::ServerContext* context, const RegisterRequest* request,
      RegisterResponse* response) override;

  // Log In.
    grpc::Status LogIn(grpc::ServerContext* context, const LogInRequest* request,
     LogInResponse* response) override;



  private:
    const std::string my_paxos_address_;
    std::shared_mutex log_mtx_;
    PaxosStubsMap* paxos_stubs_map_;
  };

}  // namespace obiew

#endif