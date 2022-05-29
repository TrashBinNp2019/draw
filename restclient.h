#ifndef RESTCLIENT_H
#define RESTCLIENT_H

#include <QDomDocument>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>


class RestClient : public QObject
{
    Q_OBJECT

public:
    RestClient( QSettings &, QObject * = nullptr );

    void init();
    void wipe();
    bool check();

    void getFiles();
    void getFile( const QString & );
    void uploadFile(const QString & , const QString &path);

signals:
    void filesReady( const QStringList & );
    void fileReady( const QDomDocument & );
    void uploadSuccessful();

    void networkError( const QNetworkReply::NetworkError & );
    void internalError( const QString & );

private slots:
    void onFilesReady();
    void onFileReady();
    void onUploadFinished();

private:
    QString userId;
    QSettings *settings;
    QNetworkAccessManager manager;
    QNetworkReply *replyGlobal;
    QDomDocument docGlobal;

    void createNewUser();
    void deleteUser();
};

#endif // RESTCLIENT_H
