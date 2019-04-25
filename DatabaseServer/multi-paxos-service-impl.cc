#include <cstdlib>

#include "multi-paxos-service-impl.h"

namespace obiew {

  using grpc::ClientContext;
  using grpc::ServerContext;
  using grpc::Status;
  using obiew::RegisterRequest;
  using obiew::RegisterResponse;
  using obiew::LogInRequest;
  using obiew::LogInResponse;
  using obiew::User;



  // Find Coordinator and recover data from Coordinator on construction.
  Status MultiPaxosServiceImpl::Initialize() {
  // // Try to get Coordinator address from other replicas.
  //   Status get_status = GetCoordinator();
  //   auto* coordinator_stub = paxos_stubs_map_->GetCoordinatorStub();
  // // If not successful, start an election for Coordinators.
  //   if (!get_status.ok()) {
  //     Status elect_status = ElectNewCoordinator();
  //     if (!elect_status.ok()) {
  //       return Status(
  //         grpc::StatusCode::ABORTED,
  //         "ElectNewCoordinator Failed: " + elect_status.error_message());
  //     }
  //   }
  //   Status recover_status = GetRecovery();
  //   if (!recover_status.ok()) {
  //     return Status(grpc::StatusCode::ABORTED,
  //       "GetRecovery Failed: " + recover_status.error_message());
  //   }
    paxos_stubs_map_->SetCoordinator(my_paxos_address_);
    return Status::OK;
  }
  Status MultiPaxosServiceImpl::Register(grpc::ServerContext* context, const RegisterRequest* request,
    RegisterResponse* response) {
    {
      std::unique_lock<std::shared_mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
      << "[Coordinator] Received RegisterRequest."
      << std::endl;
    }
    User user;
    user.set_user_id(10);
    user.set_name("zjh");
    *response->mutable_user() = user;
    {
      std::unique_lock<std::shared_mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
      << "[Coordinator] Returning RegisterResponse." << std::endl;
    }
    return Status::OK;
  }

  Status MultiPaxosServiceImpl::LogIn(grpc::ServerContext* context, const LogInRequest* request,
    LogInResponse* response) {
    {
      std::unique_lock<std::shared_mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
      << "[Coordinator] Received LogInRequest."
      << std::endl;
    }
    

    {
      std::unique_lock<std::shared_mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
      << "[Coordinator] Returning LogInResponse." << std::endl;
    }
    return Status::OK;
  }

}  // namespace obiew
