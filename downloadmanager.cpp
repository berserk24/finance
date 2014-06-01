#include <QCoreApplication>

#include "downloadmanager.h"

// constructor
DownloadManager::DownloadManager()
{
    // signal finish(), calls downloadFinished()
    date = QDate::currentDate();
    slot_get_last_update();
    connect(&manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(downloadFinished(QNetworkReply*)));
}

void DownloadManager::slot_get_last_update()
{
    query.exec("SELECT data FROM update_date WHERE id = 'bik_update'");
    query.first();
    last_update = query.value(0).toDate();
    query.clear();
}

void DownloadManager::execute()
{
    // get the argument including program's name
    QString arg;
    arg = "http://www.cbr.ru/mcirabis/BIK/bik_db_" + date.toString("ddMMyyyy") + ".zip";

    // process each url starting from the 2nd one
    QUrl url = QUrl::fromEncoded(arg.toLocal8Bit());
    doDownload(url);
}

// Constructs a QList of QNetworkReply
void DownloadManager::doDownload(const QUrl &url)
{
    QNetworkRequest request(url);
    QNetworkProxyQuery npq(QUrl("http://www.google.com"));
    QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::systemProxyForQuery(npq);
    if (listOfProxies.size())
        QNetworkProxy::setApplicationProxy(listOfProxies[0]);
    QNetworkReply *reply = manager.get(request);

    // List of reply
    currentDownloads.append(reply);
}

QString DownloadManager::saveFileName(const QUrl &url)
{
    QString path = url.path();
    basename = QCoreApplication::applicationDirPath() + "/" + QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "download";


    return basename;
}

bool DownloadManager::downloadFinished(QNetworkReply *reply)
{
    QUrl url = reply->url();
    if (reply->error())
    {
        if (date >= last_update)
        {
            date = date.addDays(-1);
            execute();
        }
        else
        {
            return false;
        }
    }
    else
    {
        QString filename = saveFileName(url);
        if (saveToDisk(filename, reply))
        {
            emit signal_end_load(basename);
            return true;
        }
    }

    currentDownloads.removeAll(reply);
    reply->deleteLater();

    if (currentDownloads.isEmpty())
        // all downloads finished
        return true;
}

bool DownloadManager::saveToDisk(const QString &filename, QIODevice *reply)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    file.write(reply->readAll());
    file.close();

    return true;
}



