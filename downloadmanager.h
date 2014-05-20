#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslError>
#include <QStringList>
#include <QTimer>
#include <QUrl>
#include <QDate>
#include <QSqlQuery>
#include <QTimer>
#include <QDebug>


class DownloadManager: public QObject
{
    Q_OBJECT
    QNetworkAccessManager manager;
    QList<QNetworkReply *> currentDownloads;

public:
    DownloadManager();
    void doDownload(const QUrl &url);
    QString saveFileName(const QUrl &url);
    bool saveToDisk(const QString &filename, QIODevice *data);
    QDate date, last_update;
    QSqlQuery query;
    QString basename;

public slots:
    void execute();
    bool downloadFinished(QNetworkReply *reply);
    void slot_get_last_update();

signals:
    void signal_end_load(QString);
};


#endif // DOWNLOADMANAGER_H
