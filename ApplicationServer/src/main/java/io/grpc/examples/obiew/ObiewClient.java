/*
 * Copyright 2015 The gRPC Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package io.grpc.examples.obiew;

import com.google.protobuf.util.JsonFormat;
import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.StatusRuntimeException;

import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * A simple client that requests a greeting from the 
 */
public class ObiewClient implements Server {
  private static final Logger logger = Logger.getLogger(ObiewClient.class.getName());

  private final ManagedChannel channel;
  private final ObiewGrpc.ObiewBlockingStub blockingStub;

  /** Construct client connecting to Obiew server at {@code host:port}. */
  public ObiewClient(String host, int port) {
    this(ManagedChannelBuilder.forAddress(host, port)
        .usePlaintext()
        .build());
  }

  /** Construct client for accessing Obiew server using the existing channel. */
  ObiewClient(ManagedChannel channel) {
    this.channel = channel;
    blockingStub = ObiewGrpc.newBlockingStub(channel);
  }

  public void shutdown() throws InterruptedException {
    channel.shutdown().awaitTermination(5, TimeUnit.SECONDS);
  }

  @Override
  public boolean login(String username, String password) {
    logger.info("login user:" + username + " ...");
    User user = User.newBuilder().setName(username).setPassword(password).build();
    LogInRequest request = LogInRequest.newBuilder().setUser(user).build();
    LogInResponse response;
    try {
      response = blockingStub.logIn(request);
    } catch (StatusRuntimeException e) {
      logger.log(Level.WARNING, "RPC failed: {0}", e.getStatus());
      return false;
    }
    logger.info("Login user success ");
    return response.getUser() != null;
  }

  @Override
  public boolean register(String username, String password, String email, String phone, int pic) {
    logger.info("register user:" + username + " ...");
    User user = User.newBuilder().setName(username).setPassword(password).setEmail(email).setPhone(phone).build();
    RegisterRequest request = RegisterRequest.newBuilder().setUser(user).build();
    RegisterResponse response;
    try {
      response = blockingStub.register(request);
    } catch (StatusRuntimeException e) {
      logger.log(Level.WARNING, "RPC failed: {0}", e.getStatus());
      return false;
    }
    logger.info("Register user success ");
    return response.getUser() != null;
  }

  @Override
  public String getUser(int userId) {
    logger.info("get user" + userId + " ...");
    User user = User.newBuilder().setUserId(userId).build();
    GetUserRequest request = GetUserRequest.newBuilder().setUser(user).build();
    GetUserResponse response;
    try {
      response = blockingStub.getUser(request);
    } catch (StatusRuntimeException e) {
      logger.log(Level.WARNING, "RPC failed: {0}", e.getStatus());
      return null;
    }
    user = response.getUser();

    String userJson = "";
    try {
      userJson = JsonFormat.printer().print(user);
      logger.info("Convert to JSON");
    } catch (Exception e) {
      e.printStackTrace();
      logger.log(Level.WARNING, "json converted failed:\n" );
    }

    return userJson;
  }

  @Override
  public List<String> getObiews(int userId) throws RemoteException {
    logger.info("get obiews of" + userId + " ...");
    User user = User.newBuilder().setUserId(userId).build();
    GetFeedRequest request = GetFeedRequest.newBuilder().setUser(user).build();
    GetFeedResponse response;
    try {
      response = blockingStub.getFeed(request);
    } catch (StatusRuntimeException e) {
      logger.log(Level.WARNING, "RPC failed: {0}", e.getStatus());
      return null;
    }

    List<Post> posts = response.getPostList();
    List<String> postsJson = new ArrayList<>();

    for (Post post :posts) {
      try {
        postsJson.add(JsonFormat.printer().print(post));
      } catch (Exception e) {
        e.printStackTrace();
        logger.log(Level.WARNING, "json converted failed:\n" );
      }
    }

    logger.info("" + posts.size());
    return postsJson;
  }

  @Override
  public List<String> getLikes(int postId) throws RemoteException {
    logger.info("get likes of" + postId + " ...");
    GetLikesRequest request = GetLikesRequest.newBuilder().setPostId(postId).build();
    GetLikesResponse response;
    try {
      response = blockingStub.getLikes(request);
    } catch (StatusRuntimeException e) {
      logger.log(Level.WARNING, "RPC failed: {0}", e.getStatus());
      return null;
    }
    List<Like> likes = response.getLikeList();
    List<String> likesJson = new ArrayList<>();

    for (Like like : likes) {
      try {
        likesJson.add(JsonFormat.printer().print(like));
      } catch (Exception e) {
        e.printStackTrace();
        logger.log(Level.WARNING, "json converted failed:\n" );
      }
    }
    logger.info("" + likesJson.size());
    return likesJson;
  }

  @Override
  public List<String> getComments(int postId) throws RemoteException {
    logger.info("get likes of" + postId + " ...");
    GetCommentsRequest request = GetCommentsRequest.newBuilder().setPostId(postId).build();
    GetCommentsResponse response;
    try {
      response = blockingStub.getComments(request);
    } catch (StatusRuntimeException e) {
      logger.log(Level.WARNING, "RPC failed: {0}", e.getStatus());
      return null;
    }

    List<Comment> comments = response.getCommentList();
    List<String> commentsJson = new ArrayList<>();

    for (Comment comment : comments) {
      try {
        commentsJson.add(JsonFormat.printer().print(comment));
      } catch (Exception e) {
        e.printStackTrace();
        logger.log(Level.WARNING, "json converted failed:\n" );
      }
    }
    logger.info("" + commentsJson.size());
    return commentsJson;
  }

  @Override
  public boolean postObiew(int userId, String content) throws RemoteException {
    logger.info("post obiew of user:" + userId + " ...");
    Post post = Post.newBuilder().setContent(content).setUserId(userId).build();
    SetPostRequest request = SetPostRequest.newBuilder().setPost(post).setOperation(OperationType.CREATE).build();

    try {
      blockingStub.setPost(request);
    } catch (StatusRuntimeException e) {
      logger.log(Level.WARNING, "RPC failed: {0}", e.getStatus());
      return false;
    }
    logger.info("create post");
    return true;
  }

  @Override
  public boolean postComment(int obiewId, int userId, String content) throws RemoteException {
    logger.info("post comment of user:" + obiewId + " ...");
    Comment comment = Comment.newBuilder().setContent(content).setUserId(userId).setPostId(obiewId).build();
    SetCommentRequest request = SetCommentRequest.newBuilder().setComment(comment).setOperation(OperationType.CREATE).build();

    try {
      blockingStub.setComment(request);
    } catch (StatusRuntimeException e) {
      logger.log(Level.WARNING, "RPC failed: {0}", e.getStatus());
      return false;
    }
    logger.info("create post");
    return true;
  }

  @Override
  public boolean postLike(int obiewId, int userId) throws RemoteException {
    logger.info("post like of post:" + obiewId + " ...");
    Like like = Like.newBuilder().setPostId(obiewId).setUserId(userId).setPostId(obiewId).build();
    SetLikeRequest request = SetLikeRequest.newBuilder().setLike(like).setOperation(OperationType.CREATE).build();

    try {
      blockingStub.setLike(request);
    } catch (StatusRuntimeException e) {
      logger.log(Level.WARNING, "RPC failed: {0}", e.getStatus());
      return false;
    }
    logger.info("create post");
    return true;
  }

  /**
   * Greet server. If provided, the first element of {@code args} is the name to use in the
   * greeting.
   */
  public static void main(String[] args) throws Exception {
    String host = "34.83.0.86";
    int port = 8000;
    if (args.length == 1) {
      args = args[0].split(":");
      host = args[0];
      try {
        port = Integer.parseInt(args[1]);
      } catch (Exception e) {
        logger.log(Level.WARNING, "please input \"host:port\", using default host: " + host + ":" + port);
      }
    }
    ObiewClient client = new ObiewClient(host, 8000);
    logger.info("initilize the connection to the server on port:" + port);
    try {
      // Looking up the registry for the remote object
      Server server = (Server) UnicastRemoteObject.exportObject(client, port);
      Registry registry = LocateRegistry.createRegistry(port);
      registry.bind("Server", server);
      logger.info("server ready on localhost:" + port + " using name \'Server\'");
    } catch (Exception e) {
      logger.log(Level.WARNING, "connection to localhost:" + port + " failed:\n" + e.getStackTrace());
    }

//    client.getUser(2);
//    client.register("crownn","pass", "crown@gmail.com", "", 0);
//    client.login("crown", "pass");
//    client.getObiews(2);
//    client.getComments(2);
//    client.getLikes(2);
//    client.postComment(2, 2,"good");
//    client.postLike(2,2);
//    client.postObiew(2, "fibi is fat");
  }
}
