#ifndef HTTPWINDOW_H
#define HTTPWINDOW_H

#include <QProgressDialog>
#include <QNetworkAccessManager>
#include <QUrl>

#include <memory>

QT_BEGIN_NAMESPACE
class QFile;
class QLabel;
class QLineEdit;
class QPushButton;
class QSslError;
class QNetworkReply;
class QCheckBox;

QT_END_NAMESPACE

/**
 * ProgressDialog is a custom QProgressDialog that displays the progress of a file download.
 */
class ProgressDialog : public QProgressDialog {
    Q_OBJECT

public:
    /**
     * Constructor for ProgressDialog
     *
     * @param url The URL of the file being downloaded
     * @param parent The parent QWidget
     */
    explicit ProgressDialog(const QUrl &url, QWidget *parent = nullptr);

public slots:
    /**
     * Updates the progress dialog with the current download progress
     *
     * @param bytesRead The number of bytes read so far
     * @param totalBytes The total number of bytes in the file
     */
    void networkReplyProgress(qint64 bytesRead, qint64 totalBytes);
};

/**
 * HttpWindow is a custom QDialog that handles file downloads.
 */
class HttpWindow : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructor for HttpWindow
     *
     * @param parent The parent QWidget
     */
    explicit HttpWindow(QWidget *parent = nullptr);
    /**
     * Destructor for HttpWindow
     */
    ~HttpWindow();

    /**
     * Starts a new download request
     */
    void startRequest();

private slots:
    /**
     * Initiates the download process
     */
    void downloadFile();
    /**
     * Cancels the current download
     */
    void cancelDownload();
    /**
     * Handles the completion of the download
     */
    void httpFinished();
    /**
     * Handles new data available for reading from the download
     */
    void httpReadyRead();
    /**
     * Enables or disables the download button based on
     * the current state of the category input
     */
    void enableDownloadButton();

private:
    /**
     * Opens a file for writing
     *
     * @param fileName The name of the file to open
     * @return A unique pointer to the opened file, or nullptr if the file cannot be opened
     */
    std::unique_ptr<QFile> openFileForWrite(const QString &fileName);

    QLabel *statusLabel;
    QLineEdit *fileNameLineEdit;
    QPushButton *downloadButton;
    QCheckBox *launchCheckBox;
    QLabel *windowHeading;

    QUrl url;
    QNetworkAccessManager qnam;
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply;
    std::unique_ptr<QFile> file;
    bool httpRequestAborted = false;
};

#endif
