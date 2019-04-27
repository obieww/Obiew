#ifndef MULTI_PAXOS_SERVICE_IMPL_H
#define MULTI_PAXOS_SERVICE_IMPL_H

#include <string>
#include <vector>

#include <grpcpp/grpcpp.h>
#include <mysql++.h>
#include <mysql.h>

#include "obiew.grpc.pb.h"
#include "paxos-stubs-map.h"
#include "time_log.h"

namespace obiew {

class MultiPaxosServiceImpl final : public MultiPaxos::Service {
 public:
  // Construction method.
  MultiPaxosServiceImpl(PaxosStubsMap* paxos_stubs_map,
                        const std::string& my_paxos_address);

  grpc::Status Initialize();

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

  grpc::Status GetPost(grpc::ServerContext* context,
                       const GetPostRequest* request,
                       GetPostResponse* response) override;

  grpc::Status GetFollowers(grpc::ServerContext* context,
                            const GetFollowersRequest* request,
                            GetFollowersResponse* response) override;

  grpc::Status GetFollowings(grpc::ServerContext* context,
                             const GetFollowingsRequest* request,
                             GetFollowingsResponse* response) override;
  // Update coordinator when old coordinator is unavailable.
  grpc::Status ElectCoordinator(grpc::ServerContext* context,
                                const ElectCoordinatorRequest* request,
                                EmptyMessage* response) override;
  // Get the current coordinator.
  grpc::Status GetCoordinator(grpc::ServerContext* context,
                              const EmptyMessage* request,
                              GetCoordinatorResponse* response) override;

  // Paxos phase 1. Coordinator -> Acceptor.
  grpc::Status Prepare(grpc::ServerContext* context,
                       const PrepareRequest* request,
                       PromiseResponse* response) override;
  // Paxos phase 2. Coordinator -> Acceptor.
  grpc::Status Propose(grpc::ServerContext* context,
                       const ProposeRequest* request,
                       AcceptResponse* response) override;
  // Paxos phase 3. Coordinator -> Learner.
  grpc::Status Inform(grpc::ServerContext* context,
                      const InformRequest* request,
                      InformResponse* response) override;

  // Test if the server is available.
  grpc::Status Ping(grpc::ServerContext* context, const EmptyMessage* request,
                    EmptyMessage* response) override;

  // After brought up again, a server will catch up with others' logs.
  grpc::Status Recover(grpc::ServerContext* context,
                       const RecoverRequest* request,
                       RecoverResponse* response) override;

 private:
  const std::string my_paxos_address_;
  std::mutex log_mtx_;
  PaxosStubsMap* paxos_stubs_map_;
  std::string db_name_;
  std::string mysql_server_;
  std::string mysql_user_;
  std::string mysql_password_;
  std::string pass_phrase_;
  int hash_len_;
  int fail_rate_;

  grpc::Status GetUserByCredentials(User* user);
  grpc::Status GetUserByUserId(User* user);
  grpc::Status DeleteUser(User* user);
  grpc::Status UpdateUser(User* user);
  grpc::Status DeletePost(Post* post);
  grpc::Status CreatePost(Post* post);
  grpc::Status DeleteLike(Like* like);
  grpc::Status CreateLike(Like* like);
  grpc::Status DeleteComment(Comment* comment);
  grpc::Status CreateComment(Comment* comment);
  grpc::Status DeleteFollow(SetFollowResponse* response);
  grpc::Status CreateFollow(SetFollowResponse* response);
  grpc::Status GetUserStatsByUserId(User* user);
  grpc::Status GetPostsByUserId(User* user);
  grpc::Status GetOriginalPostByPostId(Post* post);
  grpc::Status GetPostByPostId(Post* post);
  grpc::Status GetLikeByLikeId(Like* like);
  grpc::Status GetCommentByCommentId(Comment* comment);
  grpc::Status GetFollowByIds(SetFollowResponse* response);
  grpc::Status GetPostStatByPostId(Post* post);
  grpc::Status GetFeedByUserId(User* user, GetFeedResponse* response);
  grpc::Status GetCompletePost(Post* post);
  grpc::Status GetFollowingsByUserId(User* user,
                                     GetFollowingsResponse* response);
  grpc::Status GetFollowersByUserId(User* user, GetFollowersResponse* response);
  grpc::Status GetCoordinator();
  grpc::Status ElectNewCoordinator();
  grpc::Status GetRecovery();
  grpc::Status Register(Proposal* proposal);
  grpc::Status SetUser(Proposal* proposal);
  grpc::Status SetPost(Proposal* proposal);
  grpc::Status SetLike(Proposal* proposal);
  grpc::Status SetComment(Proposal* proposal);
  grpc::Status SetFollow(Proposal* proposal);
  bool RandomFail();
  grpc::Status RunPaxos(Proposal* proposal);

  PaxosLog GetPaxosLog(ProposalType type, int round);
  // Returns the latest Paxos round number for the given type.
  int GetLatestRound(ProposalType type);
  // Add PaxosLog when Acceptor receives a proposal.
  void AddPaxosLog(ProposalType type, int round);
  // Add PaxosLog when Acceptor promises a proposal.
  void AddPaxosLog(ProposalType type, int round, int promised_id);
  // Add PaxosLog when Acceptor accepts a proposal.
  void AddPaxosLog(ProposalType type, int round, int accepted_id,
                   Proposal proposal);
  // Add PaxosLog from recovery snapshot.
  void AddPaxosLog(ProposalType type, int round, int promised_id,
                   int accepted_id, Proposal proposal);
};

}  // namespace obiew

#endif