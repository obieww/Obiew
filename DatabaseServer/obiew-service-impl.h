#ifndef OBIEW_SERVICE_IMPL_H
#define OBIEW_SERVICE_IMPL_H

#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
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
  grpc::Status Register(grpc::ServerContext* context,
                        const RegisterRequest* request,
                        RegisterResponse* response) override;

  // Log In.
  grpc::Status LogIn(grpc::ServerContext* context, const LogInRequest* request,
                     LogInResponse* response) override;

  grpc::Status GetUser(grpc::ServerContext* context,
                       const GetUserRequest* request,
                       GetUserResponse* response) override;

  grpc::Status GetFeed(grpc::ServerContext* context,
                       const GetFeedRequest* request,
                       GetFeedResponse* response) override;

  grpc::Status GetPost(grpc::ServerContext* context,
                       const GetPostRequest* request,
                       GetPostResponse* response) override;

  grpc::Status SetUser(grpc::ServerContext* context,
                       const SetUserRequest* request,
                       SetUserResponse* response) override;

  grpc::Status SetPost(grpc::ServerContext* context,
                       const SetPostRequest* request,
                       SetPostResponse* response) override;

  grpc::Status SetLike(grpc::ServerContext* context,
                       const SetLikeRequest* request,
                       SetLikeResponse* response) override;

  grpc::Status SetComment(grpc::ServerContext* context,
                          const SetCommentRequest* request,
                          SetCommentResponse* response) override;

  grpc::Status SetFollow(grpc::ServerContext* context,
                         const SetFollowRequest* request,
                         SetFollowResponse* response) override;

  grpc::Status GetFollowers(grpc::ServerContext* context,
                            const GetFollowersRequest* request,
                            GetFollowersResponse* response) override;

  grpc::Status GetFollowings(grpc::ServerContext* context,
                             const GetFollowingsRequest* request,
                             GetFollowingsResponse* response) override;

 private:
  const std::string obiew_address_;
  const std::string my_paxos_address_;
  std::mutex log_mtx_;
  PaxosStubsMap* paxos_stubs_map_;

  grpc::Status ForwardToCoordinator(grpc::ClientContext* cc,
                                    MultiPaxos::Stub* stub,
                                    const RegisterRequest& request,
                                    RegisterResponse* response);
  grpc::Status ForwardToCoordinator(grpc::ClientContext* cc,
                                    MultiPaxos::Stub* stub,
                                    const LogInRequest& request,
                                    LogInResponse* response);
  grpc::Status ForwardToCoordinator(grpc::ClientContext* cc,
                                    MultiPaxos::Stub* stub,
                                    const GetUserRequest& request,
                                    GetUserResponse* response);
  grpc::Status ForwardToCoordinator(grpc::ClientContext* cc,
                                    MultiPaxos::Stub* stub,
                                    const GetFeedRequest& request,
                                    GetFeedResponse* response);
  grpc::Status ForwardToCoordinator(grpc::ClientContext* cc,
                                    MultiPaxos::Stub* stub,
                                    const GetPostRequest& request,
                                    GetPostResponse* response);
  grpc::Status ForwardToCoordinator(grpc::ClientContext* cc,
                                    MultiPaxos::Stub* stub,
                                    const GetFollowingsRequest& request,
                                    GetFollowingsResponse* response);
  grpc::Status ForwardToCoordinator(grpc::ClientContext* cc,
                                    MultiPaxos::Stub* stub,
                                    const GetFollowersRequest& request,
                                    GetFollowersResponse* response);
  grpc::Status ForwardToCoordinator(grpc::ClientContext* cc,
                                    MultiPaxos::Stub* stub,
                                    const SetUserRequest& request,
                                    SetUserResponse* response);
  grpc::Status ForwardToCoordinator(grpc::ClientContext* cc,
                                    MultiPaxos::Stub* stub,
                                    const SetPostRequest& request,
                                    SetPostResponse* response);
  grpc::Status ForwardToCoordinator(grpc::ClientContext* cc,
                                    MultiPaxos::Stub* stub,
                                    const SetLikeRequest& request,
                                    SetLikeResponse* response);
  grpc::Status ForwardToCoordinator(grpc::ClientContext* cc,
                                    MultiPaxos::Stub* stub,
                                    const SetCommentRequest& request,
                                    SetCommentResponse* response);
  grpc::Status ForwardToCoordinator(grpc::ClientContext* cc,
                                    MultiPaxos::Stub* stub,
                                    const SetFollowRequest& request,
                                    SetFollowResponse* response);
  template <typename Request, typename Response>
  grpc::Status RequestFlow(const Request& request, Response* response);
};

}  // namespace obiew

#endif