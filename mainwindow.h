#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QLineEdit>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

private slots:
    void updatePriorityIndicator(int id);
    void formatCanIdDisplay(int val);
    void onRtrToggled(bool checked);
    void updateDataFields(int byteCount);
    void onDlcChanged(int dlc);
    void pushButtonReset();

private:
    Ui::MainWindow *ui;
    QGridLayout *dataLayout;
    QVector<QLineEdit*> dataByteFields; // Для хранения полей байтов

};
#endif // MAINWINDOW_H
