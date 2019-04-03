# dmstrsrv
this is a project to create a music streaming service ala spotify;
This part of it is a restful server designed to allow access to a database of music.
It makes use of a C++ framework called cutelyst2 to provide this service, and requires a mysql database for which a sql schema is provided.

The only file not included in here is passwords.h, which is a single class with a single static constexpr string_view containing the password to my database.

launch from build directory with cutelyst2 -r --server -p 3000 --app-file $PWD/src/libdmstrsrv.so
test with curl for now, until it's ready for client
eg:
curl -H "Content-Type: application/json" -X POST -d '{"Username": "someone_test_5", "PWHash": "5eacbf1d12bcc89f9d5a2639180e8aa55b26e35ce3e26d2a4177e61f3109f5ef1d045207510589e99b5fad4cfef2c144e031c1fcba070094863ed874028e3bd1" }' http://localhost:3000/api/v1/login

3 URLs are currently set up, api/v1/users for registration, api/v1/login for login, api/v1/artist_search for artist search.
a fourth is WIP at present (api/v1/login_verify)

-Login_verify should be done before a login prompt to confirm that the user isn't already logged in.
happy hacking
