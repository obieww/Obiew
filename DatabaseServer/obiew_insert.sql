USE Obiew;

-- DELETE FROM Likes WHERE LikeId != 0;
-- DELETE FROM Comments WHERE CommentId != 0;
-- DELETE FROM Follows WHERE UserId != 0;
-- DELETE FROM PostStats WHERE PostId != 0;
-- DELETE FROM UserStats WHERE UserId != 0;
-- DELETE FROM Posts WHERE PostId != 0;
-- DELETE FROM Users WHERE UserId != 0;

INSERT INTO Users (Name) VALUES ("cesca");
INSERT INTO Users (Name) VALUES ("shawn");
INSERT INTO Users (Name) VALUES ("jiahan");

INSERT INTO Follows (UserId, FolloweeId) VALUES (1, 2);
INSERT INTO Follows (UserId, FolloweeId) VALUES (2, 3);
INSERT INTO Follows (UserId, FolloweeId) VALUES (1, 3);


INSERT INTO Posts (UserId, Content) VALUES (1, "My first post.");
INSERT INTO Posts (UserId, Content) VALUES (2, "My first post.");
INSERT INTO Posts (UserId, Content, RepostId) VALUES (1, "Welcome!", 2);
INSERT INTO Posts (UserId, Content, RepostId) VALUES (2, "Thank you!", 3);

SELECT * FROM Users;
SELECT * FROM Follows;
SELECT * FROM UserStats;
SELECT * FROM Posts;
SELECT * FROM PostStats;

-- DELETE FROM Users where UserId = 2;
DELETE FROM Posts WHERE PostId = 4;


