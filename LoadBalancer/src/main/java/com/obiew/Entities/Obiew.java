package com.obiew.Entities;

import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import java.util.List;
import java.util.Objects;

public class Obiew {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private long obiewId;
    private User user;
    private String content;
    private List<Like> likeList;
    private List<Comment> commentList;

    public Obiew(long obiewId, User user, String content, List<Like> likeList, List<Comment> commentList) {
        this.obiewId = obiewId;
        this.user = user;
        this.content = content;
        this.likeList = likeList;
        this.commentList = commentList;
    }

    public long getObiewId() {
        return obiewId;
    }

    public void setObiewId(long obiewId) {
        this.obiewId = obiewId;
    }

    public User getUser() {
        return user;
    }

    public void setUser(User user) {
        this.user = user;
    }

    public String getContent() {
        return content;
    }

    public void setContent(String content) {
        this.content = content;
    }

    public List<Like> getLikeList() {
        return likeList;
    }

    public void setLikeList(List<Like> likeList) {
        this.likeList = likeList;
    }

    public List<Comment> getCommentList() {
        return commentList;
    }

    public void setCommentList(List<Comment> commentList) {
        this.commentList = commentList;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Obiew obiew = (Obiew) o;
        return getObiewId() == obiew.getObiewId() &&
                Objects.equals(getUser(), obiew.getUser()) &&
                Objects.equals(getContent(), obiew.getContent()) &&
                Objects.equals(getLikeList(), obiew.getLikeList()) &&
                Objects.equals(getCommentList(), obiew.getCommentList());
    }

    @Override
    public int hashCode() {
        return Objects.hash(getObiewId(), getUser(), getContent(), getLikeList(), getCommentList());
    }

    @Override
    public String toString() {
        return "Obiew{" +
                "obiewId=" + obiewId +
                ", user=" + user +
                ", content='" + content + '\'' +
                ", likeList=" + likeList +
                ", commentList=" + commentList +
                '}';
    }
}
