-- CS6650 Final Project

# Create the schema if necessary.
SET sql_mode = '';
CREATE SCHEMA IF NOT EXISTS Obiew;
USE Obiew;

# Drop table if exists.
DROP TABLE IF EXISTS Likes;
DROP TABLE IF EXISTS Comments;
DROP TABLE IF EXISTS Follows;
DROP TABLE IF EXISTS PostStats;
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
    RepostId INT, -- The directly reposted post.
    OriginalPostId INT, -- The very original post in repost chain.
    CONSTRAINT pk_Posts_PostId PRIMARY KEY (PostId),
    CONSTRAINT fk_Posts_UserId FOREIGN KEY (UserId)
		REFERENCES Users(UserId)
        ON UPDATE CASCADE ON DELETE SET NULL,
    CONSTRAINT fk_Posts_RepostId FOREIGN KEY (RepostId)
		REFERENCES Posts(PostId)
        ON UPDATE CASCADE ON DELETE SET NULL,
    CONSTRAINT fk_Posts_OriginalPostId FOREIGN KEY (OriginalPostId)
		REFERENCES Posts(PostId)
        ON UPDATE CASCADE ON DELETE SET NULL
);

# Create Follows.
CREATE TABLE UserStats (
	UserId INT,
    Posts INT DEFAULT 0,
	Following INT DEFAULT 0,
	Followers INT DEFAULT 0,
    CONSTRAINT pk_UserStats_UserId PRIMARY KEY (UserId),
    CONSTRAINT fk_UserStats_UserId FOREIGN KEY (UserId)
		REFERENCES Users(UserId)
        ON UPDATE CASCADE ON DELETE CASCADE
);

# Create PostStats.
CREATE TABLE PostStats (
	PostId INT,
    Reposts INT DEFAULT 0,
    Comments INT DEFAULT 0,
    Likes INT DEFAULT 0,
    CONSTRAINT pk_Posts_PostId PRIMARY KEY (PostId),
    CONSTRAINT fk_Posts_PostId FOREIGN KEY (PostId)
		REFERENCES Posts(PostId)
        ON UPDATE CASCADE ON DELETE CASCADE
);

# Create Follows.
CREATE TABLE Follows (
	UserId INT AUTO_INCREMENT,
	FolloweeId INT,
    Created TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT pk_Follows_UserId_FolloweeId PRIMARY KEY (UserId, FolloweeId),
	CONSTRAINT fk_Follows_UserId FOREIGN KEY (UserId)
		REFERENCES Users(UserId)
        ON UPDATE CASCADE, -- ON DELETE controlled by trigger `before_user_delete`
    CONSTRAINT fk_Follows_FolloweeId FOREIGN KEY (FolloweeId)
		REFERENCES Users(UserId)
        ON UPDATE CASCADE -- ON DELETE controlled by trigger `before_user_delete`
);

-- # Create Reposts.
-- CREATE TABLE Reposts (
-- 	RepostId INT,
-- 	PostId INT,
--     UserId INT,
--     Content TEXT,
--     Created TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
--     CONSTRAINT pk_Reposts_RepostId PRIMARY KEY (RepostId),
--     CONSTRAINT fk_Reposts_RepostId FOREIGN KEY (RepostId)
-- 		REFERENCES Posts(PostId)
--         ON UPDATE CASCADE ON DELETE CASCADE,
--     CONSTRAINT fk_Reposts_PostId FOREIGN KEY (PostId)
-- 		REFERENCES Posts(PostId)
--         ON UPDATE CASCADE ON DELETE SET NULL,
--     CONSTRAINT fk_Reposts_UserId FOREIGN KEY (UserId)
-- 		REFERENCES Users(UserId)
--         ON UPDATE CASCADE ON DELETE SET NULL
-- );

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

CREATE TRIGGER before_user_delete
	BEFORE DELETE ON Users
    FOR EACH ROW
	BEGIN
		DELETE FROM UserStats
        WHERE UserId = OLD.UserId;
        DELETE FROM Follows WHERE UserId = OLD.UserId OR FolloweeId = OLD.UserId;
	END$$

CREATE TRIGGER after_user_insert
	AFTER INSERT ON Users
    FOR EACH ROW
	BEGIN
		INSERT INTO UserStats
        SET UserId = NEW.UserId;
	END$$
    
CREATE TRIGGER before_follow_delete
	BEFORE DELETE ON Follows
    FOR EACH ROW
	BEGIN
		UPDATE UserStats
		SET Following = Following - 1
			WHERE UserId = OLD.UserId;
		UPDATE UserStats
		SET Followers = Followers - 1
			WHERE UserId = OLD.FolloweeId;
	END$$

CREATE TRIGGER after_follow_insert
	AFTER INSERT ON Follows
    FOR EACH ROW
	BEGIN
		UPDATE UserStats
		SET Following = Following + 1
			WHERE UserId = NEW.UserId;
		UPDATE UserStats
		SET Followers = Followers + 1
			WHERE UserId = NEW.FolloweeId;
	END$$
    
CREATE TRIGGER before_post_delete
	BEFORE DELETE ON Posts
    FOR EACH ROW
	BEGIN
		DELETE FROM PostStats
        WHERE PostId = OLD.PostId;
		UPDATE UserStats
		SET Posts = Posts - 1
			WHERE UserId = OLD.UserId;
	END$$

CREATE TRIGGER after_post_insert
	AFTER INSERT ON Posts
    FOR EACH ROW
	BEGIN
		INSERT INTO PostStats
        SET PostId = NEW.PostId;
		UPDATE UserStats
		SET Posts = Posts + 1
			WHERE UserId = NEW.UserId;
	END$$

CREATE TRIGGER before_repost_delete
	BEFORE DELETE ON Posts
    FOR EACH ROW
	BEGIN
		IF OLD.RepostId IS NOT NULL THEN
			UPDATE PostStats
			SET Reposts = Reposts - 1
				WHERE PostId = OLD.RepostId;
		END IF;
		IF OLD.OriginalPostId IS NOT NULL AND OLD.OriginalPostId != OLD.RepostId THEN
			UPDATE PostStats
			SET Reposts = Reposts - 1
				WHERE PostId = OLD.OriginalPostId;
		END IF;
	END$$

CREATE TRIGGER before_repost_insert
	BEFORE INSERT ON Posts
    FOR EACH ROW
	BEGIN
		IF NEW.RepostId IS NOT NULL THEN
			SELECT OriginalPostId FROM Posts 
				WHERE PostId = NEW.RepostId 
                INTO @original_post_id;
            IF @original_post_id IS NULL THEN
				SET NEW.OriginalPostId = NEW.RepostId;
            ELSE
				SET NEW.OriginalPostId = @original_post_id;
            END IF;
			UPDATE PostStats
			SET Reposts = Reposts + 1
                WHERE PostId = NEW.RepostId;
		END IF;
		IF NEW.OriginalPostId IS NOT NULL AND NEW.OriginalPostId != NEW.RepostId THEN
			UPDATE PostStats
			SET Reposts = Reposts + 1
                WHERE PostId = NEW.OriginalPostId;
		END IF;
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
    

