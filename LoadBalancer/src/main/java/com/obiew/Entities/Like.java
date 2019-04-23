package com.obiew.Entities;

import javax.persistence.*;
import java.util.Objects;

@Entity(name = "Like")
@Table(name = "like")
public class Like {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private long likeId;
    @ManyToOne
    @JoinColumn(name = "userId")
    private User user;

    public Like() {}

    public long getLikeId() {
        return likeId;
    }

    public void setLikeId(long likeId) {
        this.likeId = likeId;
    }

    public void setUser(User user) {
        this.user = user;
    }
}
