package io.grpc.examples.obiew;

import java.rmi.registry.LocateRegistry;

public class Client {
    public static void main(String[] args) throws Exception {
        Server  server = (Server) LocateRegistry.getRegistry("35.236.22.120", 8000).lookup("server");
        server.login("crown", "pass");
    }
}
