#include <cstdlib>

#include "multi-paxos-service-impl.h"
#include "time_log.h"


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
  using obiew::Post;
  using obiew::OperationType;

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
      std::string stmt = "INSERT INTO Users (Name, Password, ProfilePicture, Email, Phone) VALUES (%0q:username, AES_ENCRYPT(%1q:password, UNHEX(SHA2('My secret passphrase',512))), %2q:profile, %3q:email, %4q:phone);";
      mysqlpp::Query query = mysql_conn_.query(stmt);
      query.parse();
      auto result = query.execute(user.name(), user.password(),user.profile_picture(), user.email(), user.phone());
      if(result.rows() > 0) {
        auto qurery_status = GetUserByCredentials(&user);
        if(qurery_status.ok()) {
          *response->mutable_user() = user;   
          TIME_LOG << response->user().DebugString();     
        } else {
          return Status(qurery_status.error_code(), qurery_status.error_message());
        }
      } else {
        return Status(grpc::StatusCode::ABORTED, "Mysql INSERT Failed.");
      }
    } else {
      return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
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
      auto qurery_status = GetUserByCredentials(&user);
      if(qurery_status.ok()) {
        *response->mutable_user() = user;   
        TIME_LOG << response->user().DebugString();     
      } else {
        return Status(qurery_status.error_code(), qurery_status.error_message());
      }
    } else {
      return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
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
      << request->user().DebugString()
      << std::endl;
    }
    User user = request->user();
    if (mysql_conn_.connect(db_name_.c_str(), mysql_server_.c_str(), mysql_user_.c_str(), mysql_password_.c_str())) {
      auto get_user_status = GetUserByUserId(&user);
      auto get_stats_status = GetUserStatsByUserId(&user);
      auto get_posts_status = GetPostsByUserId(&user);
      if(get_user_status.ok() && get_stats_status.ok() && get_posts_status.ok()) {
        *response->mutable_user()=user;
        TIME_LOG << response->user().DebugString();
      } else {
        return Status(grpc::StatusCode::ABORTED, get_user_status.error_message() + get_stats_status.error_message() + get_posts_status.error_message());
      }
    } else {
      return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
    }
    {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
      << "Returning GetUserResponse." << std::endl;
    }
    return Status::OK;
  }

  Status MultiPaxosServiceImpl::GetFeed(grpc::ServerContext* context, const GetFeedRequest* request,
    GetFeedResponse* response) {
    {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
      << "Received GetFeedRequest."
      << std::endl;
    }
    User user = request->user();
    if (mysql_conn_.connect(db_name_.c_str(), mysql_server_.c_str(), mysql_user_.c_str(), mysql_password_.c_str())) {
      auto get_feed_status = GetFeedByUserId(&user, response);
      if(get_feed_status.ok()) {
        TIME_LOG << response->DebugString();
      } else {
        return Status(grpc::StatusCode::ABORTED, get_feed_status.error_message());
      }
    } else {
      return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
    }
    {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
      << "Returning GetFeedResponse." << std::endl;
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
    User user = request->user();
    if (mysql_conn_.connect(db_name_.c_str(), mysql_server_.c_str(), mysql_user_.c_str(), mysql_password_.c_str())) {
      Status set_user_status;
      if (request->operation() == OperationType::UPDATE) {
        set_user_status = UpdateUser(&user);
      } else if (request->operation() == OperationType::DELETE) {
        set_user_status = DeleteUser(&user);
      } else {
        return Status(grpc::StatusCode::ABORTED, "Operation not supported.");
      }
      if(set_user_status.ok()) {
        *response->mutable_user()=user;
        TIME_LOG << response->user().DebugString();
      } else {
        return Status(set_user_status.error_code(), set_user_status.error_message());
      }
    } else {
      return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
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
    Post post = request->post();
    if (mysql_conn_.connect(db_name_.c_str(), mysql_server_.c_str(), mysql_user_.c_str(), mysql_password_.c_str())) {
      Status set_post_status;
      if (request->operation() == OperationType::CREATE) {
        set_post_status = CreatePost(&post);
      } else if (request->operation() == OperationType::DELETE) {
        set_post_status = DeletePost(&post);
      } else {
        return Status(grpc::StatusCode::ABORTED, "Operation not supported.");
      }
      if(set_post_status.ok()) {
        *response->mutable_post()=post;
        TIME_LOG << response->post().DebugString();
      } else {
        return Status(set_post_status.error_code(), set_post_status.error_message());
      }
    } else {
      return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
    }
    {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
      << "Returning SetPostResponse." << std::endl;
    }
    return Status::OK;
  }

  Status MultiPaxosServiceImpl::GetFeedByUserId(User* user, GetFeedResponse* response) {
    std::string stmt = "SELECT * FROM Posts WHERE UserId IN (SELECT FolloweeId FROM Follows WHERE UserId=%0q:userid) ORDER BY Created DESC";
    mysqlpp::Query query = mysql_conn_.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set = query.store(user->user_id());
    if(result_set.size() > 0) {
      for (auto it = result_set.begin(); it != result_set.end(); ++it) {
        auto* post = response->add_post();
        mysqlpp::Row row = *it;
        post->set_post_id(row["PostId"]);
        post->set_user_id(row["UserId"]);
        post->set_content(row["Content"]);
        post->set_image_id(row["ImageId"]);
        post->set_created(row["Created"]);
        post->set_direct_repost_id((row["RepostId"]==mysqlpp::null) ? 0 : row["RepostId"]);
        Post original_post;
        original_post.set_post_id((row["OriginalPostId"]==mysqlpp::null) ? 0 : row["OriginalPostId"]);
        if (original_post.post_id() != 0) {
          auto get_origin_status = GetOriginalPostByPostId(&original_post);
          if (!get_origin_status.ok()) {
            return Status(get_origin_status.error_code(), get_origin_status.error_message());
          }
        }
        *post->mutable_original_post() = original_post;
        auto get_stats_status = GetPostStatByPostId(post);
        if (!get_stats_status.ok()) {
          return Status(get_stats_status.error_code(), get_stats_status.error_message());
        }
      }
    }
    return Status::OK;
  }

  Status MultiPaxosServiceImpl::DeleteUser(User* user) {
    std::string stmt = "DELETE FROM Users WHERE UserId=%0q:userid";
    mysqlpp::Query query = mysql_conn_.query(stmt);
    query.parse();
    auto result = query.execute(user->user_id());
    if(result.rows() > 0) {
      auto get_user_status = GetUserByUserId(user);
      if(get_user_status.ok()) { // Shouldn't exist.
        return Status(grpc::StatusCode::ABORTED, "DELETE User Failed.");
      }
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql UPDATE Failed.");
    }
    return Status::OK;
  }

  Status MultiPaxosServiceImpl::UpdateUser(User* user) {
    std::string stmt = "UPDATE Users SET Name=%1q:name, Password=AES_ENCRYPT(%2q:password, UNHEX(SHA2(%5q:passphrase,%6:hashlen))), Email=%3q:email,Phone=%4q:phone WHERE UserId=%0q:userid";
    mysqlpp::Query query = mysql_conn_.query(stmt);
    query.parse();
    auto result = query.execute(user->user_id(),user->name(),user->password(), user->email(), user->phone(), pass_phrase_, hash_len_);
    if(result.rows() > 0) {
      auto get_user_status = GetUserByUserId(user);
      if(!get_user_status.ok()) {
        return Status(get_user_status.error_code(), get_user_status.error_message());
      }
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql UPDATE Failed.");
    }
    return Status::OK;
  }

  Status MultiPaxosServiceImpl::DeletePost(Post* post) {
    std::string stmt = "DELETE FROM Posts WHERE PostId=%0q:postid";
    mysqlpp::Query query = mysql_conn_.query(stmt);
    query.parse();
    auto result = query.execute(post->post_id());
    if(result.rows() > 0) {
      auto get_post_status = GetPostByPostId(post);
      if(get_post_status.ok()) { // Shouldn't exist.
        return Status(grpc::StatusCode::ABORTED, "DELETE Post Failed.");
      }
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql DELETE Failed.");
    }
    return Status::OK;
  }

  Status MultiPaxosServiceImpl::CreatePost(Post* post) {
    std::string stmt = "INSERT INTO Posts (UserId, Content, RepostId) VALUES (%0q:userid, %1q:content, %2q:repostid)";
    mysqlpp::Query query = mysql_conn_.query(stmt);
    query.parse();
    TIME_LOG << post->DebugString();
    mysqlpp::SimpleResult result;
    if (post->direct_repost_id() != 0) {
      result = query.execute(post->user_id(),post->content(),post->direct_repost_id());
    } else {
      result = query.execute(post->user_id(),post->content(),mysqlpp::null);
    }
    if(result.rows() > 0) {
      std::string stmt = "SELECT LAST_INSERT_ID() AS PostId";
      mysqlpp::Query query = mysql_conn_.query(stmt);
      query.parse();
      mysqlpp::StoreQueryResult result_set = query.store();
      if(result_set.size() == 1) {
        auto it = result_set.begin();
        mysqlpp::Row row = *it;
        post->set_post_id(row["PostId"]);
      } else {
        return Status(grpc::StatusCode::NOT_FOUND, "Mysql SELECT Failed.");
      }
      auto get_post_status = GetPostByPostId(post);
      if(!get_post_status.ok()) {
        return Status(get_post_status.error_code(), get_post_status.error_message());
      }
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql UPDATE Failed.");
    }
    return Status::OK;
  }

  Status MultiPaxosServiceImpl::GetUserByCredentials(User* user) {
    std::string stmt = "SELECT UserId,Name,ProfilePicture,Email,Phone FROM Users WHERE Name=%0q:username AND Password=AES_ENCRYPT(%1q:password, UNHEX(SHA2(%2q:passphrase,%3:hashlen)))";
    mysqlpp::Query query = mysql_conn_.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set = query.store(user->name(), user->password(), pass_phrase_, hash_len_);
    if(result_set.size() == 1) {
      auto it = result_set.begin();
      mysqlpp::Row row = *it;
      user->set_user_id(row["UserId"]);
      user->set_name(row["Name"]);
      user->set_profile_picture(row["ProfilePicture"]);
      user->set_email(row["Email"]);
      user->set_phone(row["Phone"]);
      user->clear_password();
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql SELECT Failed.");
    }
    return Status::OK;
  }

  Status MultiPaxosServiceImpl::GetUserByUserId(User* user) {
    std::string stmt = "SELECT UserId,Name,ProfilePicture,Email,Phone FROM Users WHERE UserId=%0q:userid";
    mysqlpp::Query query = mysql_conn_.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set = query.store(user->user_id());
    if(result_set.size() == 1) {
      auto it = result_set.begin();
      mysqlpp::Row row = *it;
      user->set_user_id(row["UserId"]);
      user->set_name(row["Name"]);
      user->set_profile_picture(row["ProfilePicture"]);
      user->set_email(row["Email"]);
      user->set_phone(row["Phone"]);
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql SELECT Failed.");
    }
    return Status::OK;
  }

  Status MultiPaxosServiceImpl::GetUserStatsByUserId(User* user) {
    std::string stmt = "SELECT Posts,Following,Followers FROM UserStats WHERE UserId=%0q:userid";
    mysqlpp::Query query = mysql_conn_.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set = query.store(user->user_id());
    if(result_set.size() == 1) {
      auto it = result_set.begin();
      mysqlpp::Row row = *it;
      user->set_num_posts(row["Posts"]);
      user->set_num_following(row["Following"]);
      user->set_num_followers(row["Followers"]);
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql SELECT Failed.");
    }
    return Status::OK;
  }

  Status MultiPaxosServiceImpl::GetPostsByUserId(User* user) {
    std::string stmt = "SELECT PostId,UserId,Content,ImageId,Created,RepostId,OriginalPostId FROM Posts WHERE UserId=%0q:userid LIMIT 10";
    mysqlpp::Query query = mysql_conn_.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set = query.store(user->user_id());
    if(result_set.size() > 0) {
      for (auto it = result_set.begin(); it != result_set.end(); ++it) {
        auto* post = user->add_recent_posts();
        mysqlpp::Row row = *it;
        post->set_post_id(row["PostId"]);
        post->set_user_id(row["UserId"]);
        post->set_content(row["Content"]);
        post->set_image_id(row["ImageId"]);
        post->set_created(row["Created"]);
        post->set_direct_repost_id((row["RepostId"]==mysqlpp::null) ? 0 : row["RepostId"]);
        Post original_post;
        original_post.set_post_id((row["OriginalPostId"]==mysqlpp::null) ? 0 : row["OriginalPostId"]);
        if (original_post.post_id() != 0) {
          auto get_origin_status = GetOriginalPostByPostId(&original_post);
          if (!get_origin_status.ok()) {
            return Status(get_origin_status.error_code(), get_origin_status.error_message());
          }
        }
        *post->mutable_original_post() = original_post;
        auto get_stats_status = GetPostStatByPostId(post);
        if (!get_stats_status.ok()) {
          return Status(get_stats_status.error_code(), get_stats_status.error_message());
        }
      }
    }
    return Status::OK;
  }

  Status MultiPaxosServiceImpl::GetOriginalPostByPostId(Post* post) {
    std::string stmt = "SELECT PostId,UserId,Content,ImageId,Created,RepostId,OriginalPostId FROM Posts WHERE PostId=%0q:postid";
    mysqlpp::Query query = mysql_conn_.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set = query.store(post->post_id());
    if(result_set.size() == 1) {
      auto it = result_set.begin();
      mysqlpp::Row row = *it;
      post->set_post_id(row["PostId"]);
      post->set_user_id(row["UserId"]);
      post->set_content(row["Content"]);
      post->set_image_id(row["ImageId"]);
      post->set_created(row["Created"]);
      post->set_direct_repost_id((row["RepostId"]==mysqlpp::null) ? 0 : row["RepostId"]);
      auto get_stats_status = GetPostStatByPostId(post);
      if (!get_stats_status.ok()) {
        return Status(get_stats_status.error_code(), get_stats_status.error_message());
      }
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql SELECT Failed.");
    }
    return Status::OK;
  }

  Status MultiPaxosServiceImpl::GetPostByPostId(Post* post) {
    std::string stmt = "SELECT PostId,UserId,Content,ImageId,Created,RepostId,OriginalPostId FROM Posts WHERE PostId=%0q:postid";
    mysqlpp::Query query = mysql_conn_.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set = query.store(post->post_id());
    if(result_set.size() == 1) {
      auto it = result_set.begin();
      mysqlpp::Row row = *it;
      post->set_post_id(row["PostId"]);
      post->set_user_id(row["UserId"]);
      post->set_content(row["Content"]);
      post->set_image_id(row["ImageId"]);
      post->set_created(row["Created"]);
      post->set_direct_repost_id((row["RepostId"]==mysqlpp::null) ? 0 : row["RepostId"]);
      Post original_post;
      original_post.set_post_id((row["OriginalPostId"]==mysqlpp::null) ? 0 : row["OriginalPostId"]);
      if (original_post.post_id() != 0) {
        auto get_origin_status = GetOriginalPostByPostId(&original_post);
        if (!get_origin_status.ok()) {
          return Status(get_origin_status.error_code(), get_origin_status.error_message());
        }
      }
      *post->mutable_original_post() = original_post;
      auto get_stats_status = GetPostStatByPostId(post);
      if (!get_stats_status.ok()) {
        return Status(get_stats_status.error_code(), get_stats_status.error_message());
      }
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql SELECT Failed.");
    }
    return Status::OK;
  }

  Status MultiPaxosServiceImpl::GetPostStatByPostId(Post* post) {
    std::string stmt = "SELECT Reposts,Comments,Likes FROM PostStats WHERE PostId=%0q:postid";
    mysqlpp::Query query = mysql_conn_.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set = query.store(post->post_id());
    if(result_set.size() == 1) {
      auto it = result_set.begin();
      mysqlpp::Row row = *it;
      post->set_num_reposts(row["Reposts"]);
      post->set_num_comments(row["Comments"]);
      post->set_num_likes(row["Likes"]);
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql SELECT Failed.");
    }
    return Status::OK;
  }

}  // namespace obiew
