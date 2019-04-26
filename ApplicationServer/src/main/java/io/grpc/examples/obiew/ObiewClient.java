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
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * A simple client that requests a greeting from the 
 */
public class ObiewClient {
  private static final Logger logger = Logger.getLogger(ObiewClient.class.getName());

  private final ManagedChannel channel;
  private final ObiewGrpc.ObiewBlockingStub blockingStub;

  /** Construct client connecting to Obiew server at {@code host:port}. */
  public ObiewClient(String host, int port) {
    this(ManagedChannelBuilder.forAddress(host, port)
        // Channels are secure by default (via SSL/TLS). For the example we disable TLS to avoid
        // needing certificates.
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
  public void greet(int userId) {
    logger.info("Will try to greet " + userId + " ...");
    User user = User.newBuilder().setUserId(userId).build();
    GetUserRequest request = GetUserRequest.newBuilder().setUser(user).build();
    GetUserResponse response;
    try {
      response = blockingStub.getUser(request);
    } catch (StatusRuntimeException e) {
      logger.log(Level.WARNING, "RPC failed: {0}", e.getStatus());
      return;
    }
    logger.info("Greeting: " + response.getUser());
  }

  /**
   * Greet server. If provided, the first element of {@code args} is the name to use in the
   * greeting.
   */
  public static void main(String[] args) throws Exception {
    ObiewClient client = new ObiewClient("34.83.222.184", 8000);
    try {
      /* Access a service running on the local machine on port 50051 */
      client.greet(2);
    } finally {
      client.shutdown();
    }
  }
}
