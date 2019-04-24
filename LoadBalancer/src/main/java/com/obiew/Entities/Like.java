package com.obiew.Entities;

import com.fasterxml.jackson.annotation.JsonIgnore;

import javax.persistence.*;
import java.util.Objects;

@Entity(name = "Likes")
@Table(name = "likes")
public class Like {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private long likeId;
    @OneToOne
    @JoinColumn(name = "userId")
    @JsonIgnore
    private User user;
    @ManyToOne
    @JoinColumn(name = "obiewId")
    @JsonIgnore
    private Obiew obiew;

    public Like() {}

    public long getLikeId() {
        return likeId;
    }

    public void setLikeId(long likeId) {
        this.likeId = likeId;
    }

    public User getUser() {
        return user;
    }

    public void setUser(User user) {
        this.user = user;
    }

    public Obiew getObiew() {
        return obiew;
    }

    public void setObiew(Obiew obiew) {
        this.obiew = obiew;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Like like = (Like) o;
        return getLikeId() == like.getLikeId() &&
                Objects.equals(getUser(), like.getUser()) &&
                Objects.equals(getObiew(), like.getObiew());
    }

    @Override
    public int hashCode() {
        return Objects.hash(getLikeId(), getUser(), getObiew());
    }
}
