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

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.StatusRuntimeException;

import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.Logger;

import com.fasterxml.jackson.databind.ObjectMapper;

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

  /** Say hello to server. */
  @Override
  public User getUser(int userId) {
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
    logger.info("Get user: " + user);
//    io.grpc.examples.obiew.model.User serilizedUser = new io.grpc.examples.obiew.model.User()
//            .setUserId(userId).setUsername(user.getName()).
//    ObjectMapper mapper = new ObjectMapper();
//    String jsonString = "";
//    try {
//      jsonString = mapper.writeValueAsString(user);
//      logger.info("\n" + mapper.writerWithDefaultPrettyPrinter().writeValueAsString(user));
//    } catch (Exception e) {
//      e.printStackTrace();
//      logger.log(Level.WARNING, "parse json failed");
//    }
    return user;
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
  }
}
