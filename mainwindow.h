#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QJsonArray>
#include <QDateTime>
#include <QCryptographicHash>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void resetGameStateFile();

private slots:
    void handleButtonClick();
    void resetGame();
    void loadGame();
    void saveMove(int i, int j, const QString &dateTime);

private:
    Ui::MainWindow *ui;
    QPushButton* buttons[4][4];
    QPushButton* loadButton;
    QPushButton* resetButton;
    int clickCount = 0; // Счетчик нажатий

    QString previousHash;

    QString computeHash(const QString& i, const QString& j, const QString& dateTime, const QString& prevHash);
};

#endif // MAINWINDOW_H
