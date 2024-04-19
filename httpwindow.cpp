#include "httpwindow.h"
#include "utils.h"
#include <QtWidgets>
#include <QtNetwork>
#include <QUrl>
#include <memory>

const char defaultUrl[] = "https://source.unsplash.com/random?";
const char defaultSearchTerm[] = "wallpaper";
const char defaultFileName[] = "image.png";

ProgressDialog::ProgressDialog(const QUrl &url, QWidget *parent)
    : QProgressDialog(parent)
{
    setWindowTitle(tr("Download Progress"));
    setLabelText(tr("Downloading %1.").arg(url.toDisplayString()));
    setMinimum(0);
    setValue(0);
    setMinimumDuration(0);
    setMinimumSize(QSize(400, 75));
}

void ProgressDialog::networkReplyProgress(qint64 bytesRead, qint64 totalBytes)
{
    setMaximum(totalBytes);
    setValue(bytesRead);
}

HttpWindow::HttpWindow(QWidget *parent)
    : QDialog(parent)
    , statusLabel(new QLabel(tr("Please enter the URL of a file you want to download.\n\n"), this))
    , fileNameLineEdit(new QLineEdit(defaultSearchTerm))
    , downloadButton(new QPushButton(tr("Download")))
    , launchCheckBox(new QCheckBox(tr("Open image")))
    , windowHeading(new QLabel("Your next creative Surprise"))
{
    setWindowTitle(tr("Unsplash Random Wallpapers"));
    setMinimumSize(700, 500);

    QFont headingFont;
    headingFont.setPointSize(20);
    headingFont.setBold(true);
    windowHeading->setFont(headingFont);

    QFormLayout *formLayout = new QFormLayout;
    fileNameLineEdit->setClearButtonEnabled(true);
    fileNameLineEdit->setMinimumWidth(400);
    connect(fileNameLineEdit, &QLineEdit::textChanged, this, &HttpWindow::enableDownloadButton);
    formLayout->addRow(tr("&Category:"), fileNameLineEdit);
    QString downloadDirectory = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    downloadDirectory = QDir::currentPath();
    launchCheckBox->setChecked(true);
    formLayout->addRow(launchCheckBox);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(windowHeading);
    mainLayout->addLayout(formLayout);

    mainLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));

    statusLabel->setWordWrap(true);
    mainLayout->addWidget(statusLabel);

    downloadButton->setDefault(true);
    connect(downloadButton, &QAbstractButton::clicked, this, &HttpWindow::downloadFile);
    QPushButton *quitButton = new QPushButton(tr("Quit"));
    quitButton->setAutoDefault(false);
    connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);
    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(downloadButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);
    mainLayout->addWidget(buttonBox);

    fileNameLineEdit->setFocus();
}

HttpWindow::~HttpWindow() = default;

void HttpWindow::startRequest(const QUrl &requestedUrl)
{
    QString qString = fileNameLineEdit->text();

    // Convert QString to UTF-8 encoded QByteArray
    QByteArray byteArray = qString.toUtf8();

    // Get pointer to underlying data
    const char *charArray = byteArray.constData();

    int totalLength = strlen(defaultUrl) + strlen(charArray) + 1; // +1 for null terminator

    // Create a char array to store the concatenated string
    char concatenatedUrl[totalLength];

    // Copy the defaultUrl to concatenatedUrl
    strcpy(concatenatedUrl, defaultUrl);

    // Concatenate defaultSearchTerm to concatenatedUrl
    strcat(concatenatedUrl, charArray);
    url = concatenatedUrl;
    httpRequestAborted = false;
    reply.reset(qnam.get(QNetworkRequest(url)));
    connect(reply.get(), &QNetworkReply::finished, this, &HttpWindow::httpFinished);
    connect(reply.get(), &QIODevice::readyRead, this, &HttpWindow::httpReadyRead);
    qDebug() << reply->readAll();
    ProgressDialog *progressDialog = new ProgressDialog(url, this);
    progressDialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(progressDialog, &QProgressDialog::canceled, this, &HttpWindow::cancelDownload);
    connect(reply.get(), &QNetworkReply::downloadProgress,
            progressDialog, &ProgressDialog::networkReplyProgress);
    connect(reply.get(), &QNetworkReply::finished, progressDialog, &ProgressDialog::hide);
    progressDialog->show();

    statusLabel->setText(tr("Downloading %1...").arg(url.toString()));
}

void HttpWindow::downloadFile()
{
    QString fileName = generateRandomSlug(fileNameLineEdit->text(), "png");
    QByteArray byteArray = fileName.toUtf8();
    const char *charArray = byteArray.constData();
    int totalLength = strlen(defaultUrl) + strlen(charArray) + 1;
    char concatenatedUrl[totalLength];
    strcpy(concatenatedUrl, defaultUrl);
    strcat(concatenatedUrl, charArray);

    const QString urlSpec = concatenatedUrl;

    if (urlSpec.isEmpty())
        return;

    const QUrl newUrl = QUrl::fromUserInput(urlSpec);
    if (!newUrl.isValid()) {
        QMessageBox::information(this, tr("Error"),
                                 tr("Invalid URL: %1: %2").arg(urlSpec, newUrl.errorString()));
        return;
    }

    QString dirName = QDir::homePath() + "/Downloads";
    if (fileName.isEmpty())
        fileName = defaultFileName;
    QString downloadDirectory = QDir::cleanPath(dirName.trimmed());
    bool useDirectory = !downloadDirectory.isEmpty() && QFileInfo(downloadDirectory).isDir();
    if (useDirectory)
        fileName.prepend(downloadDirectory + '/');

    if (QFile::exists(fileName)) {
        QString alreadyExists = useDirectory
                                    ? tr("There already exists a file called %1. Overwrite?")
                                    : tr("There already exists a file called %1 in the current directory. Overwrite?");
        QMessageBox::StandardButton response = QMessageBox::question(this,
                                                                     tr("Overwrite Existing File"),
                                                                     alreadyExists.arg(QDir::toNativeSeparators(fileName)),
                                                                     QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (response == QMessageBox::No)
            return;
        QFile::remove(fileName);
    }

    file = openFileForWrite(fileName);
    if (!file)
        return;

    downloadButton->setEnabled(false);

    // schedule the request
    startRequest(newUrl);
}

std::unique_ptr<QFile> HttpWindow::openFileForWrite(const QString &fileName)
{
    std::unique_ptr<QFile> file = std::make_unique<QFile>(fileName);
    if (!file->open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Error"),
                                 tr("Unable to save the file %1: %2.")
                                     .arg(QDir::toNativeSeparators(fileName),
                                          file->errorString()));
        return nullptr;
    }
    return file;
}

void HttpWindow::cancelDownload()
{
    statusLabel->setText(tr("Download canceled."));
    httpRequestAborted = true;
    reply->abort();
    downloadButton->setEnabled(true);
}

void HttpWindow::httpFinished()
{
    QFileInfo fi;
    if (file) {
        fi.setFile(file->fileName());
        file->close();
        file.reset();
    }
    QNetworkReply::NetworkError error = reply->error();
    const QString &errorString = reply->errorString();
    reply.reset();
    if (error != QNetworkReply::NoError) {
        QFile::remove(fi.absoluteFilePath());
        // For "request aborted" we handle the label and button in cancelDownload()
        if (!httpRequestAborted) {
            statusLabel->setText(tr("Download failed:\n%1.").arg(errorString));
            downloadButton->setEnabled(true);
        }
        return;
    }
    statusLabel->setText(tr("Downloaded %1 bytes to %2\nin\n%3")
                             .arg(fi.size())
                             .arg(fi.fileName(), QDir::toNativeSeparators(fi.absolutePath())));
    if (launchCheckBox->isChecked())
        QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absoluteFilePath()));
    downloadButton->setEnabled(true);
}

void HttpWindow::httpReadyRead()
{
    // This slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file)
        file->write(reply->readAll());
}

void HttpWindow::enableDownloadButton()
{
    downloadButton->setEnabled(!fileNameLineEdit->text().isEmpty());
}
