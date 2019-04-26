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
    MultiPaxosServiceImpl(
      PaxosStubsMap* paxos_stubs_map, 
      const std::string& my_paxos_address);
    
    grpc::Status Initialize();

  // Register a user.
    grpc::Status Register(grpc::ServerContext* context, const RegisterRequest* request,
      RegisterResponse* response) override;

  // Log In.
    grpc::Status LogIn(grpc::ServerContext* context, const LogInRequest* request,
     LogInResponse* response) override;

    grpc::Status GetUser(grpc::ServerContext* context, const GetUserRequest* request,
     GetUserResponse* response) override;

    grpc::Status GetFeed(grpc::ServerContext* context, const GetFeedRequest* request,
     GetFeedResponse* response) override;

    grpc::Status SetUser(grpc::ServerContext* context, const SetUserRequest* request,
     SetUserResponse* response) override;

    grpc::Status SetPost(grpc::ServerContext* context, const SetPostRequest* request,
     SetPostResponse* response) override;

    grpc::Status GetPost(grpc::ServerContext* context, const GetPostRequest* request,
     GetPostResponse* response) override;

    grpc::Status GetFollowers(grpc::ServerContext* context, const GetFollowersRequest* request,
     GetFollowersResponse* response) override;

    grpc::Status GetFollowings(grpc::ServerContext* context, const GetFollowingsRequest* request,
     GetFollowingsResponse* response) override;


  private:
    const std::string my_paxos_address_;
    std::mutex log_mtx_;
    PaxosStubsMap* paxos_stubs_map_;
    mysqlpp::Connection mysql_conn_;
    std::string db_name_;
    std::string mysql_server_;
    std::string mysql_user_;
    std::string mysql_password_;
    std::string pass_phrase_;
    int hash_len_;

    grpc::Status GetUserByCredentials(User* user);
    grpc::Status GetUserByUserId(User* user);
    grpc::Status DeleteUser(User* user);
    grpc::Status UpdateUser(User* user);
    grpc::Status DeletePost(Post* post);
    grpc::Status CreatePost(Post* post);
    grpc::Status GetUserStatsByUserId(User* user);
    grpc::Status GetPostsByUserId(User* user);
    grpc::Status GetOriginalPostByPostId(Post* post);
    grpc::Status GetPostByPostId(Post* post);
    grpc::Status GetPostStatByPostId(Post* post);
    grpc::Status GetFeedByUserId(User* user, GetFeedResponse* response);
    grpc::Status GetCompletePost(Post* post);
    grpc::Status GetFollowingsByUserId(User *user, GetFollowingsResponse *response);
    grpc::Status GetFollowersByUserId(User *user, GetFollowersResponse *response);

  };

}  // namespace obiew

#endif