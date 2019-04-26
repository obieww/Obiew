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
using obiew::GetFeedRequest;
using obiew::GetFeedResponse;
using obiew::GetPostRequest;
using obiew::GetPostResponse;
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
    // Context for the client.
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
    // Context for the client.
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
    // Context for the client.
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
    // Context for the client.
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
    // Context for the client.
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
    // Context for the client.
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
    // Context for the client.
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
      std::cout << "Post created!"<< std::endl << response.post().DebugString();
    }
  }

  void CreatePost(int user_id, const std::string& content, int repost_id) {
    // Context for the client.
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
      std::cout << "Post created!"<< std::endl << response.post().DebugString();
    }
  }

  void GetFeed(int user_id) {
    // Context for the client.
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
    // Context for the client.
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
    } else if (args.size() == 2 && ToLowerCase(args[0]) == "getfeed") {
      TIME_LOG << "Sending request: GetFeed for User " << args[1]
               << std::endl;
      client.GetFeed(std::stoi(args[1]));
    } else if (args.size() == 2 && ToLowerCase(args[0]) == "getpost") {
      TIME_LOG << "Sending request: GetPost " << args[1]
               << std::endl;
      client.GetPost(std::stoi(args[1]));
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
