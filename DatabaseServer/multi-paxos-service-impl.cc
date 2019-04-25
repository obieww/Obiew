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
  using obiew::GetUserRequest;
  using obiew::GetUserResponse;
  using obiew::GetPostsRequest;
  using obiew::GetPostsResponse;
  using obiew::SetUserRequest;
  using obiew::SetUserResponse;
  using obiew::SetPostRequest;
  using obiew::SetPostResponse;
  using obiew::User;

    MultiPaxosServiceImpl::MultiPaxosServiceImpl(
      PaxosStubsMap* paxos_stubs_map, 
      const std::string& my_paxos_address)
    : paxos_stubs_map_(paxos_stubs_map),
    my_paxos_address_(my_paxos_address) {
      mysql_conn_ = mysqlpp::Connection(false);
          db_name_ = "Obiew";
    mysql_server_ = "localhost:3306";
    mysql_user_ = "root";
    mysql_password_ = "password";
    pass_phrase_ = "My secret passphrase";
    hash_len_ = 512;
    }

  // Find Coordinator and recover data from Coordinator on construction.
  Status MultiPaxosServiceImpl::Initialize() {
  // // Try to get Coordinator address from other replicas.
  //   Status Status::OK = GetCoordinator();
  //   auto* coordinator_stub = paxos_stubs_map_->GetCoordinatorStub();
  // // If not successful, start an election for Coordinators.
  //   if (!Status::OK.ok()) {
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
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
      << "[Coordinator] Received RegisterRequest."
      << std::endl;
    }
    User user = request->user();
    *response->mutable_user() = user;
    if (mysql_conn_.connect(db_name_.c_str(), mysql_server_.c_str(), mysql_user_.c_str(), mysql_password_.c_str())) {
      std::string s = "INSERT INTO Users (Name, Password, Email, Phone) VALUES (%0q:username, AES_ENCRYPT(%1q:password, UNHEX(SHA2('My secret passphrase',512))), %2q:email, %3q:phone);";
      mysqlpp::Query query = mysql_conn_.query(s);
      query.parse();
      // mysqlpp::StoreQueryResult result_set = query.store(user.name(), user.password(), user.email(), user.phone());
      auto result = query.execute(user.name(), user.password(), user.email(), user.phone());
      if(result.rows() > 0) {
        user.clear_password();
        *response->mutable_user()=user;
      } else {
        return Status(grpc::StatusCode::ABORTED, "Mysql query Failed.");
      }
    } else {
      return Status(grpc::StatusCode::ABORTED, "Mysql connection Failed.");
    }
    {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
      << "[Coordinator] Returning RegisterResponse." << std::endl;
    }
    return Status::OK;
  }

  Status MultiPaxosServiceImpl::LogIn(grpc::ServerContext* context, const LogInRequest* request,
    LogInResponse* response) {
    {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
      << "[Coordinator] Received LogInRequest."
      << std::endl;
    }
    User user = request->user();
    if (mysql_conn_.connect(db_name_.c_str(), mysql_server_.c_str(), mysql_user_.c_str(), mysql_password_.c_str())) {
      std::string s = "SELECT UserId,Name,Email,Phone FROM Users WHERE Name=%0q:username AND Password=AES_ENCRYPT(%1q:password, UNHEX(SHA2(%2q:passphrase,%3:hashlen)))";
      mysqlpp::Query query = mysql_conn_.query(s);
      query.parse();
      mysqlpp::StoreQueryResult result_set = query.store(user.name(), user.password(), pass_phrase_, hash_len_);
      if(result_set.size() == 1) {
        auto it = result_set.begin();
        mysqlpp::Row row = *it;
        user.set_user_id(row["UserId"]);
        user.set_email(row["Email"]);
        user.set_phone(row["Phone"]);
        user.clear_password();
        *response->mutable_user()=user;
        std::cout << user.DebugString() << std::endl;
      } else {
        return Status(grpc::StatusCode::NOT_FOUND,
                  "Mysql query Failed.");
      }
    } else {
      return Status(grpc::StatusCode::ABORTED,
                  "Mysql connection Failed.");
    }
    {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
      << "[Coordinator] Returning LogInResponse." << std::endl;
    }
    return Status::OK;
  }

    Status MultiPaxosServiceImpl::GetUser(grpc::ServerContext* context, const GetUserRequest* request,
    GetUserResponse* response) {
    {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
      << "Received GetUserRequest."
      << std::endl;
    }
    
    {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
      << "Returning GetUserResponse." << std::endl;
    }
    return Status::OK;
  }

  Status MultiPaxosServiceImpl::GetPosts(grpc::ServerContext* context, const GetPostsRequest* request,
    GetPostsResponse* response) {
    {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
      << "Received GetPostsRequest."
      << std::endl;
    }
    
    {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
      << "Returning GetPostsResponse." << std::endl;
    }
    return Status::OK;
  }

  Status MultiPaxosServiceImpl::SetUser(grpc::ServerContext* context, const SetUserRequest* request,
    SetUserResponse* response) {
    {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
      << "Received SetUserRequest."
      << std::endl;
    }
    
    {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
      << "Returning SetUserResponse." << std::endl;
    }
    return Status::OK;
  }

  Status MultiPaxosServiceImpl::SetPost(grpc::ServerContext* context, const SetPostRequest* request,
    SetPostResponse* response) {
    {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
      << "Received SetPostRequest."
      << std::endl;
    }
    
    {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
      << "Returning SetPostResponse." << std::endl;
    }
    return Status::OK;
  }

}  // namespace obiew
