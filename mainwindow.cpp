#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLineEdit>
#include <QDebug>
#include <QVector>
#include <fstream>
#include <QDir>
#include <QFileInfo>
#include <QDialog>
#include <QVBoxLayout>
#include "framehandler.h"
#include "qcustomplot.h"
#include <bitset>

int BIT_IN_BYTE = 8;

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
        sp->setPrefix("0x");
        sp->setValue(0);
    }

    ui->sp_id->setRange(0x000, 0x7FF);
    ui->sp_id->setDisplayIntegerBase(16);
    ui->sp_id->setStyleSheet("QSpinBox { text-transform: uppercase; }");

    ui->sp_dls->setRange(0, vect_hex.size()); // DLC может быть от 0 до 8 байт
    ui->sp_dls->setValue(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getStrFromValue(int decimal) {
    return QString("%1").arg(decimal, BIT_IN_BYTE, 2, QLatin1Char('0'));
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

void MainWindow::drawCanPlot(const QString& bitString) {
    QVector<double> x, y;

    // Преобразуем биты в точки для графика в ступенчатом стиле
    for(int i = 0; i < bitString.length(); i++) {
        double bitValue = (bitString[i] == '1') ? 1.0 : 0.0;
        x.append(2*i + 1); // центр интервала [2*i, 2*i+2]
        y.append(bitValue);
    }

    // Создаем график
    QCustomPlot *customPlot = new QCustomPlot(this);
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, y);
    customPlot->graph(0)->setLineStyle(QCPGraph::lsStepCenter); // ступенчатый график
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone)); // убрать точки
    customPlot->xAxis->setLabel("Bit Position");
    customPlot->yAxis->setLabel("Logic Level");
    customPlot->xAxis->setRange(0, 2*bitString.length());
    customPlot->yAxis->setRange(-0.1, 1.5); // Немного увеличили верхний предел для текста

    // Добавляем отображение битов сверху графика
    for(int i = 0; i < bitString.length(); i++) {
        QCPItemText *bitText = new QCPItemText(customPlot);
        bitText->position->setCoords(2*i + 1, 1.2); // Позиция над графиком
        bitText->setText(bitString.mid(i, 1)); // Отображаем 0 или 1
        bitText->setFont(QFont(font().family(), 10, QFont::Bold));
    }

    customPlot->replot();

    // Добавляем на форму или показываем в отдельном окне
    QDialog *plotDialog = new QDialog(this);
    QVBoxLayout *layout = new QVBoxLayout(plotDialog);
    layout->addWidget(customPlot);
    plotDialog->resize(1200, 300);
    plotDialog->exec();
}

// Для Вовы - обратная конвертация из бинарного файла в строку
static std::string BinaryFileToString(const QString& filePath) {
    std::ifstream file(filePath.toStdString(), std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filePath.toStdString());
    }

    // Получаем размер файла
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Читаем файл
    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::runtime_error("Ошибка чтения файла: " + filePath.toStdString());
    }

    // Конвертируем байты обратно в строку из '0' и '1'
    std::string result;
    for (uint8_t byte : buffer) {
        result += std::bitset<8>(byte).to_string();
    }

    return result;
}

// Для Вовы - конвертация вектора байтов в строку
static std::string BytesToString(const std::vector<uint8_t>& bytes) {
    std::string result;
    for (uint8_t byte : bytes) {
        result += std::bitset<8>(byte).to_string();
    }
    return result;
}

void MainWindow::on_pb_send_clicked()
{
    int id = ui->sp_id->value();
    bool rtr = ui->cb_rtr->isChecked();
    int dlc = ui->sp_dls->value();
    QVector<quint8> data{};
    for (const auto& d : vect_hex) {
        data.push_back(d->value());
    }

    FrameHandler frame(id, rtr, dlc, data);
    frame.CreateFrame();
    std::string str = frame.GetStringFrame();
    ui->labelOutput_2->setText("Кадр: " + QString::fromStdString(str));
    drawCanPlot(QString::fromStdString(str));
    QString filePath = "files/output.bin";


    // Создаем директорию через Qt
    QDir dir;
    if (!dir.exists("files")) {
        dir.mkpath("files");
    }
    std::ofstream file(filePath.toStdString(), std::ios::binary);

    if (file.is_open()) {
        // Преобразование бинарной строки в байты
        std::string binaryStr = str;
        std::vector<uint8_t> bytes;

        // Преобразуем каждые 8 символов в байт
        for (size_t i = 0; i < binaryStr.length(); i += 8) {
            std::string byteStr = binaryStr.substr(i, 8);

            // Если в конце меньше 8 бит, дополняем нулями
            if (byteStr.length() < 8) {
                byteStr.append(8 - byteStr.length(), '0');
            }

            // Конвертируем строку из битов в байт
            uint8_t byte = 0;
            for (size_t j = 0; j < 8; ++j) {
                if (byteStr[j] == '1') {
                    byte |= (1 << (7 - j));
                }
            }
            bytes.push_back(byte);
        }

        // Записываем байты в файл
        file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        file.close();

        // Получаем абсолютный путь через Qt
        QFileInfo fileInfo(filePath);
        QString absolutePath = fileInfo.absoluteFilePath();

        // Выводим полный путь в статус-бар
        ui->statusbar->showMessage(QString("Файл сохранен: %1").arg(absolutePath), 5000);
    } else {
        ui->statusbar->showMessage("Ошибка: не удалось создать файл", 5000);
    }
    file.close();
    return;
}

