USE Obiew;

INSERT INTO Users (Name, Password, Email, Phone) VALUES ("cesca", AES_ENCRYPT('pass', UNHEX(SHA2('My secret passphrase',512))), "cesca@gmail.com", "408-123-0001");
INSERT INTO Users (Name, Password, Email, Phone) VALUES ("fibi", AES_ENCRYPT('pass', UNHEX(SHA2('My secret passphrase',512))), "fibi@gmail.com", "408-123-0002");
INSERT INTO Users (Name, Password, Email, Phone) VALUES ("hugh", AES_ENCRYPT('pass', UNHEX(SHA2('My secret passphrase',512))), "hugh@gmail.com", "408-123-0003");
INSERT INTO Users (Name, Password, Email, Phone) VALUES ("crown", AES_ENCRYPT('pass', UNHEX(SHA2('My secret passphrase',512))), "crown@gmail.com", "408-123-0004");
INSERT INTO Users (Name, Password, Email, Phone) VALUES ("fanfan", AES_ENCRYPT('pass', UNHEX(SHA2('My secret passphrase',512))), "fanfan@gmail.com", "408-123-0005");

SELECT * FROM Users WHERE AES_ENCRYPT('pass', UNHEX(SHA2('My secret passphrase',512)))=Password;

INSERT INTO Follows (UserId, FolloweeId) VALUES (1, 2);
INSERT INTO Follows (UserId, FolloweeId) VALUES (2, 3);
INSERT INTO Follows (UserId, FolloweeId) VALUES (1, 3);


INSERT INTO Posts (UserId, Content) VALUES (1, "My first post.");
INSERT INTO Posts (UserId, Content) VALUES (2, "My first post.");
INSERT INTO Posts (UserId, Content, RepostId) VALUES (1, "Welcome!", 2);
INSERT INTO Posts (UserId, Content, RepostId) VALUES (2, "Thank you!", 3);
INSERT INTO Posts (UserId, Content, RepostId) VALUES (3, "Haha :)", 4);
INSERT INTO Posts (UserId, Content, RepostId) VALUES (4, "Haha :)", 1);


INSERT INTO PostComments (PostId, UserId, Content) VALUES (2, 1, "Nice");
INSERT INTO PostComments (PostId, UserId, Content) VALUES (2, 3, "Great");
INSERT INTO PostComments (PostId, UserId, Content) VALUES (3, 2, "Haha");
INSERT INTO PostComments (PostId, UserId, Content) VALUES (1, 2, "Hello");

INSERT INTO PostLikes (PostId, UserId) VALUES (3, 1);
INSERT INTO PostLikes (PostId, UserId) VALUES (2, 1);
INSERT INTO PostLikes (PostId, UserId) VALUES (3, 2);
INSERT INTO PostLikes (PostId, UserId) VALUES (1, 3);

SELECT * FROM Users;
SELECT * FROM Follows;
SELECT * FROM UserStats;
SELECT * FROM Posts;
SELECT * FROM PostStats;
SELECT * FROM PostComments;
SELECT * FROM PostLikes;

SELECT * FROM Posts WHERE UserId IN (SELECT FolloweeId FROM Follows WHERE UserId=1) ORDER BY Created DESC;


-- DELETE FROM Users where UserId = 2;
-- DELETE FROM Posts WHERE PostId = 4;

