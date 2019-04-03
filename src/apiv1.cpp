#include "apiv1.h"

using namespace Cutelyst;

ApiV1::ApiV1(QObject *parent) : Controller(parent)
{
	db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName("localhost");
	db.setDatabaseName("Music");
	if( !db.open("dbuser",Passwords::dbPassword().data() ) )
	{
		qDebug() << "Database connection failed";
//		qDebug() << QSqlDatabase::drivers();
		qDebug() << db.lastError().text();
		qFatal("DATABASE CONNECTION FAILED!!");
	}
	qDebug() << "Connected!";
}

ApiV1::~ApiV1()
{
}

void ApiV1::index(Context *c)
{
	c->response()->body() = "Matched Controller::ApiV1 in ApiV1.";
}

void ApiV1::users_GET(Context *c)
{
	qDebug() << Q_FUNC_INFO;
	//tutorial i'm following says this should return list of users.
	//why though?
}
void ApiV1::users(Context *c)
{
	qDebug() << Q_FUNC_INFO;
}
void ApiV1::users_POST(Context *c)
{
	qDebug() << Q_FUNC_INFO;
	const QString uuid = QUuid::createUuid().toString()
		.remove(QLatin1Char('{'))
		.remove(QLatin1Char('}'));
	users_uuid_PUT(c,uuid);
}
void ApiV1::users_uuid(Context *c, const QString &uuid)
{
        qDebug() << Q_FUNC_INFO;
}
void ApiV1::users_uuid_GET(Context *c, const QString &uuid)
{
	//return Email and username. PWHash should never be sent over an unsecure connection, and there's no need to send uuid.
	qDebug() << Q_FUNC_INFO << uuid;
	QSqlQuery query;
	QString s = "SELECT Username, Email FROM Users WHERE UserID = :valA";
	query.prepare(s);
	query.bindValue(":valA","'"+uuid+"'");
	query.exec();
	bool found = false;
	qDebug() << query.executedQuery();
	while(query.next())
	{	found = true;
		c->response()->setJsonObjectBody({
							{QStringLiteral("Username"), query.value("Username").toString()},
							{QStringLiteral("Email"), query.value("Email").toString()}
						 });
	}
	if(!found)
	{
		qDebug() << uuid;
		qDebug() << query.boundValue(":valA");
		qDebug() << query.lastError().text();
	}
	qDebug() << "response size is " << c->response()->size();
}
void ApiV1::users_uuid_PUT(Context *c, const QString &uuid)
{
	qDebug() << Q_FUNC_INFO << uuid;
	const QJsonDocument doc = c->request()->bodyData().toJsonDocument();
	const QJsonObject obj = doc.object();
	//salt should be random, so look into Qt or use std::random functions to get a large random number
	//finally, we need to take a hash of the pwhash concatanated with the salt and save that as the pwhash in the db

	QSqlQuery query;
	//query.exec("SELECT * FROM Artists");//placeholder.
	//TODO: investigate what invalid errors look like, and add salt!
	QString s = "INSERT INTO Users (UserID, PWHash, Username, Email,Salt) VALUES (:valA, :valB, :valC, :valD, :valE);";
	QString salt = QUuid::createUuid().toString();
	QString tmp = obj.value(QStringLiteral("PWHash")).toString().append(salt);
	QCryptographicHash hasher(QCryptographicHash::Sha3_512);
	hasher.addData(tmp.toStdString().c_str(),tmp.size());
	auto saltedHash = hasher.result();
	//qDebug() << QCryptographicHash::hash(QByteArray::fromStdString(tmp.toStdString()),QCryptographicHash::Sha3_512).size();
	query.prepare(s);
	query.bindValue(":valA",uuid);//on the offchance this needs escaping.
	query.bindValue(":valB",saltedHash.toHex());
	query.bindValue(":valC",obj.value(QStringLiteral("Username")));
	query.bindValue(":valD",obj.value(QStringLiteral("Email")));
	query.bindValue(":valE",salt);
	query.exec();
	query.finish();
	if(query.lastError().type() != QSqlError::NoError)
	{
		qDebug() << query.lastError().text();
		//TODO: add error responses for clashes in email, username.
		c->response()->setJsonObjectBody({
							{QStringLiteral("Response"),query.lastError().text()}
						});
		
	}
	else 
	{
		//return success
		c->response()->setJsonObjectBody({
							{QStringLiteral("Response"),QStringLiteral("Account created")}
						});
	}

}
void ApiV1::users_uuid_DELETE(Context *c, const QString &uuid)
{
        qDebug() << Q_FUNC_INFO << uuid;
}
void ApiV1::users_login(Context *c)
{
	qDebug() << Q_FUNC_INFO;
}
void ApiV1::users_login_POST(Context *c)
{
	/*	TODO:
		this works but is not as fast as it should be because of the additional query
		which is ultimately unneccesary. I should refactor it at some point to be more efficient
		and additionally remove allocations which doubtlessly take up time. I should however be sure
		to clear strings stored at a class level which might otherwise be vulnerable.
	*/
	qDebug() << Q_FUNC_INFO;
	const QJsonDocument doc = c->request()->bodyData().toJsonDocument();
	const QJsonObject obj = doc.object();
	QSqlQuery query;
	QString t = "SELECT salt FROM Users WHERE Username =:valA";
	QSqlQuery first;
	first.prepare(t);
	first.bindValue(":valA",obj.value(QStringLiteral("Username")));
	first.exec();
	if(first.size()==1)
	{
		first.next();
		QString tmp = obj.value(QStringLiteral("PWHash")).toString().append(first.value("salt").toString());
		QCryptographicHash hasher(QCryptographicHash::Sha3_512);
		hasher.addData(tmp.toStdString().c_str(),tmp.length());
		auto saltedHash = hasher.result().toHex();
		QString s = "SELECT * FROM Users WHERE Username = :valA AND PWHash = :valB;";
		query.prepare(s);
		query.bindValue(":valA",obj.value(QStringLiteral("Username")));
		query.bindValue(":valB",saltedHash);//QString::fromStdString(saltedHash.toStdString()) );
		query.exec();
		if(query.size() > 0)
		{//login successful - insert the token into the database and then reply it to the user. 
			query.next();
			QString token = QUuid::createUuid().toString().remove(QLatin1Char('{')).remove(QLatin1Char('}'));
			QString secret = QUuid::createUuid().toString().remove(QLatin1Char('{')).remove(QLatin1Char('}'));
			QSqlQuery query2;
			QDate expiryDate = QDate::currentDate().addDays(1);
			t = "INSERT INTO IssuedTokens (UserId, Token, expiryDate, secret) VALUES (:valA, :valB, :valC, :valD);";
			query2.prepare(t);
			//qDebug() << query.value("UserId").toString();
			query2.bindValue(":valA",query.value("UserID").toString());
			query2.bindValue(":valB",token);
			query2.bindValue(":valC",expiryDate);
			query2.bindValue(":valD",secret);
			query2.exec();
			switch(query2.lastError().type())
			{
				case QSqlError::NoError : //qDebug() << "no error";
							  break;
				case QSqlError::StatementError : qDebug() << "Statement error: " << query2.lastError().text();
								 authenticationError(c,query2.lastError());
								 return;
								 break;
				case QSqlError::TransactionError : qDebug() << "Transaction error:" << query2.lastError().text();
								   authenticationError(c,query2.lastError());
								   return;
								   break;
				case QSqlError::UnknownError : qDebug() << "Unknown Error!";
							       authenticationError(c,query2.lastError());
							       return;
							       break;
			}
			qDebug() << "User " << query.value("UserID").toString() <<" logged in";
			//redone to support jwt standard.
			QJsonObject header  {
							{QStringLiteral("typ"),QStringLiteral("JWT")},
							{QStringLiteral("alg"),QStringLiteral("HS512")}
					    };
			QJsonObject payload {
							{QStringLiteral("UserID"), query.value("UserID").toString()},
							{QStringLiteral("jti"), token},
							{QStringLiteral("exp"),expiryDate.toString(Qt::ISODate)}
					    };
			QJsonDocument h(header);
			QJsonDocument p(payload);
			hasher.reset();
			hasher.addData(h.toJson().toBase64(QByteArray::Base64UrlEncoding));
			hasher.addData(p.toJson().toBase64(QByteArray::Base64UrlEncoding));
			hasher.addData(secret.toStdString().c_str(),secret.size());
			QString reply = h.toJson().toBase64(QByteArray::Base64UrlEncoding)
			 + "." + p.toJson().toBase64(QByteArray::Base64UrlEncoding)+"."+hasher.result().toBase64(QByteArray::Base64UrlEncoding);
			c->response()->setBody(reply);
		}
		else
		{
			//error handling
			authenticationError(c, query.lastError());
			qDebug() << "Wrong password";
		}

	}
	else
	{
		//error handling
		authenticationError(c,first.lastError());
		qDebug() << "Username wrong";
	}
}

void ApiV1::authenticationError(Context *c, const QSqlError qe)
{
	//handle a failure in the DB
	qDebug() << "Login Failed";
	qDebug() << qe.text();
	c->response()->setJsonObjectBody({
						{QStringLiteral("Response"), QStringLiteral("login failed")}
					});
}
void ApiV1::artist(Context *c)
{
	qDebug() << Q_FUNC_INFO;
}
void ApiV1::artist_search(Context *c, const QString &artist)
{
	qDebug() << Q_FUNC_INFO;
}
void ApiV1::artist_search_GET(Context *c, const QString &artist)
{
	qDebug() << Q_FUNC_INFO;
	QSqlQuery query;
	//TODO: there are better ways to search. this will do for testing.
	QString s = "SELECT Name FROM Artists WHERE (Name LIKE :val1);";
	query.prepare(s);
	query.bindValue(":val1",artist);
	query.exec();
	if(query.size() == 0)
	{	//TODO: actually return this in json format to the client
		qDebug() << "No results found.";
		return;
	}
	QMap<QString,QVariant> tmp;
	while(query.next())
	{
		tmp.insert("Name",query.value("Name").toString());
	}
	c->response()->setJsonObjectBody(QJsonObject::fromVariantMap(tmp));
}
void ApiV1::login_verify(Context *c)
{

}
void ApiV1::login_verify_POST(Context *c)
{
	const QJsonDocument doc = c->request()->bodyData().toJsonDocument();
	const QJsonObject obj = doc.object();
	QString q = "SELECT * FROM IssuedTokens WHERE UserId = :valA AND Token = :valB";
	QString jwt = obj.value(QStringLiteral("JWT")).toString();
	//format: {"JWT":"(header).(payload).(signature)"}
	QStringList jwtl = jwt.split('.');
	if(jwtl.size() != 3)
	{
		verificationFail(c);
		qDebug() << "invalid token";
		return;
	}
	QByteArray hba;
	hba.append(jwtl.at(0));
	hba = QByteArray::fromBase64(hba,QByteArray::Base64UrlEncoding);
//	QJsonDocument h = QJsonDocument::fromBinaryData(QByteArray::fromBase64(QByteArray::fromStdString(jwtl.at(0).toStdString()),QByteArray::Base64UrlEncoding));
	QJsonDocument h = QJsonDocument::fromJson(hba);
	hba.clear();
	hba.append(jwtl.at(1));
	hba = QByteArray::fromBase64(hba,QByteArray::Base64UrlEncoding);
	QJsonDocument p = QJsonDocument::fromJson(hba);
//	QJsonDocument p = QJsonDocument::fromBinaryData(QByteArray::fromBase64(QByteArray::fromStdString(jwtl.at(1).toStdString()),QByteArray::Base64UrlEncoding));
	QJsonObject header = h.object();
	QJsonObject payload = p.object();
	//todo:test this.

	qDebug() << header.value("typ");
	qDebug() << payload.value("exp");
	qDebug() << payload.value("jti").toString();
	QSqlQuery query;
	query.prepare(q);
	query.bindValue(":valA",payload.value("UserID").toString());
	query.bindValue(":valB",payload.value("jti").toString());
	query.exec();
	if(query.size() > 0)
	{
		query.next();
		QString secret = query.value("secret").toString();
		QCryptographicHash hasher(QCryptographicHash::Sha3_512);
		hasher.addData(h.toJson().toBase64(QByteArray::Base64UrlEncoding));
		hasher.addData(p.toJson().toBase64(QByteArray::Base64UrlEncoding));
		hasher.addData(secret.toStdString().c_str(),secret.size());
		if(hasher.result().toBase64(QByteArray::Base64UrlEncoding) == jwtl.at(2))
		{//authentication successful
			c->response()->setJsonObjectBody({
								{QStringLiteral("Response"),QStringLiteral("0")}
							});
			return;
		}
		else
		{
			qDebug() << "hashes don't match. possible forged token";
		}	
	}
	else
	{
		qDebug() << query.lastError().text();
		qDebug() << "token refers to nonexistent userid:" << payload.value("UserID").toString() ;
	}
	qDebug() << "verification failed";
	verificationFail(c);
	//QSqlQuery query;
	//query.prepare(q);
	//query.bindValue(":valA",obj.value(QStringLiteral("JWT")));
	//query.exec();
	/*
	if(query.size() > 0)
	{
		//token valid-allowed to proceed
	}
	else
	{
		verificationFail(c);
		return;
	}*/
}
bool verifyUser(QString JWT)
{
	//const QJsonDocument doc = c->request()->bodyData().toJsonDocument();
	//const QJsonObject obj = doc.object();
	QString q = "SELECT * FROM IssuedTokens WHERE UserId = :valA AND Token = :valB";
	QString jwt = JWT;
	//format: {"JWT":"(header).(payload).(signature)"}
	QStringList jwtl = jwt.split('.');
	if(jwtl.size() != 3)
	{
		verificationFail(c);
		qDebug() << "invalid token";
		return;
	}
	QByteArray hba;
	hba.append(jwtl.at(0));
	hba = QByteArray::fromBase64(hba,QByteArray::Base64UrlEncoding);
//	QJsonDocument h = QJsonDocument::fromBinaryData(QByteArray::fromBase64(QByteArray::fromStdString(jwtl.at(0).toStdString()),QByteArray::Base64UrlEncoding));
	QJsonDocument h = QJsonDocument::fromJson(hba);
	hba.clear();
	hba.append(jwtl.at(1));
	hba = QByteArray::fromBase64(hba,QByteArray::Base64UrlEncoding);
	QJsonDocument p = QJsonDocument::fromJson(hba);
//	QJsonDocument p = QJsonDocument::fromBinaryData(QByteArray::fromBase64(QByteArray::fromStdString(jwtl.at(1).toStdString()),QByteArray::Base64UrlEncoding));
	QJsonObject header = h.object();
	QJsonObject payload = p.object();
	//todo:test this.

	qDebug() << header.value("typ");
	qDebug() << payload.value("exp");
	qDebug() << payload.value("jti").toString();
	QSqlQuery query;
	query.prepare(q);
	query.bindValue(":valA",payload.value("UserID").toString());
	query.bindValue(":valB",payload.value("jti").toString());
	query.exec();
	if(query.size() > 0)
	{
		query.next();
		QString secret = query.value("secret").toString();
		QCryptographicHash hasher(QCryptographicHash::Sha3_512);
		hasher.addData(h.toJson().toBase64(QByteArray::Base64UrlEncoding));
		hasher.addData(p.toJson().toBase64(QByteArray::Base64UrlEncoding));
		hasher.addData(secret.toStdString().c_str(),secret.size());
		if(hasher.result().toBase64(QByteArray::Base64UrlEncoding) == jwtl.at(2))
		{//authentication successful
			return true;
		}
		else
		{
			qDebug() << "hashes don't match. possible forged token";
		}	
	}
	else
	{
		qDebug() << query.lastError().text();
		qDebug() << "token refers to nonexistent userid:" << payload.value("UserID").toString() ;
	}
	qDebug() << "verification failed";
	verificationFail(c);
	return false;
	//QSqlQuery query;
	//query.prepare(q);
	//query.bindValue(":valA",obj.value(QStringLiteral("JWT")));
	//query.exec();
	/*
	if(query.size() > 0)
	{
		//token valid-allowed to proceed
	}
	else
	{
		verificationFail(c);
		return;
	}*/
}
void ApiV1::verificationFail(Context *c)
{
		c->response()->setJsonObjectBody({
							{QStringLiteral("Response"),QStringLiteral("1")}
						});
		//c->setStatus(HttpStatus::Forbidden);
}
void song_request(Context *c)
{
	qDebug() << Q_FUNC_INFO;
}
void song_request_POST(Context *c)
{
	/*
	{
		"JWT": "<jwt token info>"
		"SongID": "<songid>"
	}
	*/
	const QJsonDocument doc = c->request()->bodyData().toJsonDocument();
	const QJsonObject obj = doc.object();
	QString jwt = obj.value(QStringLiteral("JWT")).toString();
	if(verifyUser(jwt))
	{
		QString s = "SELECT * FROM Songs WHERE SongID = :valA";
		c->response()->setContentType("audio/ogg");
		return;
	}
	
}
