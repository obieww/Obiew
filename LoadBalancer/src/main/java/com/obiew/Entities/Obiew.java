package com.obiew.Entities;

import com.fasterxml.jackson.annotation.JsonIgnore;
import org.hibernate.annotations.GenericGenerator;

import javax.persistence.*;
import java.util.LinkedList;
import java.util.List;
import java.util.Objects;

@Entity(name = "Obiew")
@Table(name = "obiew")
public class Obiew {
    @Id
    @GeneratedValue(generator = "uuid")
    @GenericGenerator(name = "uuid", strategy = "uuid")
    @Column(columnDefinition = "CHAR(32)")
    private String obiewId;
    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "userId")
    @JsonIgnore
    private User user;
    private String content;
    @OneToMany(mappedBy = "obiew", cascade = CascadeType.ALL, orphanRemoval = true)
    private List<Like> likeList;
    @OneToMany(mappedBy = "parent", cascade = CascadeType.ALL, orphanRemoval = true)
    private List<Obiew> commentList;
    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "parentId")
    @JsonIgnore
    private Obiew parent;

    public Obiew() {}

    public Obiew(String content) {
        this.content = content;
        this.likeList = new LinkedList<>();
        this.commentList = new LinkedList<>();
    }
    public void addComment(Obiew comment) {
        comment.setParent(this);
        commentList.add(comment);
    }
    public void addLike(Like like) {
        like.setObiew(this);
        likeList.add(like);
    }

    public String getObiewId() {
        return obiewId;
    }

    public void setObiewId(String obiewId) {
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

    public List<Obiew> getCommentList() {
        return commentList;
    }

    public void setCommentList(List<Obiew> commentList) {
        this.commentList = commentList;
    }

    public Obiew getParent() {
        return parent;
    }

    public void setParent(Obiew parent) {
        this.parent = parent;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Obiew obiew = (Obiew) o;
        return Objects.equals(getObiewId(), obiew.getObiewId()) &&
                Objects.equals(getContent(), obiew.getContent()) &&
                Objects.equals(getLikeList(), obiew.getLikeList()) &&
                Objects.equals(getCommentList(), obiew.getCommentList());
    }

    @Override
    public int hashCode() {
        return Objects.hash(getObiewId(), getContent(), getLikeList(), getCommentList());
    }
}
