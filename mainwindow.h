#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QSpinBox>
#include <QString>

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
    QString getStrFromValue(int decimal);
    void drawCanPlot(const QString& bitString);

private slots:
    void on_sp_dls_valueChanged(int arg1);
    void on_cb_rtr_clicked(bool checked);
    void on_pb_reset_clicked();
    void on_sp_hex1_valueChanged(int arg1);

    void on_sp_hex2_valueChanged(int arg1);

    void on_sp_hex3_valueChanged(int arg1);

    void on_sp_hex4_valueChanged(int arg1);

    void on_sp_hex5_valueChanged(int arg1);

    void on_sp_hex6_valueChanged(int arg1);

    void on_sp_hex7_valueChanged(int arg1);

    void on_sp_hex8_valueChanged(int arg1);

    void on_pb_send_clicked();


private:
    Ui::MainWindow *ui;
    QVector<QLabel*> vect_bit;
    QVector<QSpinBox*> vect_hex;

};
#endif // MAINWINDOW_H
