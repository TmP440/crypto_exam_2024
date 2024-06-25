#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QGridLayout *gridLayout = new QGridLayout();

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            buttons[i][j] = new QPushButton(QString("%1,%2").arg(i).arg(j));
            buttons[i][j]->setFixedSize(100, 100);
            connect(buttons[i][j], &QPushButton::clicked, this, &MainWindow::handleButtonClick);
            gridLayout->addWidget(buttons[i][j], i, j);
        }
    }

    loadButton = new QPushButton("Load");
    resetButton = new QPushButton("Reset");

    connect(loadButton, &QPushButton::clicked, this, &MainWindow::loadGame);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::resetGame);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(gridLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(loadButton);
    buttonLayout->addWidget(resetButton);

    mainLayout->addLayout(buttonLayout);
    ui->centralwidget->setLayout(mainLayout);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::handleButtonClick() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button || button->property("clicked").toBool()) return;

    int i = button->text().split(',')[0].toInt();
    int j = button->text().split(',')[1].toInt();
    button->setProperty("clicked", true);

    clickCount++;
    if (clickCount % 2 == 0) {
        button->setStyleSheet("background-color: green");
    } else {
        button->setStyleSheet("background-color: red");
    }

    QString dateTime = QDateTime::currentDateTime().toString("yyyy.MM.dd_HH:mm:ss");
    saveMove(i, j, dateTime);

    qDebug() << "Button clicked. Click count: " << clickCount;

    // Проверяем все кнопки
    bool allButtonsClicked = true;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (!buttons[i][j]->property("clicked").toBool()) {
                allButtonsClicked = false;
                break;
            }
        }
        if (!allButtonsClicked) break;
    }

    // Если все кнопки были нажаты, сбрасываем игру
    if (allButtonsClicked) {
        resetGame();
        resetGameStateFile();
    }
}

void MainWindow::resetGameStateFile() {
    QFile file("game_state.json");
    if (file.exists()) {
        file.remove();
    }
}

void MainWindow::resetGame() {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            buttons[i][j]->setStyleSheet("");
            buttons[i][j]->setProperty("clicked", false);
        }
    }
    clickCount = 0; // Обнуляем счетчик нажатий
    previousHash.clear();

    qDebug() << "Game reset. Click count: " << clickCount;
}

void MainWindow::loadGame() {
    QFile file("game_state.json");
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Error", "Unable to open save file");
        return;
    }

    QByteArray saveData = file.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonArray movesArray = loadDoc.array();

    resetGame();

    bool errorFound = false;
    int errorMoveIndex = -1;

    for (int i = 0; i < movesArray.size(); ++i) {
        QJsonObject moveObject = movesArray[i].toObject();
        QString iStr = moveObject["i"].toString();
        QString jStr = moveObject["j"].toString();
        QString dateTime = moveObject["dateTime"].toString();
        QString hash = moveObject["hash"].toString();

        QString computedHash = computeHash(iStr, jStr, dateTime, previousHash);
        if (computedHash != hash) {
            QMessageBox::warning(this, "Error", QString("Hash mismatch at move %1").arg(i + 1));
            errorFound = true;
            errorMoveIndex = i;
            break;
        }

        int iIndex = iStr.toInt();
        int jIndex = jStr.toInt();
        QPushButton* button = buttons[iIndex][jIndex];
        button->setProperty("clicked", true);

        clickCount++;
        if (clickCount % 2 == 0) {
            button->setStyleSheet("background-color: green");
        } else {
            button->setStyleSheet("background-color: red");
        }

        previousHash = hash;
    }

    if (!errorFound) {
        QMessageBox::information(this, "Success", "Game state loaded successfully!");
    } else {
        QMessageBox::warning(this, "Error", QString("Error found at move %1").arg(errorMoveIndex + 1));
    }

    file.close();
}

void MainWindow::saveMove(int i, int j, const QString &dateTime) {
    QFile file("game_state.json");
    QJsonArray movesArray;

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray saveData = file.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
        movesArray = loadDoc.array();
        file.close();
    }

    QString hash = computeHash(QString::number(i), QString::number(j), dateTime, previousHash);

    QJsonObject moveObject;
    moveObject["i"] = QString::number(i);
    moveObject["j"] = QString::number(j);
    moveObject["dateTime"] = dateTime;
    moveObject["hash"] = hash;

    movesArray.append(moveObject);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Error", "Unable to open save file");
        return;
    }

    QJsonDocument saveDoc(movesArray);
    file.write(saveDoc.toJson());

    previousHash = hash;

    qDebug() << "Move saved. i: " << i << ", j: " << j << ", dateTime: " << dateTime;
}

QString MainWindow::computeHash(const QString& i, const QString& j, const QString& dateTime, const QString& prevHash) {
    QByteArray data = i.toUtf8() + j.toUtf8() + dateTime.toUtf8() + prevHash.toUtf8();
    return QString(QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex());
}
