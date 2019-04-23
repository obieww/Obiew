-- CS6650 Final Project

# Create the schema if necessary.
SET sql_mode = '';
CREATE SCHEMA IF NOT EXISTS Obiew;
USE Obiew;

# Drop table if exists.
DROP TABLE IF EXISTS Likes;
DROP TABLE IF EXISTS Comments;
DROP TABLE IF EXISTS Reposts;
DROP TABLE IF EXISTS Follows;
DROP TABLE IF EXISTS UserStats;
DROP TABLE IF EXISTS Posts;
DROP TABLE IF EXISTS Users;

# Create Users.
CREATE TABLE Users (
	UserId INT AUTO_INCREMENT,
	Password VARCHAR(255),
	Name VARCHAR(255),
    ProfilePicture INT,
	Email VARCHAR(255),
	Phone VARCHAR(20),
	CONSTRAINT pk_Users_UserId PRIMARY KEY (UserId)
);

# Create Posts.
CREATE TABLE Posts (
	PostId INT AUTO_INCREMENT,
    UserId INT,
    Content TEXT,
    ImageId INT,
    Created TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    IsRepost BOOL,
    Reposts INT,
    Comments INT,
    Likes INT,
    CONSTRAINT pk_Posts_PostId PRIMARY KEY (PostId),
    CONSTRAINT fk_Posts_UserId FOREIGN KEY (UserId)
		REFERENCES Users(UserId)
        ON UPDATE CASCADE ON DELETE SET NULL
);

# Create Follows.
CREATE TABLE UserStats (
	UserId INT,
    Post INT,
	Follwing INT,
	Follwers INT,
    CONSTRAINT pk_UserStats_UserId PRIMARY KEY (UserId),
    CONSTRAINT fk_UserStats_UserId FOREIGN KEY (UserId)
		REFERENCES Users(UserId)
        ON UPDATE CASCADE ON DELETE CASCADE
);

# Create Follows.
CREATE TABLE Follows (
	UserId INT AUTO_INCREMENT,
	FolloweeId INT,
    Created TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT pk_Follows_UserId PRIMARY KEY (UserId),
    CONSTRAINT fk_Follows_FolloweeId FOREIGN KEY (FolloweeId)
		REFERENCES Users(UserId)
        ON UPDATE CASCADE ON DELETE CASCADE
);

# Create Reposts.
CREATE TABLE Reposts (
	RepostId INT,
	PostId INT,
    UserId INT,
    Content TEXT,
    Created TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT pk_Reposts_RepostId PRIMARY KEY (RepostId),
    CONSTRAINT fk_Reposts_RepostId FOREIGN KEY (RepostId)
		REFERENCES Posts(PostId)
        ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT fk_Reposts_PostId FOREIGN KEY (PostId)
		REFERENCES Posts(PostId)
        ON UPDATE CASCADE ON DELETE SET NULL,
    CONSTRAINT fk_Reposts_UserId FOREIGN KEY (UserId)
		REFERENCES Users(UserId)
        ON UPDATE CASCADE ON DELETE SET NULL
);

# Create Comments.
CREATE TABLE Comments (
	CommentId INT AUTO_INCREMENT,
	PostId INT,
    UserId INT,
    Content TEXT,
    Created TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT pk_Comments_CommentId PRIMARY KEY (CommentId),
    CONSTRAINT fk_Comments_UserId FOREIGN KEY (UserId)
		REFERENCES Users(UserId)
        ON UPDATE CASCADE ON DELETE SET NULL,
    CONSTRAINT fk_Comments_PostId FOREIGN KEY (PostId)
		REFERENCES Posts(PostId)
        ON UPDATE CASCADE ON DELETE CASCADE
);

# Create Likes.
CREATE TABLE Likes (
	LikeId INT AUTO_INCREMENT,
	PostId INT,
    UserId INT,
    Type TEXT,
    Created TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT pk_Likes_LikeId PRIMARY KEY (LikeId),
    CONSTRAINT fk_Likes_UserId FOREIGN KEY (UserId)
		REFERENCES Users(UserId)
        ON UPDATE CASCADE ON DELETE SET NULL,
    CONSTRAINT fk_Likes_PostId FOREIGN KEY (PostId)
		REFERENCES Posts(PostId)
        ON UPDATE CASCADE ON DELETE CASCADE
);

# Create Triggers
DELIMITER $$
CREATE TRIGGER before_follow_delete
	BEFORE DELETE ON Follows
    FOR EACH ROW
	BEGIN
		UPDATE UserStats
		SET Follwing = Follwing - 1
			WHERE UserId = OLD.UserId;
		UPDATE UserStats
		SET Follwers = Follwers - 1
			WHERE UserId = OLD.FolloweeId;
	END$$

CREATE TRIGGER after_follow_insert
	AFTER INSERT ON Follows
    FOR EACH ROW
	BEGIN
		UPDATE UserStats
		SET Follwing = Follwing + 1
			WHERE UserId = NEW.UserId;
		UPDATE UserStats
		SET Follwers = Follwers + 1
			WHERE UserId = NEW.FolloweeId;
	END$$
    
CREATE TRIGGER before_post_delete
	BEFORE DELETE ON Posts
    FOR EACH ROW
	BEGIN
		UPDATE UserStats
		SET Posts = Posts - 1
			WHERE UserId = OLD.UserId;
	END$$

CREATE TRIGGER after_post_insert
	AFTER INSERT ON Posts
    FOR EACH ROW
	BEGIN
		UPDATE UserStats
		SET Posts = Posts + 1
			WHERE UserId = NEW.UserId;
	END$$
    
CREATE TRIGGER before_repost_delete
	BEFORE DELETE ON Reposts
    FOR EACH ROW
	BEGIN
		UPDATE Posts
		SET Reposts = Reposts - 1
			WHERE PostId = OLD.PostId;
	END$$

CREATE TRIGGER after_repost_insert
	AFTER INSERT ON Reposts
    FOR EACH ROW
	BEGIN
		UPDATE Posts
		SET Reposts = Reposts + 1
			WHERE PostId = NEW.PostId;
	END$$
    
CREATE TRIGGER before_comments_delete
	BEFORE DELETE ON Comments
    FOR EACH ROW
	BEGIN
		UPDATE Posts
		SET Comments = Comments - 1
			WHERE PostId = OLD.PostId;
	END$$

CREATE TRIGGER after_comments_insert
	AFTER INSERT ON Comments
    FOR EACH ROW
	BEGIN
		UPDATE Posts
		SET Comments = Comments + 1
			WHERE PostId = NEW.PostId;
	END$$
    
CREATE TRIGGER before_likes_delete
	BEFORE DELETE ON Likes
    FOR EACH ROW
	BEGIN
		UPDATE Posts
		SET Likes = Likes - 1
			WHERE PostId = OLD.PostId;
	END$$

CREATE TRIGGER after_likes_insert
	AFTER INSERT ON Likes
    FOR EACH ROW
	BEGIN
		UPDATE Posts
		SET Likes = Likes + 1
			WHERE PostId = NEW.PostId;
	END$$

DELIMITER ;
    

