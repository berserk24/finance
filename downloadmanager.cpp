#include <QCoreApplication>

#include "downloadmanager.h"

// constructor
DownloadManager::DownloadManager()
{
    // signal finish(), calls downloadFinished()
    connect(&manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(downloadFinished(QNetworkReply*)));
}

void DownloadManager::execute()
{
    // get the argument including program's name
    QStringList args;
    args << "http://www.cbr.ru/mcirabis/BIK/bik_db_" + QDate::currentDate().toString("ddMMyyyy") + ".zip";

    // skip the first argument, which is the program's name
    //args.takeFirst();

    if (args.isEmpty()) {
        /*printf("Qt Download example - downloads all URLs in parallel\n"
               "Usage: download url1 [url2... urlN]\n"
               "\n"
               "Downloads the URLs passed in the command-line to the local directory\n"
               "If the target file already exists, a .0, .1, .2, etc. is appended to\n"
               "differentiate.\n");
        QCoreApplication::instance()->quit();*/
        return;
    }

    // process each url starting from the 2nd one
    foreach (QString arg, args) {

        // QString::toLocal8Bit()
        //  - local 8-bit representation of the string as a QByteArray
        // Qurl::fromEncoded(QByteArray)
        //  - Parses input and returns the corresponding QUrl.
        //    input is assumed to be in encoded form,
        //    containing only ASCII characters.

        QUrl url = QUrl::fromEncoded(arg.toLocal8Bit());

        // makes a request
        doDownload(url);
    }
}

// Constructs a QList of QNetworkReply
void DownloadManager::doDownload(const QUrl &url)
{
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);

    // List of reply
    currentDownloads.append(reply);
}

QString DownloadManager::saveFileName(const QUrl &url)
{
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "download";

    /*if (QFile::exists(basename)) {
        // already exists, don't overwrite
        int i = 0;
        basename += '.';
        while (QFile::exists(basename + QString::number(i)))
            ++i;

        basename += QString::number(i);
    }*/

    return basename;
}

bool DownloadManager::downloadFinished(QNetworkReply *reply)
{
    QUrl url = reply->url();
    if (reply->error()) {
        //fprintf(stderr, "Download of %s failed: %s\n",
        //        url.toEncoded().constData(),
        //        qPrintable(reply->errorString()));
        return false;
    } else {
        QString filename = saveFileName(url);
        if (saveToDisk(filename, reply))
        {
            //printf("Download of %s succeeded (saved to %s)\n",
            //       url.toEncoded().constData(), qPrintable(filename));
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
    if (!file.open(QIODevice::WriteOnly)) {
        //fprintf(stderr, "Could not open %s for writing: %s\n",
        //        qPrintable(filename),
        //        qPrintable(file.errorString()));
        return false;
    }

    file.write(reply->readAll());
    file.close();

    return true;
}



