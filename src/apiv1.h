#ifndef APIV1_H
#define APIV1_H

#include <Cutelyst/Controller>
#include <QDebug>
#include <QJsonDocument>
#include <QtSql>
#include <QtSql/QSqlDatabase>
#include <QCryptographicHash>
#include <Cutelyst/Plugins/Authentication/authenticationstore.h>
#include <Cutelyst/Plugins/Authentication/credentialhttp.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDate>
#include "passwords.h" //this file is included in my .gitignore so as not to expose passwords to databases etc.
using namespace Cutelyst;

class ApiV1 : public Controller
{
    Q_OBJECT
private:
    QSqlDatabase db;
    void authenticationError(Context *c, const QSqlError qse);
    void verificationFail(Context *c);
	bool verifyUser(QString JWT);
    enum FailReason {PasswordWrong, UsernameWrong, ServerFailure};
public:
    explicit ApiV1(QObject *parent = nullptr);
    ~ApiV1();

    C_ATTR(index, :Path :AutoArgs)
    void index(Context *c);

    C_ATTR(users, :Local :AutoArgs :ActionClass(REST))
    void users(Context *c);

    C_ATTR(users_GET, :Private)
    void users_GET(Context *c);

    C_ATTR(users_POST, :Private)
    void users_POST(Context *c);

    C_ATTR(users_login, :Path('login') :AutoArgs :ActionClass(REST))
    void users_login(Context *c);

    C_ATTR(users_login_POST, :Private)
    void users_login_POST(Context *c);

    C_ATTR(users_uuid, :Path('users') : AutoArgs :ActionClass(REST))
    void users_uuid(Context *c, const QString &uuid);

    C_ATTR(users_uuid_GET, :Private)
    void users_uuid_GET(Context *c, const QString  &uuid);

    C_ATTR(users_uuid_PUT, :Private)
    void users_uuid_PUT(Context *c, const QString &uuid);

    C_ATTR(users_uuid_DELETE, :Private)
    void users_uuid_DELETE(Context *c, const QString &uuid);

    C_ATTR(artist, :Local :AutoArgs :ActionClass(REST))
    void artist(Context *c);

    C_ATTR(artist_search, :Path('Artists') :AutoArgs :ActionClass(REST))
    void artist_search(Context *c, const QString &artist);

    C_ATTR(artist_search_GET, :Private)
    void artist_search_GET(Context *c, const QString &artist);
    
    C_ATTR(login_verify, :Path('login_verify') :AutoArgs :ActionClass(REST))
    void login_verify(Context *c);

    C_ATTR(login_verify_POST, :Private)
    void login_verify_POST(Context *c);
	
	C_ATTR(song_request, :Path('SongRequest') :AutoArgs :ActionClass(REST))
	void song_request(Context *c);
	
	C_ATTR(song_request_POST, :Private)
	void song_request_POST(Context *c);
};

#endif //APIV1_H

