#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QDebug>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    connect(ui->browseButton, &QPushButton::clicked, this, &MainWindow::browse);
    connect(ui->uploadButton, &QPushButton::clicked, this, &MainWindow::upload);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::browse()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Select File"));
    if (!filePath.isEmpty()) {
        ui->filePathLineEdit->setText(filePath);
    }
}

void MainWindow::upload()
{
    QString filePath = ui->filePathLineEdit->text();
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a file to upload."));
        return;
    }

    QUrl url("https://www.virustotal.com/vtapi/v2/file/scan");
    QNetworkRequest request(url);

    // Set your API key here
    QString apiKey = "e2a59eb38d66539c2ad5086baf908e8487b91f13452b3650c89f740633ed4cd1"; //= "YOUR_API_KEY";
    QByteArray apiKeyBytes = apiKey.toUtf8();

    // Set request headers
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    // Set request parameters
    QByteArray postData;
    postData.append("apikey=" + apiKeyBytes);
    postData.append("&file=" + QFile::encodeName(filePath));

    // Send POST request
    QNetworkReply *reply = networkManager->post(request, postData);

    // Connect signals/slots for response handling
    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            qDebug() << "Response: " << responseData;
            QMessageBox::information(this, tr("Upload Complete"), tr("File uploaded successfully."));
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to upload file. Error code: %1").arg(reply->error()));
        }
        reply->deleteLater();
    });
}
