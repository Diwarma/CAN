#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLineEdit>
#include <QDebug>
#include <QVector>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    vect_bit.clear();
    vect_hex.clear();

    vect_bit = {
        ui->lbl_bit1,
        ui->lbl_bit2,
        ui->lbl_bit3,
        ui->lbl_bit4,
        ui->lbl_bit5,
        ui->lbl_bit6,
        ui->lbl_bit7,
        ui->lbl_bit8
    };

    vect_hex = {
        ui->sp_hex1,
        ui->sp_hex2,
        ui->sp_hex3,
        ui->sp_hex4,
        ui->sp_hex5,
        ui->sp_hex6,
        ui->sp_hex7,
        ui->sp_hex8
    };

    for (auto& lbl : vect_bit) {
        lbl->clear();
    }

    for (auto& sp : vect_hex) {
        sp->setRange(0x00, 0xFF);
        sp->setDisplayIntegerBase(16);
        sp->setStyleSheet("QSpinBox { text-transform: uppercase; }");
        sp->clear();
        sp->setPrefix("0x");
        sp->setValue(0);
    }

    ui->sp_id->setRange(0x000, 0x7FF);
    ui->sp_id->setDisplayIntegerBase(16);
    ui->sp_id->setStyleSheet("QSpinBox { text-transform: uppercase; }");

    ui->sp_dls->setRange(0, vect_hex.size()); // DLC может быть от 0 до 8 байт
    ui->sp_dls->setValue(0);
}

QString MainWindow::getStrFromValue(int decimal) {
    if (decimal == 0) {
        return "0";
    }
    QString binary{};
    int val = decimal;

    while (decimal != 0) {
        int rev = decimal % 2;
        decimal /= 2;
        binary.insert(0, QString::number(rev));
    }

    return binary;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_sp_dls_valueChanged(int arg1)
{
    for (int i = 0; i < arg1; ++i) {
        QString binary = getStrFromValue(vect_hex[i]->value());
        vect_bit[i]->setText(binary);
        vect_hex[i]->setEnabled(true);
    }

    for (int i = arg1; i < vect_hex.size(); ++i) {
        vect_bit[i]->clear();
        vect_hex[i]->setDisabled(true);
    }
}

void MainWindow::on_cb_rtr_clicked(bool checked)
{
    if (checked) {
        // Если RTR активирован
        ui->sp_dls->setValue(0); // Устанавливаем DLS в 0
        ui->sp_dls->setEnabled(false); // Отключаем spinBoxDLS
        ui->cb_rtr->setToolTip("Remote Frame активен - данные не передаются");
    } else {
        // Если RTR деактивирован
        ui->sp_dls->setEnabled(true); // Включаем spinBoxDLS
        ui->cb_rtr->setToolTip("Remote Transmission Request - запрос данных от другого узла");
    }
}


void MainWindow::on_pb_reset_clicked()
{
    ui->cb_rtr->setChecked(false);
    ui->sp_id->setValue(0);
    ui->sp_dls->setValue(0);

    for (auto& lbl : vect_bit) {
        lbl->clear();
    }

    for (auto& sp : vect_hex) {
        sp->setValue(0);
    }
}

void MainWindow::on_sp_hex1_valueChanged(int arg1)
{
    QString binary = getStrFromValue(arg1);
    ui->lbl_bit1->setText(binary);
}


void MainWindow::on_sp_hex2_valueChanged(int arg1)
{
    QString binary = getStrFromValue(arg1);
    ui->lbl_bit2->setText(binary);
}


void MainWindow::on_sp_hex3_valueChanged(int arg1)
{
    QString binary = getStrFromValue(arg1);
    ui->lbl_bit3->setText(binary);
}


void MainWindow::on_sp_hex4_valueChanged(int arg1)
{
    QString binary = getStrFromValue(arg1);
    ui->lbl_bit4->setText(binary);
}


void MainWindow::on_sp_hex5_valueChanged(int arg1)
{
    QString binary = getStrFromValue(arg1);
    ui->lbl_bit5->setText(binary);
}


void MainWindow::on_sp_hex6_valueChanged(int arg1)
{
    QString binary = getStrFromValue(arg1);
    ui->lbl_bit6->setText(binary);
}


void MainWindow::on_sp_hex7_valueChanged(int arg1)
{
    QString binary = getStrFromValue(arg1);
    ui->lbl_bit7->setText(binary);
}


void MainWindow::on_sp_hex8_valueChanged(int arg1)
{
    QString binary = getStrFromValue(arg1);
    ui->lbl_bit8->setText(binary);
}
