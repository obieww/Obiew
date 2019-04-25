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

    grpc::Status GetPosts(grpc::ServerContext* context, const GetPostsRequest* request,
     GetPostsResponse* response) override;

    grpc::Status SetUser(grpc::ServerContext* context, const SetUserRequest* request,
     SetUserResponse* response) override;

    grpc::Status SetPost(grpc::ServerContext* context, const SetPostRequest* request,
     SetPostResponse* response) override;



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
    grpc::Status GetUserStatsByUserId(User* user);
    

  };

}  // namespace obiew

#endif