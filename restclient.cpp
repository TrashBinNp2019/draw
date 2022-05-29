#include "restclient.h"

#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QFile>

RestClient::RestClient( QSettings &_settings, QObject *parent )
    : QObject( parent )
{
    settings = &_settings;
}

void RestClient::init()
{
    if( !settings->contains( "api/userId" ) ) {
        createNewUser();
    } else {
        userId = settings->value( "api/userId" ).toString();
    }

    settings->setValue( "api/enabled", true );
}

void RestClient::wipe()
{
    if( settings->contains( "api/enabled" ) && !settings->value( "api/enabled" ).toBool() ) return;

    deleteUser();
    settings->remove( "api/userId" );
    settings->setValue( "api/enabled", false );
}

bool RestClient::check()
{
    return settings->contains( "api/enabled" ) && settings->value( "api/enabled" ).toBool();
}

void RestClient::getFiles()
{
    QNetworkRequest request;
    request.setUrl( QUrl( "http://localhost:3000/api/" + userId ) );

    replyGlobal = manager.get( request );
    disconnect( replyGlobal, SIGNAL( finished() ), this, SLOT( onFileReady() ) );
    disconnect( replyGlobal, SIGNAL( finished() ), this, SLOT( onUploadFinished() ) );
    connect( replyGlobal, SIGNAL( finished() ), this, SLOT( onFilesReady() ) );
}

void RestClient::getFile( const QString &name )
{
    QNetworkRequest request;
    request.setUrl( QUrl( "http://localhost:3000/api/" + userId + "/files/" + name ) );

    replyGlobal = manager.get( request );
    disconnect( replyGlobal, SIGNAL( finished() ), this, SLOT( onFilesReady() ) );
    disconnect( replyGlobal, SIGNAL( finished() ), this, SLOT( onUploadFinished() ) );
    connect( replyGlobal, SIGNAL( finished() ), this, SLOT( onFileReady() ) );
}

void RestClient::uploadFile( const QString &name, const QString &path )
{
    QNetworkRequest request;
    request.setUrl( QUrl( "http://localhost:3000/api/" + userId + "/files/" + name ) );

    QHttpMultiPart *multiPart = new QHttpMultiPart( QHttpMultiPart::FormDataType );

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"upload\"; filename=\"upload.svg\"") );
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/svg+xml") );
    QFile *file = new QFile( path );
    if ( !file->open(QIODevice::ReadOnly) ) {
        emit internalError( "File inaccessible" );

        delete file;
        delete multiPart;
        return;
    }
    filePart.setBodyDevice(file);
    file->setParent( multiPart ); // delete resource later

    multiPart->append( filePart );

    replyGlobal = manager.put( request, multiPart );
    multiPart->setParent( replyGlobal ); // delete resource later

    disconnect( replyGlobal, SIGNAL( finished() ), this, SLOT( onFilesReady() ) );
    disconnect( replyGlobal, SIGNAL( finished() ), this, SLOT( onFileReady() ) );
    connect( replyGlobal, SIGNAL( finished() ), this, SLOT( onUploadFinished() ) );
}

void RestClient::onFilesReady()
{
    if ( replyGlobal->error() == QNetworkReply::NoError ) {
        QList< QString > files{};

        auto data = replyGlobal->readAll();
        auto obj = QJsonDocument::fromJson( data ).object();

        if ( obj.contains( "files" ) && obj[ "files" ].isArray() ) {
            auto arr = obj[ "files" ].toArray();
            bool success = true;

            for ( auto const &value : arr ) {
                auto file = value.toObject();

                if ( file.contains( "name" ) && file[ "name" ].isString() ) {
                    files.append( file[ "name" ].toString() );
                } else {
                    emit networkError( QNetworkReply::ContentNotFoundError );
                    success = false;

                    break;
                }
            }

            if ( success ) emit filesReady( files );
        } else {
            emit networkError( QNetworkReply::ContentNotFoundError );
        }
    } else {
        emit networkError( replyGlobal->error() );
    }

    replyGlobal->deleteLater();
    replyGlobal = 0;
}

void RestClient::onFileReady()
{
    if ( replyGlobal->error() == QNetworkReply::NoError ) {
        auto data = replyGlobal->readAll();
        if ( data.length() > 0 ) {
            docGlobal.setContent( data );
            emit fileReady( docGlobal );
        } else {
            emit networkError( QNetworkReply::ContentNotFoundError );
        }
    } else {
        emit networkError( replyGlobal->error() );
    }

    replyGlobal->deleteLater();
    replyGlobal = 0;
}

void RestClient::onUploadFinished()
{
    if ( replyGlobal->error() == QNetworkReply::NoError ) {
        emit uploadSuccessful();
    } else {
        emit networkError( replyGlobal->error() );
    }

    replyGlobal->deleteLater();
    replyGlobal = 0;
}

void RestClient::createNewUser()
{
    QNetworkRequest request;
    request.setUrl( QUrl( "http://localhost:3000/api" ) );

    QJsonObject param;
    QJsonArray arr;
    param.insert( "files", arr );

    QNetworkReply *reply = manager.put( request, QJsonDocument( param ).toJson( QJsonDocument::Compact ) );

    QEventLoop loop;
    connect( reply, SIGNAL( finished() ), &loop, SLOT( quit() ) );
    loop.exec();

    if ( reply->error() == QNetworkReply::NoError ) {
        auto data = reply->readAll();
        auto obj = QJsonDocument::fromJson( data ).object();

        if ( obj.contains( "id" ) && obj[ "id" ].isString() ) {
            userId = obj[ "id" ].toString();
            settings->setValue( "api/userId", userId );
        } else {
            emit networkError( QNetworkReply::ContentNotFoundError );
        }

    } else {
        emit networkError( reply->error() );
    }

    delete reply;
}

void RestClient::deleteUser()
{
    QNetworkRequest request;
    request.setUrl( QUrl( "http://localhost:3000/api/" + userId ) );

    QNetworkReply *reply = manager.deleteResource( request );

    QEventLoop loop;
    connect( reply, SIGNAL( finished() ), &loop, SLOT( quit() ) );
    loop.exec();

    if ( reply->error() == QNetworkReply::NoError ) {
    } else {
        emit networkError( reply->error() );
    }

    delete reply;
}
