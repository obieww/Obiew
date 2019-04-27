// Server side of obiew.
#include "obiew-service-impl.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <set>
#include <thread>
#include <utility>

#include <grpcpp/grpcpp.h>

#include "obiew.grpc.pb.h"
#include "time_log.h"

namespace obiew {

using grpc::ClientContext;
using grpc::ServerContext;
using grpc::Status;
using obiew::GetFeedRequest;
using obiew::GetFeedResponse;
using obiew::GetPostRequest;
using obiew::GetPostResponse;
using obiew::GetUserRequest;
using obiew::GetUserResponse;
using obiew::LogInRequest;
using obiew::LogInResponse;
using obiew::RegisterRequest;
using obiew::RegisterResponse;
using obiew::SetPostRequest;
using obiew::SetPostResponse;
using obiew::SetUserRequest;
using obiew::SetUserResponse;

Status ObiewServiceImpl::Register(grpc::ServerContext* context,
                                  const RegisterRequest* request,
                                  RegisterResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Received RegisterRequest." << std::endl;
  }
  Status get_status = RequestFlow(*request, response);
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Returning RegisterResponse." << std::endl;
  }
  return get_status;
}

Status ObiewServiceImpl::LogIn(grpc::ServerContext* context,
                               const LogInRequest* request,
                               LogInResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Received LogInRequest." << std::endl;
  }
  Status get_status = RequestFlow(*request, response);
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Returning LogInResponse." << std::endl;
  }
  return get_status;
}

Status ObiewServiceImpl::GetUser(grpc::ServerContext* context,
                                 const GetUserRequest* request,
                                 GetUserResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Received GetUserRequest." << std::endl;
  }
  Status get_status = RequestFlow(*request, response);
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Returning GetUserResponse." << std::endl;
  }
  return get_status;
}

Status ObiewServiceImpl::GetFeed(grpc::ServerContext* context,
                                 const GetFeedRequest* request,
                                 GetFeedResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Received GetFeedRequest." << std::endl;
  }
  Status get_status = RequestFlow(*request, response);
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Returning GetFeedResponse." << std::endl;
  }
  return get_status;
}

Status ObiewServiceImpl::GetPost(grpc::ServerContext* context,
                                 const GetPostRequest* request,
                                 GetPostResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Received GetPostRequest." << std::endl;
  }
  Status get_status = RequestFlow(*request, response);
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Returning GetPostResponse." << std::endl;
  }
  return get_status;
}

Status ObiewServiceImpl::GetFollowings(grpc::ServerContext* context,
                                       const GetFollowingsRequest* request,
                                       GetFollowingsResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Received GetFollowingsRequest." << std::endl;
  }
  Status get_status = RequestFlow(*request, response);
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Returning GetFollowingsResponse." << std::endl;
  }
  return get_status;
}

Status ObiewServiceImpl::GetFollowers(grpc::ServerContext* context,
                                      const GetFollowersRequest* request,
                                      GetFollowersResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Received GetFollowersRequest." << std::endl;
  }
  Status get_status = RequestFlow(*request, response);
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Returning GetFollowersResponse." << std::endl;
  }
  return get_status;
}

Status ObiewServiceImpl::SetUser(grpc::ServerContext* context,
                                 const SetUserRequest* request,
                                 SetUserResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Received SetUserRequest." << std::endl;
  }
  Status set_status = RequestFlow(*request, response);
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Returning SetUserResponse." << std::endl;
  }
  return set_status;
}

Status ObiewServiceImpl::SetPost(grpc::ServerContext* context,
                                 const SetPostRequest* request,
                                 SetPostResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Received SetPostRequest." << std::endl;
  }
  Status set_status = RequestFlow(*request, response);
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Returning SetPostResponse." << std::endl;
  }
  return set_status;
}

Status ObiewServiceImpl::SetLike(grpc::ServerContext* context,
                                 const SetLikeRequest* request,
                                 SetLikeResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Received SetLikeRequest." << std::endl;
  }
  Status set_status = RequestFlow(*request, response);
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Returning SetLikeResponse." << std::endl;
  }
  return set_status;
}

Status ObiewServiceImpl::SetComment(grpc::ServerContext* context,
                                    const SetCommentRequest* request,
                                    SetCommentResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Received SetCommentRequest." << std::endl;
  }
  Status set_status = RequestFlow(*request, response);
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Returning SetCommentResponse." << std::endl;
  }
  return set_status;
}

Status ObiewServiceImpl::SetFollow(grpc::ServerContext* context,
                                   const SetFollowRequest* request,
                                   SetFollowResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Received SetFollowRequest." << std::endl;
  }
  Status set_status = RequestFlow(*request, response);
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << obiew_address_ << "] "
             << "Returning SetFollowResponse." << std::endl;
  }
  return set_status;
}

// Forward RegisterRequest to Coordinator.
Status ObiewServiceImpl::ForwardToCoordinator(ClientContext* cc,
                                              MultiPaxos::Stub* stub,
                                              const RegisterRequest& request,
                                              RegisterResponse* response) {
  return stub->Register(cc, request, response);
}

// Forward LogInRequest to Coordinator.
Status ObiewServiceImpl::ForwardToCoordinator(ClientContext* cc,
                                              MultiPaxos::Stub* stub,
                                              const LogInRequest& request,
                                              LogInResponse* response) {
  return stub->LogIn(cc, request, response);
}

// Forward GetUserRequest to Coordinator.
Status ObiewServiceImpl::ForwardToCoordinator(ClientContext* cc,
                                              MultiPaxos::Stub* stub,
                                              const GetUserRequest& request,
                                              GetUserResponse* response) {
  return stub->GetUser(cc, request, response);
}

// Forward GetFeedRequest to Coordinator.
Status ObiewServiceImpl::ForwardToCoordinator(ClientContext* cc,
                                              MultiPaxos::Stub* stub,
                                              const GetFeedRequest& request,
                                              GetFeedResponse* response) {
  return stub->GetFeed(cc, request, response);
}

// Forward GetPostRequest to Coordinator.
Status ObiewServiceImpl::ForwardToCoordinator(ClientContext* cc,
                                              MultiPaxos::Stub* stub,
                                              const GetPostRequest& request,
                                              GetPostResponse* response) {
  return stub->GetPost(cc, request, response);
}

// Forward GetFollowingsRequest to Coordinator.
Status ObiewServiceImpl::ForwardToCoordinator(
    ClientContext* cc, MultiPaxos::Stub* stub,
    const GetFollowingsRequest& request, GetFollowingsResponse* response) {
  return stub->GetFollowings(cc, request, response);
}

// Forward GetFollowersRequest to Coordinator.
Status ObiewServiceImpl::ForwardToCoordinator(
    ClientContext* cc, MultiPaxos::Stub* stub,
    const GetFollowersRequest& request, GetFollowersResponse* response) {
  return stub->GetFollowers(cc, request, response);
}

// Forward SetUserRequest to Coordinator.
Status ObiewServiceImpl::ForwardToCoordinator(ClientContext* cc,
                                              MultiPaxos::Stub* stub,
                                              const SetUserRequest& request,
                                              SetUserResponse* response) {
  return stub->SetUser(cc, request, response);
}

// Forward SetPostRequest to Coordinator.
Status ObiewServiceImpl::ForwardToCoordinator(ClientContext* cc,
                                              MultiPaxos::Stub* stub,
                                              const SetPostRequest& request,
                                              SetPostResponse* response) {
  return stub->SetPost(cc, request, response);
}

// Forward SetPostRequest to Coordinator.
Status ObiewServiceImpl::ForwardToCoordinator(ClientContext* cc,
                                              MultiPaxos::Stub* stub,
                                              const SetLikeRequest& request,
                                              SetLikeResponse* response) {
  return stub->SetLike(cc, request, response);
}

// Forward SetPostRequest to Coordinator.
Status ObiewServiceImpl::ForwardToCoordinator(ClientContext* cc,
                                              MultiPaxos::Stub* stub,
                                              const SetCommentRequest& request,
                                              SetCommentResponse* response) {
  return stub->SetComment(cc, request, response);
}

// Forward SetPostRequest to Coordinator.
Status ObiewServiceImpl::ForwardToCoordinator(ClientContext* cc,
                                              MultiPaxos::Stub* stub,
                                              const SetFollowRequest& request,
                                              SetFollowResponse* response) {
  return stub->SetFollow(cc, request, response);
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

}  // namespace obiew