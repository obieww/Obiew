package com.obiew.Entities;

import com.fasterxml.jackson.annotation.JsonBackReference;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonManagedReference;
import org.hibernate.annotations.GenericGenerator;

import javax.persistence.*;
import java.io.Serializable;
import java.util.*;

@Entity(name = "User")
@Table(name = "user")
public class User {
    @Id
    @GeneratedValue(generator = "uuid")
    @GenericGenerator(name = "uuid", strategy = "uuid")
    @Column(columnDefinition = "CHAR(32)")
    private String userId;
    private String username;
    private String password;
    @OneToMany(mappedBy = "user", cascade = CascadeType.ALL, orphanRemoval = true)
    @JsonManagedReference
    private List<Obiew> obiewList;
    @ManyToMany
    @JoinTable(
            joinColumns = @JoinColumn(name = "followingId"),
            inverseJoinColumns = @JoinColumn(name = "followerId")
    )
    @JsonManagedReference
    private Set<User> followingList;
    @ManyToMany(mappedBy = "followingList")
    @JsonBackReference
    private Set<User> followerList;

    public User() {
    }

    public User(String username, String password) {
        this.username = username;
        this.password = password;
        this.obiewList = new LinkedList<>();
        this.followerList = new HashSet<>();
        this.followingList = new HashSet<>();
    }

    public void addObiew(Obiew obiew) {
        obiewList.add(obiew);
        obiew.setUser(this);
    }

    public void addFollowing(User following) {
        followingList.add(following);
        following.followerList.add(this);
    }

    public String getUserId() {
        return userId;
    }

    public void setUserId(String userId) {
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

    public List<Obiew> getObiewList() {
        return obiewList;
    }

    public void setObiewList(List<Obiew> obiewList) {
        this.obiewList = obiewList;
    }

    public Set<User> getFollowingList() {
        return followingList;
    }

    public void setFollowingList(Set<User> followingList) {
        this.followingList = followingList;
    }

    public Set<User> getFollowerList() {
        return followerList;
    }

    public void setFollowerList(Set<User> followerList) {
        this.followerList = followerList;
    }


    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        User user = (User) o;
        return Objects.equals(getUsername(), user.getUsername()) &&
                Objects.equals(getPassword(), user.getPassword());
    }

    @Override
    public int hashCode() {
        return Objects.hash(getUserId(), getUsername(), getPassword());
    }
}
