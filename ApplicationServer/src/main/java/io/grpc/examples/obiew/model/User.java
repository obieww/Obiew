package io.grpc.examples.obiew.model;

import java.io.Serializable;
import java.util.List;

public class User implements Serializable {
    private int userId;
    private String username;
    private String password;
    private List<Obiew> obiewList;
    private List<User> followingList;
    private List<User> followerList;

    public User() {
    }

    public int getUserId() {
        return userId;
    }

    public User setUserId(int userId) {
        this.userId = userId;
        return this;
    }

    public String getUsername() {
        return username;
    }

    public User setUsername(String username) {
        this.username = username;
        return this;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public List<Obiew> getObiewList() {
        return obiewList;
    }

    public User setObiewList(List<Obiew> obiewList) {
        this.obiewList = obiewList;
        return  this;
    }

    public List<User> getFollowingList() {
        return followingList;
    }

    public User setFollowingList(List<User> followingList) {
        this.followingList = followingList;
        return this;
    }

    public List<User> getFollowerList() {
        return followerList;
    }

    public User setFollowerList(List<User> followerList) {
        this.followerList = followerList;
        return this;
    }
}
