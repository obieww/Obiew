// Server side of obiew.
#include "obiew-service-impl.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <set>
#include <shared_mutex>
#include <thread>
#include <utility>

#include <grpcpp/grpcpp.h>

#include "obiew.grpc.pb.h"
#include "time_log.h"

namespace obiew {

  using grpc::ClientContext;
  using grpc::ServerContext;
  using grpc::Status;
  using obiew::RegisterRequest;
  using obiew::RegisterResponse;
  using obiew::LogInRequest;
  using obiew::LogInResponse;

  Status ObiewServiceImpl::Register(grpc::ServerContext* context, const RegisterRequest* request,
    RegisterResponse* response) {
    {
      std::unique_lock<std::shared_mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << obiew_address_ << "] "
      << "Received RegisterRequest."
      << std::endl;
    }
    Status get_status = RequestFlow(*request, response);
    {
      std::unique_lock<std::shared_mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << obiew_address_ << "] "
      << "Returning RegisterResponse." << std::endl;
    }
    return get_status;
  }

  Status ObiewServiceImpl::LogIn(grpc::ServerContext* context, const LogInRequest* request,
    LogInResponse* response) {
    {
      std::unique_lock<std::shared_mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << obiew_address_ << "] "
      << "Received LogInRequest."
      << std::endl;
    }
    Status get_status = RequestFlow(*request, response);
    {
      std::unique_lock<std::shared_mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << obiew_address_ << "] "
      << "Returning LogInResponse." << std::endl;
    }
    return get_status;
  }

  // Forward RegisterRequest to Coordinator.
  Status ObiewServiceImpl::ForwardToCoordinator(
    ClientContext* cc, MultiPaxos::Stub* stub, const RegisterRequest& request,
    RegisterResponse* response) {
    return stub->Register(cc, request, response);
  }

  // Forward LogInRequest to Coordinator.
  Status ObiewServiceImpl::ForwardToCoordinator(
    ClientContext* cc, MultiPaxos::Stub* stub, const LogInRequest& request,
    LogInResponse* response) {
    return stub->LogIn(cc, request, response);
  }

template <typename Request, typename Response>
  Status ObiewServiceImpl::RequestFlow(const Request& request,
   Response* response) {
    assert(paxos_stubs_map_ != nullptr);
    auto* coordinator_stub = paxos_stubs_map_->GetCoordinatorStub();

    if (coordinator_stub == nullptr) {
      return Status(grpc::StatusCode::ABORTED, "Coordinator is not set.");
    }
    ClientContext cc;
    auto deadline =
    std::chrono::system_clock::now() + std::chrono::milliseconds(5000);
    cc.set_deadline(deadline);
  // Forward request to Coordinator.
    Status forward_status =
    ForwardToCoordinator(&cc, coordinator_stub, request, response);

  // // Elect a new Coordinator if the current one is unavailable.
  //   if (forward_status.error_code() == grpc::StatusCode::UNAVAILABLE ||
  //     forward_status.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
  //     Status election_status = ElectNewCoordinator();
  //   coordinator_stub = paxos_stubs_map_->GetCoordinatorStub();
  //   if (!election_status.ok() || coordinator_stub == nullptr) {
  //     return Status(
  //       election_status.error_code(),
  //       "Can't reach Coordinator. Failed to elect a new Coordinator. " +
  //       election_status.error_message());
  //   }
  //   // Forward request to new Coordinator.
  //   ClientContext new_cc;
  //   forward_status =
  //   ForwardToCoordinator(&new_cc, coordinator_stub, request, response);
    if (!forward_status.ok()) {
      return Status(forward_status.error_code(),
        "Failed to communicate with Coordinator. " +
        forward_status.error_message());
    }
  // }
  return forward_status;
}

} //namespace obiew