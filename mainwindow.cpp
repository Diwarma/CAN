#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLineEdit>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Настраиваем spinBoxID
    ui->spinBoxID->setRange(0, 0x7FF); // 11 бит для стандартного CAN
    ui->spinBoxID->setDisplayIntegerBase(16); // HEX отображение
    ui->spinBoxID->setAlignment(Qt::AlignRight);
    ui->spinBoxID->setFixedSize(90, 30);

    ui->spinBoxDLS->setRange(0, 8); // DLC может быть от 0 до 8 байт
    ui->spinBoxDLS->setValue(0);
    ui->spinBoxDLS->setFixedSize(80, 30); // Устанавливаем размер

    // Настраиваем scrollAreaDataField
    ui->scrollAreaDataField->setWidgetResizable(true);
    QWidget *scrollWidget = new QWidget();
    dataLayout = new QGridLayout(scrollWidget); // Сохраняем layout как член класса
    ui->scrollAreaDataField->setWidget(scrollWidget);

    // Подключаем сигнал
    connect(ui->spinBoxID, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::updatePriorityIndicator);

    connect(ui->checkBoxRTR, &QCheckBox::toggled,
            this, &MainWindow::onRtrToggled);

    connect(ui->spinBoxDLS, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onDlcChanged);

    connect(ui->pushButtonReset, &QPushButton::clicked,
            this, &MainWindow::pushButtonReset);

    // Инициализируем индикатор
    updatePriorityIndicator(0x000);
    updateDataFields(0);
}

void MainWindow::pushButtonReset()
{
    ui->checkBoxRTR->setChecked(false);
    ui->spinBoxID->setValue(0);
    ui->spinBoxDLS->setValue(0);
}

void MainWindow::formatCanIdDisplay(int value)
{
    // Временно отключаем сигналы чтобы избежать рекурсии
    ui->spinBoxID->blockSignals(true);

    // Получаем QLineEdit внутри QSpinBox и устанавливаем текст с ведущими нулями
    QLineEdit *lineEdit = ui->spinBoxID->findChild<QLineEdit*>();
    if (lineEdit) {
        QString formattedText = QString("0x%1").arg(value, 3, 16, QLatin1Char('0')).toUpper();
        lineEdit->setText(formattedText);
    }

    ui->spinBoxID->blockSignals(false);
}

void MainWindow::updatePriorityIndicator(int id)
{
    // Сначала форматируем отображение
    formatCanIdDisplay(id);

    QString style;

    // Определяем уровень приоритета и внешний вид
    if (id == 0)
        style = "background-color: #FF0000; color: white; font-weight: bold; border: 2px solid #CC0000;";
    else if (id <= 0x0F)
        style = "background-color: #FF4444; color: white; font-weight: bold; border: 2px solid #CC2222;";
    else if (id <= 0x7F)
        style = "background-color: #FF8800; color: black; font-weight: bold; border: 2px solid #CC6600;";
    else if (id <= 0x1FF)
        style = "background-color: #FFFF00; color: black; border: 2px solid #CCCC00;";
    else if (id <= 0x5FF)
        style = "background-color: #88FF88; color: black; border: 2px solid #66CC66;";
    else if (id <= 0x7FE)
        style = "background-color: #44AA44; color: white; border: 2px solid #338833;";
    else
        style = "background-color: #226622; color: white; font-weight: bold; border: 2px solid #114411;";

    // Применяем стиль к spinBox
    ui->spinBoxID->setStyleSheet(QString("QSpinBox { %1 }").arg(style));
}

// В mainwindow.cpp
void MainWindow::onRtrToggled(bool checked)
{
    if (checked) {
        // Если RTR активирован
        ui->spinBoxDLS->setValue(0); // Устанавливаем DLC в 0
        ui->spinBoxDLS->setEnabled(false); // Отключаем spinBoxDLS
        ui->checkBoxRTR->setToolTip("Remote Frame активен - данные не передаются");
    } else {
        // Если RTR деактивирован
        ui->spinBoxDLS->setEnabled(true); // Включаем spinBoxDLS
        ui->checkBoxRTR->setToolTip("Remote Transmission Request - запрос данных от другого узла");
    }

    // Обновляем поля данных в зависимости от текущего DLC
    updateDataFields(ui->spinBoxDLS->value());
}
void MainWindow::onDlcChanged(int dlc)
{
    updateDataFields(dlc);
}

void MainWindow::updateDataFields(int byteCount)
{
    // Всегда создаем 8 полей для байтов
    const int totalBytes = 8;

    // Сохраняем текущие значения полей перед очисткой
    QVector<QString> savedValues(totalBytes);
    for (int i = 0; i < qMin(dataByteFields.size(), totalBytes); i++) {
        savedValues[i] = dataByteFields[i]->text();
    }

    // Очищаем существующие поля
    for (QLineEdit *field : dataByteFields) {
        dataLayout->removeWidget(field);
        delete field;
    }
    dataByteFields.clear();


    // Очищаем все виджеты из layout (кроме заголовков)
    QLayoutItem *item;
    while ((item = dataLayout->takeAt(1)) != nullptr) { // Начинаем с 1, чтобы сохранить заголовки
        if (item->widget() && item->widget()->objectName() != "header_widget") {
            delete item->widget();
        }
        delete item;
    }


    // Добавляем заголовки
    dataLayout->addWidget(new QLabel("Байт"), 0, 0);
    dataLayout->addWidget(new QLabel("HEX"), 0, 1);
    dataLayout->addWidget(new QLabel("Биты"), 0, 2);

    // Удаляем старую информационную строку если есть
    QLayoutItem *infoItem;
    while ((infoItem = dataLayout->takeAt(totalBytes + 1)) != nullptr) {
        if (infoItem->widget()) {
            delete infoItem->widget();
        }
        delete infoItem;
    }

    // Создаем все 8 полей для байтов
    for (int i = 0; i < totalBytes; i++) {
        // Создаем label для номера байта
        QLabel *byteLabel = new QLabel("Байт " + QString::number(i+1));
        byteLabel->setAlignment(Qt::AlignLeft);
        dataLayout->addWidget(byteLabel, i + 1, 0);

        // Создаем поле для ввода HEX значения байта
        QLineEdit *byteField = new QLineEdit();
        byteField->setPlaceholderText("00");
        byteField->setMaxLength(2);
        byteField->setFixedSize(50, 25);
        byteField->setAlignment(Qt::AlignCenter);

        // Устанавливаем валидатор для HEX ввода
        QRegularExpressionValidator *validator = new QRegularExpressionValidator(
            QRegularExpression("[0-9A-Fa-f]{0,2}"), byteField);
        byteField->setValidator(validator);

        // Восстанавливаем сохраненное значение если оно есть
        if (i < savedValues.size() && !savedValues[i].isEmpty()) {
            byteField->setText(savedValues[i]);
        }

        // Управляем доступностью поля в зависимости от byteCount
        if (i < byteCount) {
            byteField->setEnabled(true);
            byteField->setStyleSheet("QLineEdit { background-color: white; }");
        } else {
            byteField->setEnabled(false);
            byteField->setStyleSheet("QLineEdit { background-color: #f0f0f0; color: #999; }");
            if (i >= byteCount) {
                byteField->setText(""); // Очищаем только неактивные поля
            }
        }

        // Добавляем в layout и в массив
        dataLayout->addWidget(byteField, i + 1, 1);
        dataByteFields.append(byteField);

        // Создаем label для битового представления
        QLabel *bitLabel = new QLabel("00000000");
        bitLabel->setFixedSize(60, 25);
        bitLabel->setAlignment(Qt::AlignCenter);

        // Управляем стилем битового представления
        if (i < byteCount) {
            bitLabel->setStyleSheet("QLabel { background-color: #e8f4fd; border: 1px solid #ccc; font-family: monospace; }");

            // Обновляем битовое представление для восстановленных значений
            if (!byteField->text().isEmpty()) {
                QString hexText = byteField->text();
                bool ok;
                QString paddedHex = hexText.length() == 1 ? "0" + hexText : hexText;
                int value = paddedHex.toInt(&ok, 16);
                if (ok) {
                    QString binary = QString("%1").arg(value, 8, 2, QLatin1Char('0'));
                    bitLabel->setText(binary);
                }
            }
        } else {
            bitLabel->setStyleSheet("QLabel { background-color: #f0f0f0; border: 1px solid #ddd; color: #999; font-family: monospace; }");
            bitLabel->setText("--------");
        }

        dataLayout->addWidget(bitLabel, i + 1, 2);

        // Подключаем обновление битового представления для активных полей
        if (i < byteCount) {
            connect(byteField, &QLineEdit::textChanged, this, [bitLabel, byteField]() {
                QString hexText = byteField->text();

                if (!hexText.isEmpty()) {
                    bool ok;
                    QString paddedHex = hexText.length() == 1 ? "0" + hexText : hexText;
                    int value = paddedHex.toInt(&ok, 16);
                    if (ok) {
                        QString binary = QString("%1").arg(value, 8, 2, QLatin1Char('0'));
                        bitLabel->setText(binary);
                        return;
                    }
                }
                bitLabel->setText("00000000");
            });
        }
    }

    // Добавляем информационную строку
    QLabel *infoLabel = new QLabel();
    if (byteCount == 0) {
        if (ui->checkBoxRTR->isChecked()) {
            infoLabel->setText("Remote Frame - передача данных отключена");
        } else {
            infoLabel->setText("DLC = 0 - данные отсутствуют");
        }
    } else {
        infoLabel->setText(QString("Активные байты: 1-%1").arg(byteCount));
    }
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setStyleSheet("QLabel { color: #666; font-style: italic; margin-top: 10px; }");
    dataLayout->addWidget(infoLabel, totalBytes + 1, 0, 1, 3);
}

MainWindow::~MainWindow()
{
    delete ui;
}
