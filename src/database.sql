#reference for the database schema

#CREATE DATABASE Music;
USE Music;
CREATE TABLE Artists 
(
	ArtistID int NOT NULL UNIQUE,
	Name varchar(255),
	PRIMARY KEY (ArtistID)
);
CREATE TABLE Albums
(
	AlbumID int NOT NULL UNIQUE,
	Title varchar(255),
	Releasedate date,
	ArtistID int,
	PRIMARY KEY(AlbumID),
	FOREIGN KEY(ArtistID) REFERENCES Artists(ArtistID)
);
CREATE TABLE Songs
(
	AlbumID int ,
	ArtistID int,
	SongID int not null UNIQUE,
	SongTitle varchar(255),
	TrackNum int not null,
	RunningTime time,
	steamLink varchar(255),
	PRIMARY KEY (SongID),
	FOREIGN KEY (ArtistID) REFERENCES Artists(ArtistID),
	FOREIGN KEY (AlbumID) REFERENCES Albums(AlbumID)
);
CREATE TABLE Users
(
	UserID VARCHAR(36) NOT NULL UNIQUE,/*  #todo: switch to a uuid based system */
	PWHash varchar(1024) NOT NULL,
	Username varchar(50) NOT NULL UNIQUE,
	Email varchar(255),
	Salt varchar(255),
	privilageLevel int default 0,
	PRIMARY KEY (UserId)
);

CREATE TABLE IssuedTokens
(
	UserId VARCHAR(36) UNIQUE,
	Token VARCHAR(36),
	ExpiryDate date,
	ExpiryTime time,
	secret VARCHAR(36),
	FOREIGN KEY (UserID) REFERENCES Users(UserID),
	PRIMARY KEY (Token)
);

CREATE OR REPLACE EVENT RemoveExpiredTokens
ON SCHEDULE EVERY 1 HOUR
DO DELETE FROM IssuedTokens WHERE ExpiryDate > NOW();
