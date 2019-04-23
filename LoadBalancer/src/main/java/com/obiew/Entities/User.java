package com.obiew.Entities;

import javax.persistence.*;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

@Entity(name = "User")
@Table(name = "user")
public class User {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private long userId;
    private String username;
    private String password;

    @OneToMany(mappedBy = "user")
    private List<Like> likes;
    public User() {}
    public User(String username, String password) {
        this.likes = new ArrayList<>();
        this.username = username;
        this.password = password;
    }

    public long getUserId() {
        return userId;
    }

    public void setUserId(long userId) {
        this.userId = userId;
    }

    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public List<Like> getLikes() {
        return likes;
    }

    public void addLike(Like like) {
        likes.add(like);
        like.setUser(this);
    }

}
