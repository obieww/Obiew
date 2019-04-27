package io.grpc.examples.obiew;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface Server extends Remote {
    User getUser(int userId) throws RemoteException;

    boolean login(String username, String password) throws RemoteException;

    boolean register(String username, String password, String email, String phone, int pic) throws RemoteException;
}
