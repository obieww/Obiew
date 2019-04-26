// Test client side of obiew.

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <grpcpp/grpcpp.h>

#include "obiew.grpc.pb.h"
#include "obiew-service-impl.h"
#include "time_log.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using obiew::User;
using obiew::RegisterRequest;
using obiew::RegisterResponse;
using obiew::LogInRequest;
using obiew::LogInResponse;
using obiew::GetUserRequest;
using obiew::GetUserResponse;
using obiew::SetUserRequest;
using obiew::SetUserResponse;
using obiew::SetPostRequest;
using obiew::SetPostResponse;
using obiew::SetLikeRequest;
using obiew::SetLikeResponse;
using obiew::SetCommentRequest;
using obiew::SetCommentResponse;
using obiew::SetFollowRequest;
using obiew::SetFollowResponse;
using obiew::GetFeedRequest;
using obiew::GetFeedResponse;
using obiew::GetPostRequest;
using obiew::GetPostResponse;
using obiew::GetFollowingsRequest;
using obiew::GetFollowingsResponse;
using obiew::GetFollowersRequest;
using obiew::GetFollowersResponse;
using obiew::Obiew;
using obiew::Post;
using obiew::Comment;
using obiew::Like;
using obiew::Repost;
using obiew::OperationType;
// #define TIME_LOG() std::cout << TimeNow();

class ObiewClient {
 public:
  ObiewClient(std::shared_ptr<Channel> channel)
      : stub_(Obiew::NewStub(channel)) {}

  void Register(const std::string& name, const std::string& password, const std::string& email, const std::string& phone) {
    ClientContext context;
    User user;
    user.set_name(name);
    user.set_password(password);
    user.set_email(email);
    user.set_phone(phone);
    RegisterRequest request;
    *request.mutable_user() = user;
    RegisterResponse response;
    Status status = stub_->Register(&context, request, &response);
    if (!status.ok()) {
      TIME_LOG << "Error Code " << status.error_code() << ". "
               << status.error_message() << std::endl;
    } else {
      std::cout << response.user().DebugString();
      TIME_LOG << "UserId " << response.user().user_id() << " is registered!" << std::endl;
    }
  }

  void LogIn(const std::string& name, const std::string& password) {
    ClientContext context;
    User user;
    user.set_name(name);
    user.set_password(password);
    LogInRequest request;
    *request.mutable_user() = user;
    LogInResponse response;
    Status status = stub_->LogIn(&context, request, &response);
    if (!status.ok()) {
      TIME_LOG << "Error Code " << status.error_code() << ". "
               << status.error_message() << std::endl;
    } else {
      std::cout << response.user().DebugString();
      TIME_LOG << "UserId " << response.user().user_id() << " is logged in!" << std::endl;
    }
  }

  void GetUser(int user_id) {
    ClientContext context;
    User user;
    user.set_user_id(user_id);
    GetUserRequest request;
    *request.mutable_user() = user;
    GetUserResponse response;
    Status status = stub_->GetUser(&context, request, &response);
    if (!status.ok()) {
      TIME_LOG << "Error Code " << status.error_code() << ". "
               << status.error_message() << std::endl;
    } else {
      std::cout << response.user().DebugString();
      TIME_LOG << "UserId " << response.user().user_id() << " is found!" << std::endl;
    }
  }

  void DeleteUser(int user_id) {
    ClientContext context;
    User user;
    user.set_user_id(user_id);
    SetUserRequest request;
    *request.mutable_user() = user;
    request.set_operation(OperationType::DELETE);
    SetUserResponse response;
    Status status = stub_->SetUser(&context, request, &response);
    if (!status.ok()) {
      TIME_LOG << "Error Code " << status.error_code() << ". "
               << status.error_message() << std::endl;
    } else {
      TIME_LOG << "UserId " << user_id << " is deleted!" << std::endl;
    }
  }

  void UpdateUser(int user_id, const std::string& name, const std::string& password, const std::string& email, const std::string& phone) {
    ClientContext context;
    User user;
    user.set_user_id(user_id);
    user.set_name(name);
    user.set_password(password);
    user.set_email(email);
    user.set_phone(phone);
    SetUserRequest request;
    *request.mutable_user() = user;
    request.set_operation(OperationType::UPDATE);
    SetUserResponse response;
    Status status = stub_->SetUser(&context, request, &response);
    if (!status.ok()) {
      TIME_LOG << "Error Code " << status.error_code() << ". "
               << status.error_message() << std::endl;
    } else {
      TIME_LOG << "UserId " << user_id << " is updated!" << std::endl;
    }
  }

  void DeletePost(int post_id) {
    ClientContext context;
    Post post;
    post.set_post_id(post_id);
    SetPostRequest request;
    *request.mutable_post() = post;
    request.set_operation(OperationType::DELETE);
    SetPostResponse response;
    Status status = stub_->SetPost(&context, request, &response);
    if (!status.ok()) {
      TIME_LOG << "Error Code " << status.error_code() << ". "
               << status.error_message() << std::endl;
    } else {
      TIME_LOG << "PostId " << post_id << " is deleted!" << std::endl;
    }
  }

  void CreatePost(int user_id, const std::string& content) {
    ClientContext context;
    Post post;
    post.set_user_id(user_id);
    post.set_content(content);
    SetPostRequest request;
    *request.mutable_post() = post;
    request.set_operation(OperationType::CREATE);
    SetPostResponse response;
    Status status = stub_->SetPost(&context, request, &response);
    if (!status.ok()) {
      TIME_LOG << "Error Code " << status.error_code() << ". "
               << status.error_message() << std::endl;
    } else {
      std::cout << response.post().DebugString();
      TIME_LOG << "PostId " << response.post().post_id() << " is created!" << std::endl;
    }
  }

  void CreatePost(int user_id, const std::string& content, int repost_id) {
    ClientContext context;
    Post post;
    post.set_user_id(user_id);
    post.set_content(content);
    post.set_direct_repost_id(repost_id);
    SetPostRequest request;
    *request.mutable_post() = post;
    request.set_operation(OperationType::CREATE);
    SetPostResponse response;
    Status status = stub_->SetPost(&context, request, &response);
    if (!status.ok()) {
      TIME_LOG << "Error Code " << status.error_code() << ". "
               << status.error_message() << std::endl;
    } else {
      std::cout << response.post().DebugString();
      TIME_LOG << "PostId " << response.post().post_id() << " is created!" << std::endl;
    }
  }

  void DeleteLike(int like_id) {
    ClientContext context;
    Like like;
    like.set_like_id(like_id);
    SetLikeRequest request;
    *request.mutable_like() = like;
    request.set_operation(OperationType::DELETE);
    SetLikeResponse response;
    Status status = stub_->SetLike(&context, request, &response);
    if (!status.ok()) {
      TIME_LOG << "Error Code " << status.error_code() << ". "
               << status.error_message() << std::endl;
    } else {
      TIME_LOG << "LikeId " << like_id << " is deleted!" << std::endl;
    }
  }

  void CreateLike(int post_id, int user_id) {
    ClientContext context;
    Like like;
    like.set_post_id(post_id);
    like.set_user_id(user_id);
    SetLikeRequest request;
    *request.mutable_like() = like;
    request.set_operation(OperationType::CREATE);
    SetLikeResponse response;
    Status status = stub_->SetLike(&context, request, &response);
    if (!status.ok()) {
      TIME_LOG << "Error Code " << status.error_code() << ". "
               << status.error_message() << std::endl;
    } else {
      std::cout << response.like().DebugString();
      TIME_LOG << "LikeId " << response.like().like_id() << " is created!" << std::endl;
    }
  }

  void DeleteComment(int comment_id) {
    ClientContext context;
    Comment comment;
    comment.set_comment_id(comment_id);
    SetCommentRequest request;
    *request.mutable_comment() = comment;
    request.set_operation(OperationType::DELETE);
    SetCommentResponse response;
    Status status = stub_->SetComment(&context, request, &response);
    if (!status.ok()) {
      TIME_LOG << "Error Code " << status.error_code() << ". "
               << status.error_message() << std::endl;
    } else {
      TIME_LOG << "CommentId " << comment_id << " is deleted!" << std::endl;
    }
  }

  void CreateComment(int post_id, int user_id, const std::string& content) {
    ClientContext context;
    Comment comment;
    comment.set_post_id(post_id);
    comment.set_user_id(user_id);
    comment.set_content(content);
    SetCommentRequest request;
    *request.mutable_comment() = comment;
    request.set_operation(OperationType::CREATE);
    SetCommentResponse response;
    Status status = stub_->SetComment(&context, request, &response);
    if (!status.ok()) {
      TIME_LOG << "Error Code " << status.error_code() << ". "
               << status.error_message() << std::endl;
    } else {
      std::cout << response.comment().DebugString();
      TIME_LOG << "CommentId " << response.comment().comment_id() << " is created!" << std::endl;
    }
  }

  void DeleteFollow(int follower_id, int followee_id) {
    ClientContext context;
    User follower;
    User followee;
    follower.set_user_id(follower_id);
    followee.set_user_id(followee_id);
    SetFollowRequest request;
    *request.mutable_follower() = follower;
    *request.mutable_followee() = followee;
    request.set_operation(OperationType::DELETE);
    SetFollowResponse response;
    Status status = stub_->SetFollow(&context, request, &response);
    if (!status.ok()) {
      TIME_LOG << "Error Code " << status.error_code() << ". "
               << status.error_message() << std::endl;
    } else {
      TIME_LOG << "Follow [" << follower_id << " follows " << followee_id << "] is deleted!" << std::endl;
    }
  }

  void CreateFollow(int follower_id, int followee_id) {
    ClientContext context;
    User follower;
    User followee;
    follower.set_user_id(follower_id);
    followee.set_user_id(followee_id);
    SetFollowRequest request;
    *request.mutable_follower() = follower;
    *request.mutable_followee() = followee;
    request.set_operation(OperationType::CREATE);
    SetFollowResponse response;
    Status status = stub_->SetFollow(&context, request, &response);
    if (!status.ok()) {
      TIME_LOG << "Error Code " << status.error_code() << ". "
               << status.error_message() << std::endl;
    } else {
      std::cout << response.follower().DebugString();
      TIME_LOG << "Follow [" << follower_id << " follows " << followee_id << "] is created!" << std::endl;
    }
  }

  void GetFeed(int user_id) {
    ClientContext context;
    User user;
    user.set_user_id(user_id);
    GetFeedRequest request;
    *request.mutable_user() = user;
    GetFeedResponse response;
    Status status = stub_->GetFeed(&context, request, &response);
    if (!status.ok()) {
      TIME_LOG << "Error Code " << status.error_code() << ". "
               << status.error_message() << std::endl;
    } else {
      std::cout << response.DebugString();
      TIME_LOG << "Feed for User " << user_id << " is generated!" << std::endl;
    }
  }

  void GetPost(int post_id) {
    ClientContext context;
    Post post;
    post.set_post_id(post_id);
    GetPostRequest request;
    *request.mutable_post() = post;
    GetPostResponse response;
    Status status = stub_->GetPost(&context, request, &response);
    if (!status.ok()) {
      TIME_LOG << "Error Code " << status.error_code() << ". "
               << status.error_message() << std::endl;
    } else {
      std::cout << response.DebugString();
      TIME_LOG << "PostId " << post_id << " is found!" << std::endl;
    }
  }

  void GetFollowers(int user_id) {
    ClientContext context;
    User user;
    user.set_user_id(user_id);
    GetFollowersRequest request;
    *request.mutable_user() = user;
    GetFollowersResponse response;
    Status status = stub_->GetFollowers(&context, request, &response);
    if (!status.ok()) {
      TIME_LOG << "Error Code " << status.error_code() << ". "
               << status.error_message() << std::endl;
    } else {
      std::cout << response.DebugString();
      TIME_LOG << "Followers for UserId " << user_id << " are found!" << std::endl;
    }
  }

  void GetFollowings(int user_id) {
    ClientContext context;
    User user;
    user.set_user_id(user_id);
    GetFollowingsRequest request;
    *request.mutable_user() = user;
    GetFollowingsResponse response;
    Status status = stub_->GetFollowings(&context, request, &response);
    if (!status.ok()) {
      TIME_LOG << "Error Code " << status.error_code() << ". "
               << status.error_message() << std::endl;
    } else {
      std::cout << response.DebugString();
      TIME_LOG << "Followings for UserId " << user_id << " are found!" << std::endl;
    }
  }

 private:
  std::unique_ptr<Obiew::Stub> stub_;
};

std::string ToLowerCase(const std::string& s) {
  std::string lower(s);
  for (int i = 0; i < s.length(); ++i) {
    if (s[i] <= 'Z' && s[i] >= 'A') lower[i] = s[i] - ('Z' - 'z');
  }
  return lower;
}


void RunClient(const std::string& server_address) {
  TIME_LOG << "Listening to server_address: " << server_address << std::endl;
  // Instantiate the client.
  ObiewClient client(
      grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()));

  TIME_LOG << "Please enter your request: (Separate words with a white space.)"
           << std::endl;
  while (true) {
    std::string query;
    std::getline(std::cin, query);
    if (ToLowerCase(query) == "exit") break;
    std::stringstream querystream(query);
    std::string item;
    std::vector<std::string> args;
    while (std::getline(querystream, item, ' ')) {
      args.push_back(std::move(item));
    }
    if (args.size() == 5 && ToLowerCase(args[0]) == "register") {
      TIME_LOG << "Sending request: Register " << args[1] << std::endl;
      client.Register(args[1], args[2], args[3], args[4]);
    } else if (args.size() == 3 && ToLowerCase(args[0]) == "login") {
      TIME_LOG << "Sending request: LogIn " << args[1]
               << std::endl;
      client.LogIn(args[1], args[2]);
    } else if (args.size() == 2 && ToLowerCase(args[0]) == "getuser") {
      TIME_LOG << "Sending request: GetUser " << args[1]
               << std::endl;
      client.GetUser(std::stoi(args[1]));
    } else if (args.size() == 2 && ToLowerCase(args[0]) == "deleteuser") {
      TIME_LOG << "Sending request: DeleteUser " << args[1]
               << std::endl;
      client.DeleteUser(std::stoi(args[1]));
    } else if (args.size() == 6 && ToLowerCase(args[0]) == "updateuser") {
      TIME_LOG << "Sending request: UpdateUser " << args[1]
               << std::endl;
      client.UpdateUser(std::stoi(args[1]), args[2], args[3], args[4], args[5]);
    } else if (args.size() == 2 && ToLowerCase(args[0]) == "deletepost") {
      TIME_LOG << "Sending request: DeletePost " << args[1]
               << std::endl;
      client.DeletePost(std::stoi(args[1]));
    } else if (args.size() == 3 && ToLowerCase(args[0]) == "createpost") {
      TIME_LOG << "Sending request: CreatePost " << args[2]
               << std::endl;
      client.CreatePost(std::stoi(args[1]), args[2]);
    } else if (args.size() == 4 && ToLowerCase(args[0]) == "createpost") {
      TIME_LOG << "Sending request: CreatePost " << args[2]
               << std::endl;
      client.CreatePost(std::stoi(args[1]), args[2], std::stoi(args[3]));
    } else if (args.size() == 2 && ToLowerCase(args[0]) == "deletelike") {
      TIME_LOG << "Sending request: DeleteLike " << args[1]
               << std::endl;
      client.DeleteLike(std::stoi(args[1]));
    } else if (args.size() == 3 && ToLowerCase(args[0]) == "createlike") {
      TIME_LOG << "Sending request: CreateLike for PostId" << args[1]
               << std::endl;
      client.CreateLike(std::stoi(args[1]), std::stoi(args[2]));
    } else if (args.size() == 2 && ToLowerCase(args[0]) == "deletecomment") {
      TIME_LOG << "Sending request: DeleteComment " << args[1]
               << std::endl;
      client.DeleteComment(std::stoi(args[1]));
    } else if (args.size() == 4 && ToLowerCase(args[0]) == "createcomment") {
      TIME_LOG << "Sending request: CreateComment " << args[3]
               << std::endl;
      client.CreateComment(std::stoi(args[1]), std::stoi(args[2]), args[3]);
    } else if (args.size() == 3 && ToLowerCase(args[0]) == "deletefollow") {
      TIME_LOG << "Sending request: DeleteFollow " << args[1] << " follows " << args[2]
               << std::endl;
      client.DeleteFollow(std::stoi(args[1]), std::stoi(args[2]));
    } else if (args.size() == 3 && ToLowerCase(args[0]) == "createfollow") {
      TIME_LOG << "Sending request: CreateFollow " << args[1] << " follows " << args[2]
               << std::endl;
      client.CreateFollow(std::stoi(args[1]), std::stoi(args[2]));
    } else if (args.size() == 2 && ToLowerCase(args[0]) == "getfeed") {
      TIME_LOG << "Sending request: GetFeed for User " << args[1]
               << std::endl;
      client.GetFeed(std::stoi(args[1]));
    } else if (args.size() == 2 && ToLowerCase(args[0]) == "getpost") {
      TIME_LOG << "Sending request: GetPost " << args[1]
               << std::endl;
      client.GetPost(std::stoi(args[1]));
    } else if (args.size() == 2 && ToLowerCase(args[0]) == "getfollowers") {
      TIME_LOG << "Sending request: GetFollowers " << args[1]
               << std::endl;
      client.GetFollowers(std::stoi(args[1]));
    } else if (args.size() == 2 && ToLowerCase(args[0]) == "getfollowings") {
      TIME_LOG << "Sending request: GetFollowings " << args[1]
               << std::endl;
      client.GetFollowings(std::stoi(args[1]));
    } else {
      TIME_LOG << "Invalid command." << std::endl;
    }
  }
}

int main(int argc, char **argv) {
  // Set server address.
  std::string server_address = "localhost:8000";
  if (argc > 1) {
    server_address = argv[1];
  }
    
  TIME_LOG << "Server address set to " << server_address << std::endl;

  RunClient(server_address);

  return 0;
}
