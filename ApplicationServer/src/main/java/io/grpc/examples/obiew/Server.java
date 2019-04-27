package io.grpc.examples.obiew;

import java.rmi.Remote;
import java.rmi.RemoteException;
import java.util.List;

public interface Server extends Remote {
    boolean login(String username, String password) throws RemoteException;

    boolean register(String username, String password, String email, String phone, int pic) throws RemoteException;

    String getUser(int userId) throws RemoteException;

    List<String> getObiews(int userId) throws RemoteException;

    List<String> getLikes(int postId) throws RemoteException;

    List<String> getComments(int postId) throws RemoteException;

    boolean postObiew(int userId, String content) throws RemoteException;

    boolean postComment(int obiewId, int userId, String content) throws RemoteException;

    boolean postLike(int obiewId, int userId) throws RemoteException;
}
