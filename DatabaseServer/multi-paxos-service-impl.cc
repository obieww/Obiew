#include <cstdlib>

#include "multi-paxos-service-impl.h"
#include "time_log.h"

namespace obiew {

using grpc::ClientContext;
using grpc::ServerContext;
using grpc::Status;
// using obiew::Comment;
// using obiew::GetPostRequest;
// using obiew::GetPostResponse;
// using obiew::GetUserRequest;
// using obiew::GetUserResponse;
// using obiew::Like;
// using obiew::LogInRequest;
// using obiew::LogInResponse;
// using obiew::OperationType;
// using obiew::Post;
// using obiew::RegisterRequest;
// using obiew::RegisterResponse;
// using obiew::Repost;
// using obiew::SetPostRequest;
// using obiew::SetPostResponse;
// using obiew::SetUserRequest;
// using obiew::SetUserResponse;
// using obiew::User;

MultiPaxosServiceImpl::MultiPaxosServiceImpl(
    PaxosStubsMap* paxos_stubs_map, const std::string& my_paxos_address)
    : paxos_stubs_map_(paxos_stubs_map), my_paxos_address_(my_paxos_address) {
  db_name_ = "Obiew";
  mysql_server_ = "localhost:3306";
  mysql_user_ = "root";
  mysql_password_ = "password";
  pass_phrase_ = "My secret passphrase";
  hash_len_ = 512;
  fail_rate_ = 0;
}

// Find Coordinator and recover data from Coordinator on construction.
Status MultiPaxosServiceImpl::Initialize() {
  // Try to get Coordinator address from other replicas.
  Status get_status = GetCoordinator();
  auto* coordinator_stub = paxos_stubs_map_->GetCoordinatorStub();
  // If not successful, start an election for Coordinators.
  if (!get_status.ok()) {
    Status elect_status = ElectNewCoordinator();
    if (!elect_status.ok()) {
      return Status(
          grpc::StatusCode::ABORTED,
          "ElectNewCoordinator Failed: " + elect_status.error_message());
    }
  }
  Status recover_status = GetRecovery();
  if (!recover_status.ok()) {
    return Status(grpc::StatusCode::ABORTED,
                  "GetRecovery Failed: " + recover_status.error_message());
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::ElectCoordinator(
    grpc::ServerContext* context, const ElectCoordinatorRequest* request,
    EmptyMessage* response) {
  if (context->IsCancelled()) {
    return Status(grpc::StatusCode::CANCELLED,
                  "Deadline exceeded or Client cancelled, abandoning.");
  }
  TIME_LOG << "[" << my_paxos_address_ << "] "
           << "Received ElectCoordinator Request: [coordinator: "
           << request->coordinator() << "]." << std::endl;
  Proposal proposal;
  proposal.set_type(ProposalType::COORDINATOR);
  proposal.set_coordinator(request->coordinator());
  // Run a Paxos instance to reach consensus on the operation.
  Status set_status = RunPaxos(&proposal);
  TIME_LOG << "[" << my_paxos_address_ << "] "
           << "Returning Response to Request: ElectCoordinator [coordinator: "
           << request->coordinator() << "]." << std::endl;

  return set_status;
}

Status MultiPaxosServiceImpl::GetCoordinator(grpc::ServerContext* context,
                                             const EmptyMessage* request,
                                             GetCoordinatorResponse* response) {
  if (context->IsCancelled()) {
    return Status(grpc::StatusCode::CANCELLED,
                  "Deadline exceeded or Client cancelled, abandoning.");
  }
  TIME_LOG << "[" << my_paxos_address_ << "] "
           << "Received GetCoordinator Request." << std::endl;

  assert(paxos_stubs_map_ != nullptr);
  std::string coordinator = paxos_stubs_map_->GetCoordinator();
  if (coordinator.empty()) {
    return Status(grpc::StatusCode::NOT_FOUND, "Coordinator not found.");
  }
  response->set_coordinator(coordinator);
  TIME_LOG << "[" << my_paxos_address_ << "] "
           << "Returning GetCoordinatorResponse: [coordinator: "
           << response->coordinator() << "]." << std::endl;

  return Status::OK;
}

Status MultiPaxosServiceImpl::Ping(grpc::ServerContext* context,
                                   const EmptyMessage* request,
                                   EmptyMessage* response) {
  // {
  //   std::unique_lock<std::mutex> writer_lock(log_mtx_);
  //   TIME_LOG << "[" << my_paxos_address_ << "] "
  //            << "Received Ping Request. Returning Response." << std::endl;
  // }
  return Status::OK;
}

// Logic upon receiving a Prepare message.
// Role: Acceptor
Status MultiPaxosServiceImpl::Prepare(grpc::ServerContext* context,
                                      const PrepareRequest* request,
                                      PromiseResponse* response) {
  if (context->IsCancelled()) {
    return Status(grpc::StatusCode::CANCELLED,
                  "Deadline exceeded or Client cancelled, abandoning.");
  }
  ProposalType type = request->type();
  int round = request->round();
  int propose_id = request->propose_id();
  // Update round id in db.
  AddPaxosLog(type, round);
  response->set_round(round);
  response->set_propose_id(propose_id);
  PaxosLog paxos_log = GetPaxosLog(type, round);
  std::stringstream promise_msg;
  promise_msg << "[Promised] [type: " << type << ", round: " << round
              << ", propose_id: " << propose_id;
  // Will NOT accept PrepareRequests with propose_id <= promised_id.
  if (paxos_log.promised_id() >= propose_id) {
    return Status(grpc::StatusCode::ABORTED,
                  "Aborted. Proposal ID is too low.");
  } else if (type != ProposalType::COORDINATOR && RandomFail()) {
    std::stringstream fail_msg;
    fail_msg << "[Rejected] Acceptor random-failed on Prepare[type: " << type
             << ", round: " << round << ", propose_id: " << propose_id << ")";
    {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] " << fail_msg.str()
               << std::endl;
    }
    return Status(grpc::StatusCode::ABORTED, fail_msg.str());
  } else if (paxos_log.accepted_id() > 0) {
    // Piggyback accepted proposal information in response.
    response->set_accepted_id(paxos_log.accepted_id());
    *response->mutable_proposal() = paxos_log.proposal();
    promise_msg << ", accepted_id: " << paxos_log.accepted_id()
                << ", type: " << paxos_log.proposal().type();
  }
  promise_msg << "].";
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] " << promise_msg.str()
             << std::endl;
  }
  return Status::OK;
}

// Logic upon receiving a Propose message.
// Role: Acceptor
Status MultiPaxosServiceImpl::Propose(grpc::ServerContext* context,
                                      const ProposeRequest* request,
                                      AcceptResponse* response) {
  if (context->IsCancelled()) {
    return Status(grpc::StatusCode::CANCELLED,
                  "Deadline exceeded or Client cancelled, abandoning.");
  }
  ProposalType type = request->proposal().type();
  int round = request->round();
  int propose_id = request->propose_id();
  PaxosLog paxos_log = GetPaxosLog(type, round);
  std::stringstream accept_msg;
  // Will NOT accept ProposeRequests with propose_id < promised_id.
  if (paxos_log.promised_id() > propose_id) {
    return Status(grpc::StatusCode::ABORTED,
                  "Aborted. Proposal ID is too low.");
  } else if (type != ProposalType::COORDINATOR && RandomFail()) {
    std::stringstream fail_msg;
    fail_msg << "[Rejected] Acceptor random-failed on Propose[type: " << type
             << ", round: " << round << ", propose_id: " << propose_id << ")";
    {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] " << fail_msg.str()
               << std::endl;
    }
    return Status(grpc::StatusCode::ABORTED, fail_msg.str());
  } else {
    // Respond with acceptance and update accepted proposal in db.
    Proposal proposal = request->proposal();
    response->set_round(round);
    response->set_propose_id(propose_id);
    *response->mutable_proposal() = proposal;
    AddPaxosLog(type, round, propose_id, proposal);
    accept_msg << "[Accepted] [type: " << type << ", round: " << round
               << ", propose_id: " << propose_id << "].";
  }
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] " << accept_msg.str()
             << std::endl;
  }
  return Status::OK;
}

// Logic upon receiving an Inform message.
// Role: Learner
Status MultiPaxosServiceImpl::Inform(grpc::ServerContext* context,
                                     const InformRequest* request,
                                     InformResponse* response) {
  if (context->IsCancelled()) {
    return Status(grpc::StatusCode::CANCELLED,
                  "Deadline exceeded or Client cancelled, abandoning.");
  }
  const auto& acceptance = request->acceptance();
  auto proposal = acceptance.proposal();
  *response->mutable_proposal() = proposal;
  // Update Paxos log in db.
  AddPaxosLog(proposal.type(), acceptance.round(), acceptance.propose_id(),
              proposal);
  // Will NOT execute operation if it's not the latest round.
  if (acceptance.round() < GetLatestRound(proposal.type())) {
    return Status(grpc::StatusCode::ABORTED,
                  "Aborted. Operation overwritten by others.");
  }
  // Execute operation.
  if (proposal.type() == ProposalType::COORDINATOR) {
    paxos_stubs_map_->SetCoordinator(proposal.coordinator());
    {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
               << "[Success] Set Coordinator to [" << proposal.coordinator()
               << "]." << std::endl;
    }
  } else if (proposal.type() == ProposalType::SQL) {
    if (proposal.has_register_request()) {
      return Register(response->mutable_proposal());
    } else if (proposal.has_set_user_request()) {
      return SetUser(response->mutable_proposal());
    } else if (proposal.has_set_post_request()) {
      return SetPost(response->mutable_proposal());
    } else if (proposal.has_set_comment_request()) {
      return SetComment(response->mutable_proposal());
    } else if (proposal.has_set_like_request()) {
      return SetLike(response->mutable_proposal());
    } else if (proposal.has_set_follow_request()) {
      return SetFollow(response->mutable_proposal());
    }
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::Recover(grpc::ServerContext* context,
                                      const RecoverRequest* request,
                                      RecoverResponse* response) {
  auto sql_paxos_logs = GetPaxosLogs(ProposalType::SQL);
  auto cdnt_paxos_logs = GetPaxosLogs(ProposalType::COORDINATOR);
  *response->mutable_sql_paxos_logs()->mutable_logs() =
      google::protobuf::Map<int, PaxosLog>(sql_paxos_logs.begin(),
                                           sql_paxos_logs.end());
  *response->mutable_cdnt_paxos_logs()->mutable_logs() =
      google::protobuf::Map<int, PaxosLog>(cdnt_paxos_logs.begin(),
                                           cdnt_paxos_logs.end());
  return Status::OK;
}

Status MultiPaxosServiceImpl::Register(grpc::ServerContext* context,
                                       const RegisterRequest* request,
                                       RegisterResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "[Coordinator] Received RegisterRequest." << std::endl;
  }
  User user = request->user();
  Proposal proposal;
  proposal.set_type(ProposalType::SQL);
  *proposal.mutable_register_request() = *request;
  auto register_status = RunPaxos(&proposal);
  if (register_status.ok()) {
    *response->mutable_user() = proposal.register_response().user();
    std::cout << response->user().DebugString();
  } else {
    return Status(register_status.error_code(),
                  register_status.error_message());
  }
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "[Coordinator] Returning RegisterResponse." << std::endl;
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::LogIn(grpc::ServerContext* context,
                                    const LogInRequest* request,
                                    LogInResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "[Coordinator] Received LogInRequest." << std::endl;
  }
  User user = request->user();
  auto qurery_status = GetUserByCredentials(&user);
  if (qurery_status.ok()) {
    *response->mutable_user() = user;
    std::cout << response->user().DebugString();
  } else {
    return Status(qurery_status.error_code(), qurery_status.error_message());
  }
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "[Coordinator] Returning LogInResponse." << std::endl;
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::GetUser(grpc::ServerContext* context,
                                      const GetUserRequest* request,
                                      GetUserResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Received GetUserRequest." << request->user().DebugString()
             << std::endl;
  }
  User user = request->user();
  auto get_user_status = GetUserByUserId(&user);
  auto get_stats_status = GetUserStatsByUserId(&user);
  auto get_posts_status = GetPostsByUserId(&user);
  if (get_user_status.ok() && get_stats_status.ok() && get_posts_status.ok()) {
    *response->mutable_user() = user;
    std::cout << response->user().DebugString();
  } else {
    return Status(grpc::StatusCode::ABORTED,
                  get_user_status.error_message() +
                      get_stats_status.error_message() +
                      get_posts_status.error_message());
  }
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Returning GetUserResponse." << std::endl;
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::GetFeed(grpc::ServerContext* context,
                                      const GetFeedRequest* request,
                                      GetFeedResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Received GetFeedRequest." << std::endl;
  }
  User user = request->user();
  auto get_feed_status = GetFeedByUserId(&user, response);
  if (get_feed_status.ok()) {
    std::cout << response->DebugString();
  } else {
    return Status(grpc::StatusCode::ABORTED, get_feed_status.error_message());
  }
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Returning GetFeedResponse." << std::endl;
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::GetPost(grpc::ServerContext* context,
                                      const GetPostRequest* request,
                                      GetPostResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Received GetPostRequest." << std::endl;
  }
  Post post = request->post();
  auto get_post_status = GetCompletePost(&post);
  if (get_post_status.ok()) {
    *response->mutable_post() = post;
    std::cout << response->DebugString();
  } else {
    return Status(grpc::StatusCode::ABORTED, get_post_status.error_message());
  }
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Returning GetPostResponse." << std::endl;
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::GetFollowers(grpc::ServerContext* context,
                                           const GetFollowersRequest* request,
                                           GetFollowersResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Received GetFollowersRequest." << std::endl;
  }
  User user = request->user();
  auto get_followers_status = GetFollowersByUserId(&user, response);
  if (get_followers_status.ok()) {
    std::cout << response->DebugString();
  } else {
    return Status(grpc::StatusCode::ABORTED,
                  get_followers_status.error_message());
  }
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Returning GetFollowersResponse." << std::endl;
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::GetFollowings(grpc::ServerContext* context,
                                            const GetFollowingsRequest* request,
                                            GetFollowingsResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Received GetFollowingsRequest." << std::endl;
  }
  User user = request->user();
  auto get_followings_status = GetFollowingsByUserId(&user, response);
  if (get_followings_status.ok()) {
    std::cout << response->DebugString();
  } else {
    return Status(grpc::StatusCode::ABORTED,
                  get_followings_status.error_message());
  }
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Returning GetFollowingsResponse." << std::endl;
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::SetUser(grpc::ServerContext* context,
                                      const SetUserRequest* request,
                                      SetUserResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Received SetUserRequest." << std::endl;
  }
  Proposal proposal;
  proposal.set_type(ProposalType::SQL);
  *proposal.mutable_set_user_request() = *request;
  auto set_user_status = RunPaxos(&proposal);
  if (set_user_status.ok()) {
    *response->mutable_user() = proposal.set_user_response().user();
    std::cout << response->user().DebugString();
  } else {
    return Status(set_user_status.error_code(),
                  set_user_status.error_message());
  }
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Returning SetUserResponse." << std::endl;
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::SetPost(grpc::ServerContext* context,
                                      const SetPostRequest* request,
                                      SetPostResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Received SetPostRequest." << std::endl;
  }
  Proposal proposal;
  proposal.set_type(ProposalType::SQL);
  *proposal.mutable_set_post_request() = *request;
  auto set_post_status = RunPaxos(&proposal);
  if (set_post_status.ok()) {
    *response->mutable_post() = proposal.set_post_response().post();
    std::cout << response->post().DebugString();
  } else {
    return Status(set_post_status.error_code(),
                  set_post_status.error_message());
  }
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Returning SetPostResponse." << std::endl;
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::SetLike(grpc::ServerContext* context,
                                      const SetLikeRequest* request,
                                      SetLikeResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Received SetLikeRequest." << std::endl;
  }
  Proposal proposal;
  proposal.set_type(ProposalType::SQL);
  *proposal.mutable_set_like_request() = *request;
  auto set_like_status = RunPaxos(&proposal);
  if (set_like_status.ok()) {
    *response->mutable_like() = proposal.set_like_response().like();
    std::cout << response->like().DebugString();
  } else {
    return Status(set_like_status.error_code(),
                  set_like_status.error_message());
  }
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Returning SetLikeResponse." << std::endl;
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::SetComment(grpc::ServerContext* context,
                                         const SetCommentRequest* request,
                                         SetCommentResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Received SetCommentRequest." << std::endl;
  }
  Proposal proposal;
  proposal.set_type(ProposalType::SQL);
  *proposal.mutable_set_comment_request() = *request;
  auto set_comment_status = RunPaxos(&proposal);
  if (set_comment_status.ok()) {
    *response->mutable_comment() = proposal.set_comment_response().comment();
    std::cout << response->comment().DebugString();
  } else {
    return Status(set_comment_status.error_code(),
                  set_comment_status.error_message());
  }
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Returning SetCommentResponse." << std::endl;
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::SetFollow(grpc::ServerContext* context,
                                        const SetFollowRequest* request,
                                        SetFollowResponse* response) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Received SetFollowRequest." << std::endl;
  }
  Proposal proposal;
  proposal.set_type(ProposalType::SQL);
  *proposal.mutable_set_follow_request() = *request;
  auto set_follow_status = RunPaxos(&proposal);
  if (set_follow_status.ok()) {
    *response->mutable_follower() = proposal.set_follow_response().follower();
    *response->mutable_followee() = proposal.set_follow_response().followee();
    std::cout << response->DebugString();
  } else {
    return Status(set_follow_status.error_code(),
                  set_follow_status.error_message());
  }
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Returning SetFollowResponse." << std::endl;
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::GetFeedByUserId(User* user,
                                              GetFeedResponse* response) {
  std::string stmt =
      "SELECT * FROM Posts WHERE UserId IN (SELECT FolloweeId FROM Follows "
      "WHERE UserId=%0q:userid) ORDER BY PostId DESC";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set = query.store(user->user_id());
    if (result_set.size() > 0) {
      for (auto it = result_set.begin(); it != result_set.end(); ++it) {
        auto* post = response->add_post();
        mysqlpp::Row row = *it;
        post->set_post_id(row["PostId"]);
        post->set_user_id(row["UserId"]);
        post->set_content(row["Content"]);
        post->set_image_id(row["ImageId"]);
        post->set_created(row["Created"]);
        post->set_direct_repost_id(
            (row["RepostId"] == mysqlpp::null) ? 0 : row["RepostId"]);
        Post original_post;
        original_post.set_post_id((row["OriginalPostId"] == mysqlpp::null)
                                      ? 0
                                      : row["OriginalPostId"]);
        if (original_post.post_id() != 0) {
          auto get_origin_status = GetOriginalPostByPostId(&original_post);
          if (!get_origin_status.ok()) {
            return Status(get_origin_status.error_code(),
                          get_origin_status.error_message());
          }
        }
        *post->mutable_original_post() = original_post;
        auto get_stats_status = GetPostStatByPostId(post);
        if (!get_stats_status.ok()) {
          return Status(get_stats_status.error_code(),
                        get_stats_status.error_message());
        }
      }
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::DeleteUser(User* user) {
  std::string stmt = "DELETE FROM Users WHERE UserId=%0q:userid";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    auto result = query.execute(user->user_id());
    if (result.rows() > 0) {
      auto get_user_status = GetUserByUserId(user);
      if (get_user_status.ok()) {  // Shouldn't exist.
        return Status(grpc::StatusCode::ABORTED, "DELETE User Failed.");
      }
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql UPDATE Failed.");
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::UpdateUser(User* user) {
  std::string stmt =
      "UPDATE Users SET Name=%1q:name, Password=AES_ENCRYPT(%2q:password, "
      "UNHEX(SHA2(%5q:passphrase,%6:hashlen))), "
      "Email=%3q:email,Phone=%4q:phone WHERE UserId=%0q:userid";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    auto result =
        query.execute(user->user_id(), user->name(), user->password(),
                      user->email(), user->phone(), pass_phrase_, hash_len_);
    if (result.rows() > 0) {
      auto get_user_status = GetUserByUserId(user);
      if (!get_user_status.ok()) {
        return Status(get_user_status.error_code(),
                      get_user_status.error_message());
      }
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql UPDATE Failed.");
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::DeletePost(Post* post) {
  std::string stmt = "DELETE FROM Posts WHERE PostId=%0q:postid";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    auto result = query.execute(post->post_id());
    if (result.rows() > 0) {
      auto get_post_status = GetPostByPostId(post);
      if (get_post_status.ok()) {  // Shouldn't exist.
        return Status(grpc::StatusCode::ABORTED, "DELETE Post Failed.");
      }
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql DELETE Failed.");
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::CreatePost(Post* post) {
  std::string stmt =
      "INSERT INTO Posts (UserId, Content, RepostId) VALUES "
      "(%0q:userid, %1q:content, %2q:repostid)";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    std::cout << post->DebugString();
    mysqlpp::SimpleResult result;
    if (post->direct_repost_id() != 0) {
      result = query.execute(post->user_id(), post->content(),
                             post->direct_repost_id());
    } else {
      result = query.execute(post->user_id(), post->content(), mysqlpp::null);
    }
    if (result.rows() > 0) {
      std::string stmt = "SELECT LAST_INSERT_ID() AS PostId";
      mysqlpp::Connection mysql_conn(false);
      if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                             mysql_user_.c_str(), mysql_password_.c_str())) {
      } else {
        return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
      }
      mysqlpp::Query query = mysql_conn.query(stmt);
      query.parse();
      mysqlpp::StoreQueryResult result_set = query.store();
      if (result_set.size() == 1) {
        auto it = result_set.begin();
        mysqlpp::Row row = *it;
        post->set_post_id(row["PostId"]);
      } else {
        return Status(grpc::StatusCode::NOT_FOUND, "Mysql SELECT Failed.");
      }
      auto get_post_status = GetPostByPostId(post);
      if (!get_post_status.ok()) {
        return Status(get_post_status.error_code(),
                      get_post_status.error_message());
      }
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql CREATE Failed.");
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::GetLikeByLikeId(Like* like) {
  std::string stmt =
      "SELECT LikeId,PostId,UserId,Created FROM PostLikes WHERE "
      "LikeId=%0q:likeid";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set = query.store(like->like_id());
    if (result_set.size() == 1) {
      auto it = result_set.begin();
      mysqlpp::Row row = *it;
      like->set_like_id(row["LikeId"]);
      like->set_post_id(row["PostId"]);
      like->set_user_id(row["UserId"]);
      like->set_created(row["Created"]);
    } else {
      return Status(grpc::StatusCode::ABORTED, "SELECT Like Failed.");
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::DeleteLike(Like* like) {
  std::string stmt = "DELETE FROM PostLikes WHERE LikeId=%0q:likeid";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    auto result = query.execute(like->like_id());
    if (result.rows() > 0) {
      auto get_like_status = GetLikeByLikeId(like);
      if (get_like_status.ok()) {  // Shouldn't exist.
        return Status(grpc::StatusCode::ABORTED, "DELETE Like Failed.");
      }
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql DELETE Failed.");
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::CreateLike(Like* like) {
  std::string stmt =
      "INSERT INTO PostLikes (UserId, PostId) VALUES (%0q:userid, "
      "%1q:postid)";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    std::cout << like->DebugString();
    mysqlpp::SimpleResult result;
    result = query.execute(like->user_id(), like->post_id());
    if (result.rows() > 0) {
      std::string stmt = "SELECT LAST_INSERT_ID() AS LikeId";
      mysqlpp::Connection mysql_conn(false);
      if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                             mysql_user_.c_str(), mysql_password_.c_str())) {
      } else {
        return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
      }
      mysqlpp::Query query = mysql_conn.query(stmt);
      query.parse();
      mysqlpp::StoreQueryResult result_set = query.store();
      if (result_set.size() == 1) {
        auto it = result_set.begin();
        mysqlpp::Row row = *it;
        like->set_like_id(row["LikeId"]);
      } else {
        return Status(grpc::StatusCode::NOT_FOUND,
                      "Mysql SELECT LAST_INSERT_ID() Failed.");
      }
      auto get_like_status = GetLikeByLikeId(like);
      if (!get_like_status.ok()) {
        return Status(get_like_status.error_code(),
                      get_like_status.error_message());
      }
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql CREATE Failed.");
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::GetCommentByCommentId(Comment* comment) {
  std::string stmt =
      "SELECT CommentId,PostId,UserId,Content,Created FROM "
      "PostComments WHERE CommentId=%0q:commentid";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set = query.store(comment->comment_id());
    if (result_set.size() == 1) {
      auto it = result_set.begin();
      mysqlpp::Row row = *it;
      comment->set_comment_id(row["CommentId"]);
      comment->set_post_id(row["PostId"]);
      comment->set_user_id(row["UserId"]);
      comment->set_content(row["Content"]);
      comment->set_created(row["Created"]);
    } else {
      return Status(grpc::StatusCode::ABORTED, "SELECT Comment Failed.");
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::DeleteComment(Comment* comment) {
  std::string stmt = "DELETE FROM PostComments WHERE CommentId=%0q:commentid";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    auto result = query.execute(comment->comment_id());
    if (result.rows() > 0) {
      auto get_comment_status = GetCommentByCommentId(comment);
      if (get_comment_status.ok()) {  // Shouldn't exist.
        return Status(grpc::StatusCode::ABORTED, "DELETE Comment Failed.");
      }
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql DELETE Failed.");
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::CreateComment(Comment* comment) {
  std::string stmt =
      "INSERT INTO PostComments (UserId, PostId, Content) "
      "VALUES (%0q:userid, %1q:postid, %2q:content)";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    std::cout << comment->DebugString();
    mysqlpp::SimpleResult result;
    result = query.execute(comment->user_id(), comment->post_id(),
                           comment->content());
    if (result.rows() > 0) {
      std::string stmt = "SELECT LAST_INSERT_ID() AS CommentId";
      mysqlpp::Connection mysql_conn(false);
      if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                             mysql_user_.c_str(), mysql_password_.c_str())) {
      } else {
        return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
      }
      mysqlpp::Query query = mysql_conn.query(stmt);
      query.parse();
      mysqlpp::StoreQueryResult result_set = query.store();
      if (result_set.size() == 1) {
        auto it = result_set.begin();
        mysqlpp::Row row = *it;
        comment->set_comment_id(row["CommentId"]);
      } else {
        return Status(grpc::StatusCode::NOT_FOUND,
                      "Mysql SELECT LAST_INSERT_ID() Failed.");
      }
      auto get_comment_status = GetCommentByCommentId(comment);
      if (!get_comment_status.ok()) {
        return Status(get_comment_status.error_code(),
                      get_comment_status.error_message());
      }
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql CREATE Failed.");
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::GetFollowByIds(SetFollowResponse* response) {
  std::string stmt =
      "SELECT Created FROM Follows WHERE UserId=%0q:userid AND "
      "FolloweeId=%1q:followeeid";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set = query.store(
        response->follower().user_id(), response->followee().user_id());
    if (result_set.size() == 1) {
      auto it = result_set.begin();
      mysqlpp::Row row = *it;
      response->set_created(row["Created"]);
    } else {
      return Status(grpc::StatusCode::ABORTED, "SELECT Follow Failed.");
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::DeleteFollow(SetFollowResponse* response) {
  std::string stmt =
      "DELETE FROM Follows WHERE UserId=%0q:userid AND "
      "FolloweeId=%1q:followeeid";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    auto result = query.execute(response->follower().user_id(),
                                response->followee().user_id());
    if (result.rows() > 0) {
      auto get_follow_status = GetFollowByIds(response);
      if (get_follow_status.ok()) {  // Shouldn't exist.
        return Status(grpc::StatusCode::ABORTED, "DELETE Follow Failed.");
      }
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql DELETE Failed.");
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::CreateFollow(SetFollowResponse* response) {
  std::string stmt =
      "INSERT INTO Follows (UserId, FolloweeId) VALUES "
      "(%0q:userid, %1q:followeeid)";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    std::cout << response->DebugString();
    mysqlpp::SimpleResult result;
    result = query.execute(response->follower().user_id(),
                           response->followee().user_id());
    if (result.rows() > 0) {
      auto get_follow_status = GetFollowByIds(response);
      if (!get_follow_status.ok()) {
        return Status(get_follow_status.error_code(),
                      get_follow_status.error_message());
      }
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql CREATE Failed.");
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::GetUserByCredentials(User* user) {
  std::string stmt =
      "SELECT UserId,Name,ProfilePicture,Email,Phone FROM Users WHERE "
      "Name=%0q:username AND Password=AES_ENCRYPT(%1q:password, "
      "UNHEX(SHA2(%2q:passphrase,%3:hashlen)))";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set =
        query.store(user->name(), user->password(), pass_phrase_, hash_len_);
    if (result_set.size() == 1) {
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
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::GetUserByUserId(User* user) {
  std::string stmt =
      "SELECT UserId,Name,ProfilePicture,Email,Phone FROM Users "
      "WHERE UserId=%0q:userid";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set = query.store(user->user_id());
    if (result_set.size() == 1) {
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
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::GetUserStatsByUserId(User* user) {
  std::string stmt =
      "SELECT Posts,Following,Followers FROM UserStats WHERE "
      "UserId=%0q:userid";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set = query.store(user->user_id());
    if (result_set.size() == 1) {
      auto it = result_set.begin();
      mysqlpp::Row row = *it;
      user->set_num_posts(row["Posts"]);
      user->set_num_following(row["Following"]);
      user->set_num_followers(row["Followers"]);
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql SELECT Failed.");
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::GetPostsByUserId(User* user) {
  std::string stmt =
      "SELECT PostId,UserId,Content,ImageId,Created,RepostId,OriginalPostId "
      "FROM Posts WHERE UserId=%0q:userid ORDER BY PostId DESC LIMIT 10";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set = query.store(user->user_id());
    if (result_set.size() > 0) {
      for (auto it = result_set.begin(); it != result_set.end(); ++it) {
        auto* post = user->add_recent_posts();
        mysqlpp::Row row = *it;
        post->set_post_id(row["PostId"]);
        post->set_user_id(row["UserId"]);
        post->set_content(row["Content"]);
        post->set_image_id(row["ImageId"]);
        post->set_created(row["Created"]);
        post->set_direct_repost_id(
            (row["RepostId"] == mysqlpp::null) ? 0 : row["RepostId"]);
        Post original_post;
        original_post.set_post_id((row["OriginalPostId"] == mysqlpp::null)
                                      ? 0
                                      : row["OriginalPostId"]);
        if (original_post.post_id() != 0) {
          auto get_origin_status = GetOriginalPostByPostId(&original_post);
          if (!get_origin_status.ok()) {
            return Status(get_origin_status.error_code(),
                          get_origin_status.error_message());
          }
        }
        *post->mutable_original_post() = original_post;
        auto get_stats_status = GetPostStatByPostId(post);
        if (!get_stats_status.ok()) {
          return Status(get_stats_status.error_code(),
                        get_stats_status.error_message());
        }
      }
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::GetOriginalPostByPostId(Post* post) {
  std::string stmt =
      "SELECT PostId,UserId,Content,ImageId,Created,RepostId,OriginalPostId "
      "FROM Posts WHERE PostId=%0q:postid";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set = query.store(post->post_id());
    if (result_set.size() == 1) {
      auto it = result_set.begin();
      mysqlpp::Row row = *it;
      post->set_post_id(row["PostId"]);
      post->set_user_id(row["UserId"]);
      post->set_content(row["Content"]);
      post->set_image_id(row["ImageId"]);
      post->set_created(row["Created"]);
      post->set_direct_repost_id(
          (row["RepostId"] == mysqlpp::null) ? 0 : row["RepostId"]);
      auto get_stats_status = GetPostStatByPostId(post);
      if (!get_stats_status.ok()) {
        return Status(get_stats_status.error_code(),
                      get_stats_status.error_message());
      }
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql SELECT Failed.");
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::GetPostByPostId(Post* post) {
  std::string stmt =
      "SELECT PostId,UserId,Content,ImageId,Created,RepostId,OriginalPostId "
      "FROM Posts WHERE PostId=%0q:postid";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set = query.store(post->post_id());
    if (result_set.size() == 1) {
      auto it = result_set.begin();
      mysqlpp::Row row = *it;
      post->set_post_id(row["PostId"]);
      post->set_user_id(row["UserId"]);
      post->set_content(row["Content"]);
      post->set_image_id(row["ImageId"]);
      post->set_created(row["Created"]);
      post->set_direct_repost_id(
          (row["RepostId"] == mysqlpp::null) ? 0 : row["RepostId"]);
      Post original_post;
      original_post.set_post_id(
          (row["OriginalPostId"] == mysqlpp::null) ? 0 : row["OriginalPostId"]);
      if (original_post.post_id() != 0) {
        auto get_origin_status = GetOriginalPostByPostId(&original_post);
        if (!get_origin_status.ok()) {
          return Status(get_origin_status.error_code(),
                        get_origin_status.error_message());
        }
      }
      *post->mutable_original_post() = original_post;
      auto get_stats_status = GetPostStatByPostId(post);
      if (!get_stats_status.ok()) {
        return Status(get_stats_status.error_code(),
                      get_stats_status.error_message());
      }
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql SELECT Failed.");
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::GetCompletePost(Post* post) {
  auto get_post_status = GetPostByPostId(post);
  if (get_post_status.ok()) {
    std::string stmt =
        "SELECT PostId,CommentId,UserId,Content,Created FROM PostComments "
        "WHERE PostId=%0q:postid ORDER BY CommentId DESC LIMIT 10";
    mysqlpp::Connection mysql_conn(false);
    if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                           mysql_user_.c_str(), mysql_password_.c_str())) {
      mysqlpp::Query query = mysql_conn.query(stmt);
      query.parse();
      mysqlpp::StoreQueryResult result_set = query.store(post->post_id());
      if (result_set.size() > 0) {
        for (auto it = result_set.begin(); it != result_set.end(); ++it) {
          auto* comment = post->add_comment();
          mysqlpp::Row row = *it;
          comment->set_post_id(row["PostId"]);
          comment->set_comment_id(row["CommentId"]);
          comment->set_user_id(row["UserId"]);
          comment->set_content(row["Content"]);
          comment->set_created(row["Created"]);
        }
      }
      std::string stmt2 =
          "SELECT PostId,LikeId,UserId,Created FROM PostLikes "
          "WHERE PostId=%0q:postid ORDER BY LikeId DESC LIMIT 10";
      mysqlpp::Query query2 = mysql_conn.query(stmt2);
      query2.parse();
      mysqlpp::StoreQueryResult result_set2 = query2.store(post->post_id());
      if (result_set2.size() > 0) {
        for (auto it = result_set2.begin(); it != result_set2.end(); ++it) {
          auto* like = post->add_like();
          mysqlpp::Row row = *it;
          like->set_post_id(row["PostId"]);
          like->set_like_id(row["LikeId"]);
          like->set_user_id(row["UserId"]);
          like->set_created(row["Created"]);
        }
      }
      std::string stmt3 =
          "SELECT PostId,OriginalPostId,UserId,Content,Created "
          "FROM Posts WHERE OriginalPostId=%0q:postid LIMIT 10";
      mysqlpp::Query query3 = mysql_conn.query(stmt3);
      query3.parse();
      mysqlpp::StoreQueryResult result_set3 = query3.store(post->post_id());
      if (result_set3.size() > 0) {
        for (auto it = result_set3.begin(); it != result_set3.end(); ++it) {
          auto* repost = post->add_repost();
          mysqlpp::Row row = *it;
          repost->set_repost_id(row["PostId"]);
          repost->set_post_id(row["OriginalPostId"]);
          repost->set_user_id(row["UserId"]);
          repost->set_content(row["Content"]);
          repost->set_created(row["Created"]);
        }
      }
    } else {
      return Status(get_post_status.error_code(),
                    get_post_status.error_message());
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::GetPostStatByPostId(Post* post) {
  std::string stmt =
      "SELECT Reposts,Comments,Likes FROM PostStats WHERE PostId=%0q:postid";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set = query.store(post->post_id());
    if (result_set.size() == 1) {
      auto it = result_set.begin();
      mysqlpp::Row row = *it;
      post->set_num_reposts(row["Reposts"]);
      post->set_num_comments(row["Comments"]);
      post->set_num_likes(row["Likes"]);
    } else {
      return Status(grpc::StatusCode::NOT_FOUND, "Mysql SELECT Failed.");
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::GetFollowingsByUserId(
    User* user, GetFollowingsResponse* response) {
  std::string stmt =
      "SELECT Users.UserId AS UserId, Name FROM Users JOIN "
      "(SELECT * FROM Follows WHERE UserId=%0q:userid) AS "
      "Following ON Users.UserId = Following.FolloweeId";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set = query.store(user->user_id());
    if (result_set.size() > 0) {
      for (auto it = result_set.begin(); it != result_set.end(); ++it) {
        auto* following = response->add_user();
        mysqlpp::Row row = *it;
        following->set_user_id(row["UserId"]);
        following->set_name(row["Name"]);
      }
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }

  return Status::OK;
}

Status MultiPaxosServiceImpl::GetFollowersByUserId(
    User* user, GetFollowersResponse* response) {
  std::string stmt =
      "SELECT Users.UserId AS UserId, Name FROM Users JOIN "
      "(SELECT * FROM Follows WHERE FolloweeId=%0q:userid) AS "
      "Follower ON Users.UserId = Follower.UserId";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    mysqlpp::StoreQueryResult result_set = query.store(user->user_id());
    if (result_set.size() > 0) {
      for (auto it = result_set.begin(); it != result_set.end(); ++it) {
        auto* follower = response->add_user();
        mysqlpp::Row row = *it;
        follower->set_user_id(row["UserId"]);
        follower->set_name(row["Name"]);
      }
    }
  } else {
    return Status(grpc::StatusCode::ABORTED, "Mysql Connection Failed.");
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::GetCoordinator() {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Sending Requests to get Coordinator address." << std::endl;
  }
  assert(paxos_stubs_map_ != nullptr);
  auto stubs = paxos_stubs_map_->GetPaxosStubs();
  std::set<std::string> coordinators;
  std::set<std::string> live_paxos_stubs;
  for (const auto& stub : stubs) {
    ClientContext context;
    auto deadline =
        std::chrono::system_clock::now() + std::chrono::milliseconds(1000);
    context.set_deadline(deadline);
    EmptyMessage get_cdnt_req;
    GetCoordinatorResponse get_cdnt_resp;
    Status get_status =
        stub.second->GetCoordinator(&context, get_cdnt_req, &get_cdnt_resp);
    if (get_status.ok()) {
      live_paxos_stubs.insert(stub.first);
      if (!get_cdnt_resp.coordinator().empty()) {
        coordinators.insert(get_cdnt_resp.coordinator());
      }
    }
  }
  if (coordinators.size() != 1) {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Failed to get Coordinator addresses." << std::endl;
    return Status(grpc::StatusCode::ABORTED,
                  "Failed to get Coordinator addresses.");
  }
  if (live_paxos_stubs.find(*coordinators.begin()) == live_paxos_stubs.end()) {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Coordinator is unavailable." << std::endl;
    return Status(grpc::StatusCode::ABORTED, "Coordinator is unavailable.");
  }
  paxos_stubs_map_->SetCoordinator(*coordinators.begin());
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "[Success] Set Coordinator addresses to ["
             << *coordinators.begin() << "]." << std::endl;
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::ElectNewCoordinator() {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Sending Request to elect Coordinator via Paxos." << std::endl;
  }
  assert(paxos_stubs_map_ != nullptr);
  auto* my_paxos_stub = paxos_stubs_map_->GetStub(my_paxos_address_);
  ClientContext context;
  auto deadline =
      std::chrono::system_clock::now() + std::chrono::milliseconds(5000);
  context.set_deadline(deadline);
  ElectCoordinatorRequest elect_cdnt_req;
  elect_cdnt_req.set_coordinator(my_paxos_address_);
  EmptyMessage elect_cdnt_resp;
  return my_paxos_stub->ElectCoordinator(&context, elect_cdnt_req,
                                         &elect_cdnt_resp);
}

Status MultiPaxosServiceImpl::GetRecovery() {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Sending RecoverRequest to Coordinator." << std::endl;
  }
  assert(paxos_stubs_map_ != nullptr);
  auto* coordinator_stub = paxos_stubs_map_->GetCoordinatorStub();
  ClientContext context;
  auto deadline =
      std::chrono::system_clock::now() + std::chrono::milliseconds(5000);
  context.set_deadline(deadline);
  RecoverRequest recover_req;
  RecoverResponse recover_resp;
  Status recover_status =
      coordinator_stub->Recover(&context, recover_req, &recover_resp);
  if (!recover_status.ok()) {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Failed to get Recovery from Coordinator." << std::endl;
    return Status(grpc::StatusCode::ABORTED,
                  "Failed to get Recovery from Coordinator.");
  }
  int last_cdnt_round = GetLatestRound(ProposalType::COORDINATOR);
  int last_sql_round = GetLatestRound(ProposalType::SQL);
  const auto& sql_paxos_logs = recover_resp.sql_paxos_logs().logs();
  auto it = sql_paxos_logs.find(last_sql_round);
  if (it == sql_paxos_logs.end()) {
    it = sql_paxos_logs.begin();
  } else {
    it++;
  }
  for (; it != sql_paxos_logs.end(); ++it) {
    AddPaxosLog(ProposalType::SQL, it->first, it->second.promised_id(),
                it->second.accepted_id(), it->second.proposal());
    auto* my_paxos_stub = paxos_stubs_map_->GetStub(my_paxos_address_);
    InformRequest inform_req;
    *(inform_req.mutable_acceptance()->mutable_proposal()) =
        it->second.proposal();
    ClientContext context;
    auto deadline =
        std::chrono::system_clock::now() + std::chrono::milliseconds(5000);
    context.set_deadline(deadline);
    InformResponse inform_resp;
    Status inform_status =
        my_paxos_stub->Inform(&context, inform_req, &inform_resp);
    if (!inform_status.ok()) {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      std::cout << inform_resp.proposal().DebugString();
      TIME_LOG << "[" << my_paxos_address_ << "] "
               << "  Recover SQL falied." << std::endl;
    } else {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      std::cout << inform_resp.proposal().DebugString();
      TIME_LOG << "[" << my_paxos_address_ << "] "
               << "successfully executed SQL!" << std::endl;
    }
  }
  const auto& cdnt_paxos_logs = recover_resp.cdnt_paxos_logs().logs();
  auto it2 = cdnt_paxos_logs.find(last_cdnt_round);
  if (it2 == cdnt_paxos_logs.end()) {
    it2 = cdnt_paxos_logs.begin();
  } else {
    it2++;
  }
  for (; it2 != cdnt_paxos_logs.end(); ++it2) {
    AddPaxosLog(ProposalType::COORDINATOR, it2->first,
                it2->second.promised_id(), it2->second.accepted_id(),
                it2->second.proposal());
  }

  // auto tmp_kv_db = kv_db_->GetDataMap();
  // auto tmp_paxos_keys = kv_db_->GetPaxosLogKeys();
  // {
  //   std::unique_lock<std::mutex> writer_lock(log_mtx_);
  //   TIME_LOG << "[" << my_paxos_address_ << "] "
  //            << "Received recovery snapshot:" << std::endl;
  //   TIME_LOG << "[" << my_paxos_address_ << "] "
  //            << "KV Data Map:" << std::endl;
  //   for (const auto& kv : tmp_kv_db) {
  //     TIME_LOG << "[" << my_paxos_address_ << "] "
  //              << "  [key: " << kv.first << ", value: " << kv.second << "]"
  //              << std::endl;
  //   }
  //   TIME_LOG << "[" << my_paxos_address_ << "] "
  //            << "Paxos Logs:" << std::endl;
  //   for (const auto& key : tmp_paxos_keys) {
  //     TIME_LOG << "[" << my_paxos_address_ << "] "
  //              << "  [key: " << key
  //              << ", last round: " << kv_db_->GetLatestRound(key) << "]."
  //              << std::endl;
  //   }
  TIME_LOG << "[" << my_paxos_address_ << "] "
           << "[Success] Recovered data and paxos logs." << std::endl;
  // }
  return Status::OK;
}

Status MultiPaxosServiceImpl::Register(Proposal* proposal) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "[Coordinator] Received RegisterRequest." << std::endl;
  }
  RegisterRequest* request = proposal->mutable_register_request();
  RegisterResponse* response = proposal->mutable_register_response();
  User user = request->user();
  *response->mutable_user() = user;
  std::string stmt =
      "INSERT INTO Users (Name, Password, ProfilePicture, Email, Phone) "
      "VALUES (%0q:username, AES_ENCRYPT(%1q:password, UNHEX(SHA2('My secret "
      "passphrase',512))), %2q:profile, %3q:email, %4q:phone);";
  mysqlpp::Connection mysql_conn(false);
  if (mysql_conn.connect(db_name_.c_str(), mysql_server_.c_str(),
                         mysql_user_.c_str(), mysql_password_.c_str())) {
    mysqlpp::Query query = mysql_conn.query(stmt);
    query.parse();
    auto result =
        query.execute(user.name(), user.password(), user.profile_picture(),
                      user.email(), user.phone());
    if (result.rows() > 0) {
      auto qurery_status = GetUserByCredentials(&user);
      if (qurery_status.ok()) {
        *response->mutable_user() = user;
        std::cout << response->user().DebugString();
      } else {
        return Status(qurery_status.error_code(),
                      qurery_status.error_message());
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

Status MultiPaxosServiceImpl::SetUser(Proposal* proposal) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Received SetUserRequest." << std::endl;
  }
  SetUserRequest* request = proposal->mutable_set_user_request();
  SetUserResponse* response = proposal->mutable_set_user_response();
  User user = request->user();
  Status set_user_status;
  if (request->operation() == OperationType::UPDATE) {
    set_user_status = UpdateUser(&user);
  } else if (request->operation() == OperationType::DELETE) {
    set_user_status = DeleteUser(&user);
  } else {
    return Status(grpc::StatusCode::ABORTED, "Operation not supported.");
  }
  if (set_user_status.ok()) {
    *response->mutable_user() = user;
    std::cout << response->user().DebugString();
  } else {
    return Status(set_user_status.error_code(),
                  set_user_status.error_message());
  }
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Returning SetUserResponse." << std::endl;
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::SetPost(Proposal* proposal) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Received SetPostRequest." << std::endl;
  }
  SetPostRequest* request = proposal->mutable_set_post_request();
  SetPostResponse* response = proposal->mutable_set_post_response();
  Post post = request->post();
  Status set_post_status;
  if (request->operation() == OperationType::CREATE) {
    set_post_status = CreatePost(&post);
  } else if (request->operation() == OperationType::DELETE) {
    set_post_status = DeletePost(&post);
  } else {
    return Status(grpc::StatusCode::ABORTED, "Operation not supported.");
  }
  if (set_post_status.ok()) {
    *response->mutable_post() = post;
    std::cout << response->post().DebugString();
  } else {
    return Status(set_post_status.error_code(),
                  set_post_status.error_message());
  }
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Returning SetPostResponse." << std::endl;
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::SetLike(Proposal* proposal) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Received SetLikeRequest." << std::endl;
  }
  SetLikeRequest* request = proposal->mutable_set_like_request();
  SetLikeResponse* response = proposal->mutable_set_like_response();
  Like like = request->like();
  Status set_like_status;
  if (request->operation() == OperationType::CREATE) {
    set_like_status = CreateLike(&like);
  } else if (request->operation() == OperationType::DELETE) {
    set_like_status = DeleteLike(&like);
  } else {
    return Status(grpc::StatusCode::ABORTED, "Operation not supported.");
  }
  if (set_like_status.ok()) {
    *response->mutable_like() = like;
    std::cout << response->like().DebugString();
  } else {
    return Status(set_like_status.error_code(),
                  set_like_status.error_message());
  }
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Returning SetLikeResponse." << std::endl;
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::SetComment(Proposal* proposal) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Received SetCommentRequest." << std::endl;
  }
  SetCommentRequest* request = proposal->mutable_set_comment_request();
  SetCommentResponse* response = proposal->mutable_set_comment_response();
  Comment comment = request->comment();
  Status set_comment_status;
  if (request->operation() == OperationType::CREATE) {
    set_comment_status = CreateComment(&comment);
  } else if (request->operation() == OperationType::DELETE) {
    set_comment_status = DeleteComment(&comment);
  } else {
    return Status(grpc::StatusCode::ABORTED, "Operation not supported.");
  }
  if (set_comment_status.ok()) {
    *response->mutable_comment() = comment;
    std::cout << response->comment().DebugString();
  } else {
    return Status(set_comment_status.error_code(),
                  set_comment_status.error_message());
  }
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Returning SetCommentResponse." << std::endl;
  }
  return Status::OK;
}

Status MultiPaxosServiceImpl::SetFollow(Proposal* proposal) {
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Received SetFollowRequest." << std::endl;
  }
  SetFollowRequest* request = proposal->mutable_set_follow_request();
  SetFollowResponse* response = proposal->mutable_set_follow_response();
  *response->mutable_follower() = request->follower();
  *response->mutable_followee() = request->followee();
  Status set_follow_status;
  if (request->operation() == OperationType::CREATE) {
    set_follow_status = CreateFollow(response);
  } else if (request->operation() == OperationType::DELETE) {
    set_follow_status = DeleteFollow(response);
  } else {
    return Status(grpc::StatusCode::ABORTED, "Operation not supported.");
  }
  if (set_follow_status.ok()) {
    std::cout << response->DebugString();
  } else {
    return Status(set_follow_status.error_code(),
                  set_follow_status.error_message());
  }
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Returning SetFollowResponse." << std::endl;
  }
  return Status::OK;
}

// Returns a copy of PaxosLogs of a type.
std::map<int, PaxosLog> MultiPaxosServiceImpl::GetPaxosLogs(ProposalType type) {
  if (type == ProposalType::SQL) {
    return paxos_stubs_map_->GetSqlPaxosLogs();
  } else {
    return paxos_stubs_map_->GetCdntPaxosLogs();
  }
}

// Returns the Paxos log for given type & round.
PaxosLog MultiPaxosServiceImpl::GetPaxosLog(ProposalType type, int round) {
  return paxos_stubs_map_->GetPaxosLog(type, round);
}

// Returns the latest Paxos round number for the given type.
int MultiPaxosServiceImpl::GetLatestRound(ProposalType type) {
  return paxos_stubs_map_->GetLatestRound(type);
}
// Add PaxosLog when Acceptor receives a proposal.
void MultiPaxosServiceImpl::AddPaxosLog(ProposalType type, int round) {
  paxos_stubs_map_->AddPaxosLog(type, round);
}
// Update the promised_id in paxos_logs_map_ for the given key and round.
void MultiPaxosServiceImpl::AddPaxosLog(ProposalType type, int round,
                                        int promised_id) {
  paxos_stubs_map_->AddPaxosLog(type, round, promised_id);
}

// Update the acceptance info in paxos_logs_map_ for the given key and round.
void MultiPaxosServiceImpl::AddPaxosLog(ProposalType type, int round,
                                        int accepted_id, Proposal proposal) {
  paxos_stubs_map_->AddPaxosLog(type, round, accepted_id, proposal);
}

// Add PaxosLog from recovery snapshot.
void MultiPaxosServiceImpl::AddPaxosLog(ProposalType type, int round,
                                        int promised_id, int accepted_id,
                                        Proposal proposal) {
  paxos_stubs_map_->AddPaxosLog(type, round, promised_id, accepted_id,
                                proposal);
}

Status MultiPaxosServiceImpl::RunPaxos(Proposal* proposal) {
  int round = GetLatestRound(proposal->type()) + 1;
  int propose_id = 1;
  auto paxos_stubs = paxos_stubs_map_->GetPaxosStubs();
  // Ping.
  std::set<std::string> live_paxos_stubs;
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "Sending PingRequest to " << paxos_stubs.size() << " Acceptors."
             << std::endl;
  }
  for (const auto& stub : paxos_stubs) {
    ClientContext context;
    auto deadline =
        std::chrono::system_clock::now() + std::chrono::milliseconds(500);
    context.set_deadline(deadline);
    EmptyMessage ping_req, ping_resp;
    Status ping_status = stub.second->Ping(&context, ping_req, &ping_resp);
    if (ping_status.ok()) {
      live_paxos_stubs.insert(stub.first);
    }
  }
  if (live_paxos_stubs.empty()) {
    return Status(grpc::StatusCode::ABORTED,
                  "Aborted. Can't connect to any PaxosStub.");
  }
  int num_of_acceptors = live_paxos_stubs.size();
  // Prepare.
  PrepareRequest prepare_req;
  prepare_req.set_type(proposal->type());
  prepare_req.set_round(round);
  prepare_req.set_propose_id(propose_id);

  int num_of_promised = 0;
  int accepted_id = 0;
  // {
  //   std::unique_lock<std::mutex> writer_lock(log_mtx_);
  //   TIME_LOG << "[" << my_paxos_address_ << "] "
  //            << "Sending PrepareRequest [type: " << prepare_req.type()
  //            << ", round: " << prepare_req.round()
  //            << ", propose_id: " << prepare_req.propose_id() << "] to "
  //            << num_of_acceptors << " Acceptors." << std::endl;
  // }
  for (const std::string& addr : live_paxos_stubs) {
    const auto& stub = paxos_stubs[addr];
    ClientContext context;
    auto deadline =
        std::chrono::system_clock::now() + std::chrono::milliseconds(5000);
    context.set_deadline(deadline);
    PromiseResponse promise_resp;
    Status promise_status = stub->Prepare(&context, prepare_req, &promise_resp);
    if (!promise_status.ok()) {
      // std::unique_lock<std::mutex> writer_lock(log_mtx_);
      // TIME_LOG << "[" << my_paxos_address_ << "] "
      //          << "  Acceptor " << addr
      //          << " rejected Prepare: " << promise_status.error_message()
      //          << std::endl;
    } else {
      // {
      //   std::unique_lock<std::mutex> writer_lock(log_mtx_);
      //   TIME_LOG << "[" << my_paxos_address_ << "] "
      //            << "  Acceptor " << addr << " promised." << std::endl;
      // }
      num_of_promised++;
      if (promise_resp.accepted_id() > accepted_id) {
        accepted_id = promise_resp.accepted_id();
        *proposal = promise_resp.proposal();
      }
    }
  }
  std::stringstream quorum_msg;
  quorum_msg << "[type: " << prepare_req.type()
             << ", round: " << prepare_req.round()
             << ", propose_id: " << prepare_req.propose_id()
             << "]: " << num_of_promised << " Promise, "
             << num_of_acceptors - num_of_promised << " Reject.";
  if (num_of_promised <= num_of_acceptors / 2) {
    {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
               << "[Failed QUORUM] on " << quorum_msg.str() << std::endl;
    }
    return Status(grpc::StatusCode::ABORTED,
                  "[Failed QUORUM] on " + quorum_msg.str());
  }
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "[Reached QUORUM] on " << quorum_msg.str() << std::endl;
  }

  // Propose.
  ProposeRequest propose_req;
  propose_req.set_round(round);
  propose_req.set_propose_id(propose_id);
  *propose_req.mutable_proposal() = *proposal;
  // {
  //   std::unique_lock<std::mutex> writer_lock(log_mtx_);
  //   TIME_LOG << "[" << my_paxos_address_ << "] "
  //            << "Sending ProposeRequest [type: " << propose_req.type()
  //            << ", round: " << propose_req.round()
  //            << ", propose_id: " << propose_req.propose_id()
  //            << ", type: " << propose_req.type()
  //            << ", value: " << propose_req.value() << "] to "
  //            << num_of_acceptors << " Acceptors." << std::endl;
  // }
  int num_of_accepted = 0;
  AcceptResponse acceptance;
  for (const std::string& addr : live_paxos_stubs) {
    const auto& stub = paxos_stubs[addr];
    ClientContext context;
    auto deadline =
        std::chrono::system_clock::now() + std::chrono::milliseconds(5000);
    context.set_deadline(deadline);
    AcceptResponse accept_resp;
    Status accept_status = stub->Propose(&context, propose_req, &accept_resp);
    if (!accept_status.ok()) {
      // std::unique_lock<std::mutex> writer_lock(log_mtx_);
      // TIME_LOG << "[" << my_paxos_address_ << "] "
      //          << "  Acceptor " << addr
      //          << " rejected Propose: " << accept_status.error_message()
      //          << std::endl;
    } else {
      // {
      //   std::unique_lock<std::mutex> writer_lock(log_mtx_);
      //   TIME_LOG << "[" << my_paxos_address_ << "] "
      //            << "  Acceptor " << addr << " accepted." << std::endl;
      // }
      num_of_accepted++;
      acceptance = accept_resp;
    }
  }
  std::stringstream consensus_msg;
  consensus_msg << "[type: " << propose_req.proposal().type()
                << ", round: " << propose_req.round()
                << ", propose_id: " << propose_req.propose_id()
                << "]: " << num_of_accepted << " Accept, "
                << num_of_acceptors - num_of_accepted << " Reject.";
  if (num_of_accepted <= num_of_acceptors / 2) {
    {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
               << "[Failed CONSENSUS] on " << consensus_msg.str() << std::endl;
    }
    return Status(grpc::StatusCode::ABORTED,
                  "[Failed CONSENSUS] on " + consensus_msg.str());
  }
  {
    std::unique_lock<std::mutex> writer_lock(log_mtx_);
    TIME_LOG << "[" << my_paxos_address_ << "] "
             << "[Reached CONSENSUS] on " << consensus_msg.str() << std::endl;
  }
  // Inform Learners.
  InformRequest inform_req;
  *inform_req.mutable_acceptance() = acceptance;
  // {
  //   std::unique_lock<std::mutex> writer_lock(log_mtx_);
  //   TIME_LOG << "[" << my_paxos_address_ << "] "
  //            << "Informed " << num_of_acceptors << " Learners:"
  //            << " [type: " << inform_req.type()
  //            << ", type: " << inform_req.acceptance().type()
  //            << ", value: " << inform_req.acceptance().value() << "]."
  //            << std::endl;
  // }
  for (const std::string& addr : live_paxos_stubs) {
    const auto& stub = paxos_stubs[addr];
    ClientContext context;
    auto deadline =
        std::chrono::system_clock::now() + std::chrono::milliseconds(5000);
    context.set_deadline(deadline);
    InformResponse inform_resp;
    Status inform_status = stub->Inform(&context, inform_req, &inform_resp);
    if (!inform_status.ok()) {
      std::unique_lock<std::mutex> writer_lock(log_mtx_);
      TIME_LOG << "[" << my_paxos_address_ << "] "
               << "  Learner " << addr
               << " returned error: " << inform_status.error_message()
               << std::endl;
    } else {
      *proposal = inform_resp.proposal();
      // std::unique_lock<std::mutex> writer_lock(log_mtx_);
      // TIME_LOG << "[" << my_paxos_address_ << "] "
      //          << "  Learner " << addr << " returned OK." << std::endl;
    }
  }
  return Status::OK;
}

// fail_rate_ >= 1 will work as fail_rate_ == 1
// fail_rate_ <= 0 will work as fail_rate_ == 0
bool MultiPaxosServiceImpl::RandomFail() {
  double rand_num = rand();
  if (rand_num / RAND_MAX < fail_rate_) return true;
  return false;
}

}  // namespace obiew
