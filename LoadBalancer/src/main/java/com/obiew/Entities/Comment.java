package com.obiew.Entities;

import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import java.util.List;
import java.util.Objects;

public class Comment {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private long commentId;
    private User user;
    private String content;
    private List<Comment> commentList;
    private List<Like> likeList;

    public Comment(long commentId, User user, String content, List<Comment> commentList, List<Like> likeList) {
        this.commentId = commentId;
        this.user = user;
        this.content = content;
        this.commentList = commentList;
        this.likeList = likeList;
    }

    public long getCommentId() {
        return commentId;
    }

    public void setCommentId(long commentId) {
        this.commentId = commentId;
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

    public List<Comment> getCommentList() {
        return commentList;
    }

    public void setCommentList(List<Comment> commentList) {
        this.commentList = commentList;
    }

    public List<Like> getLikeList() {
        return likeList;
    }

    public void setLikeList(List<Like> likeList) {
        this.likeList = likeList;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Comment comment = (Comment) o;
        return getCommentId() == comment.getCommentId() &&
                Objects.equals(getUser(), comment.getUser()) &&
                Objects.equals(getContent(), comment.getContent()) &&
                Objects.equals(getCommentList(), comment.getCommentList()) &&
                Objects.equals(getLikeList(), comment.getLikeList());
    }

    @Override
    public int hashCode() {
        return Objects.hash(getCommentId(), getUser(), getContent(), getCommentList(), getLikeList());
    }

    @Override
    public String toString() {
        return "Comment{" +
                "commentId=" + commentId +
                ", user=" + user +
                ", content='" + content + '\'' +
                ", commentList=" + commentList +
                ", likeList=" + likeList +
                '}';
    }
}

