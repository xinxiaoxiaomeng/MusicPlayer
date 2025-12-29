--name: create_searchList
create table if not exists searchList(id integer primary key autoincrement, title text, artist text, album text, album_id text, hash text, filePath text);

--name: create_playHistoryList
create table if not exists playHistoryList(id integer primary key autoincrement, title text, artist text, album text, album_id text, like bool, filePath text, hash text);

--name: create_localList
create table if not exists localList(id integer primary key autoincrement, title text, artist text, album text, album_id text, filePath text);

--name: create_likeList
create table if not exists likeList(id integer primary key autoincrement, title text, artist text, album text, album_id text, hash text, filePath text);

--name: get_playHistoryList
select * from playHistoryList;

--name: get_searchList
select * from searchList;

--name: get_localList
select * from localList;

--name: get_likeList
select * from likeList;

--name: clear_searchList
delete from searchList;

--name: clear_playHistoryList
delete from playHisatoryList;

--name: clear_localList
delete from localList;

--name: reset_searchList
delete from sqlite_sequence WHERE name='searchList';

--name: reset_playHistoryList
DELETE FROM sqlite_sequence WHERE name='playHisatoryList';

--name: reset_localList
DELETE FROM sqlite_sequence WHERE name='localList';

--name: save_url
update searchList set filePath = '%1' where hash = '%2';

--name: update_searchList
insert into searchList  (title, artist, album, album_id, hash, filePath) values ('%1', '%2', '%3', '%4', '%5', '');

--name: update_playHistoryList
insert into playHistoryList  (title, artist, album, album_id, hash, filePath) values ('%1', '%2', '%3', '%4', '%5', '%6');

--name: update_likeList
insert into likeList (title, artist, album, album_id, hash, filePath) values ('%1', '%2', '%3', '%4', '%5', '%6');

--name: update_localList
insert into localList (title, artist, album, album_id, hash, filePath, downloadPath) values ('%1', '%2', '%3', '%4', '%5', '%6', '%7');

--name: trackExists_playHistoryList
select * from playHistoryList where title = '%1' and artist = '%2';

--name: trackExists_likeList
select * from likeList where title = '%1' and artist = '%2';

--name: deleteTrack_playHistoryList
delete from playHistoryList where title = '%1' and artist = '%2';

--name: deleteTrack_likeList
delete from likeList where title = '%1' and artist = '%2';

--name: deleteTrack_searchList
delete from searchList where title = '%1' and artist = '%2';

--name: deleteTrack_localList
delete from localList where title = '%1' and artist = '%2';
