#include "framehandler.h"

#include <bitset>

void FrameHandler::CreateFrame() {

    frame_.sof = 0;
    frame_.id = id_;
    frame_.rtr = rtr_;
    frame_.ide = 0;
    frame_.r0 = 0;
    if (frame_.rtr == 1) {
        frame_.dlc = 0;
    } else {
        frame_.dlc = dlc_;
    }

    SetDataFromVector(frame_.data, data_, frame_.dlc);

    uint16_t initial_bits = frame_.sof + frame_.id + frame_.rtr + frame_.ide + frame_.r0;
    frame_.crc = calculateCRC15(frame_.data, frame_.dlc, initial_bits);

    frame_.crc_delimeter = 1;
    frame_.ack = 0;
    frame_.ack_delimeter = 1;
    frame_.eof = 0b1111111;

    return;
}

std::string FrameHandler::GetStringFrame() const {
    std::string frame_str {};

    frame_str += ToBinaryString(frame_.sof, 1);
    frame_str += ToBinaryString(frame_.id, 11);
    frame_str += ToBinaryString(frame_.rtr, 1);
    frame_str += ToBinaryString(frame_.ide, 1);
    frame_str += ToBinaryString(frame_.r0, 1);
    frame_str += ToBinaryString(frame_.dlc, 4);

    for (int i = 0; i < frame_.dlc; ++i) {
        frame_str += ToBinaryString(frame_.data[i], 8);
    }

    frame_str += ToBinaryString(frame_.crc, 15);
    frame_str += ToBinaryString(frame_.crc_delimeter, 1);
    frame_str += ToBinaryString(frame_.ack, 1);
    frame_str += ToBinaryString(frame_.ack_delimeter, 1);

    AddFillBit(frame_str);

    frame_str += ToBinaryString(frame_.eof, 7);

    return frame_str;
}

std::string FrameHandler::ToBinaryString(uint16_t number, size_t length) const {
    std::string binary = std::bitset<16>(number).to_string();

    // Очистка ведущих нулей
    size_t pos = binary.find('1');
    if (pos != std::string::npos) {
        binary = binary.substr(pos);
    } else {
        binary = "0";
    }

    // Добавление необходимого количества ведущих нулей
    if (length > binary.length()) {
        binary = std::string(length - binary.length(), '0') + binary;
    }

    return binary;

}

void FrameHandler::SetDataFromVector(uint16_t data[8], const QVector<uint8_t>& initial_data, uint16_t dlc) const {
    // Очищаем весь массив данных
    for (int i = 0; i < 8; ++i) {
        data[i] = 0x00;
    }

    // Заполняем только те байты, которые указаны в DLC
    for (int i = 0; i < dlc && i < 8 && i < initial_data.size(); ++i) {
        data[i] = initial_data[i];
    }
}

void FrameHandler::AddFillBit(std::string& frame) const {
    size_t i = 0;
    while (i <= frame.length() - 5) {
        bool allSame = true;

        // Проверка 5 ближайших символов
        for (size_t j = i + 1; j < i + 5; j++) {
            if (frame[j] != frame[i]) {
                allSame = false;
                break;
            }
        }

        if (allSame) {
            // Вставить противоположный символ
            char opposite = (frame[i] == '0') ? '1' : '0';
            frame.insert(i + 5, 1, opposite);
            i += 6; // Пропустить обработанные символы
        } else {
            i++;
        }
    }
}

uint16_t FrameHandler::calculateCRC15(const uint16_t data[8], uint8_t dlc, uint16_t initialBitstream) const {
    // Константы для CAN CRC15
    const uint16_t CRC15_POLYNOMIAL = 0x4599; // x^15 + x^14 + x^10 + x^8 + x^7 + x^4 + x^3 + 1
    const uint16_t CRC15_MASK = 0x7FFF;       // Маска для 15 бит (0b0111111111111111)
    const int CRC15_LENGTH = 15;              // Длина CRC в битах

    // Размеры полей CAN кадра в битах
    const int ARBITRATION_FIELD_BITS = 15;    // SOF(1) + ID(11) + RTR(1) + IDE(1) + r0(1)
    const int CONTROL_FIELD_BITS = 4;         // DLC(4) + reserved(2)
    const int BITS_PER_BYTE = 8;

    uint16_t crc = 0;
    uint32_t bitstream = initialBitstream;

    // Общее количество бит для расчета CRC
    int dataFieldBits = dlc * BITS_PER_BYTE;
    int totalBitsForCRC = ARBITRATION_FIELD_BITS + CONTROL_FIELD_BITS + dataFieldBits;

    for (int currentBit = 0; currentBit < totalBitsForCRC; ++currentBit) {
        bool inputBit;

        // Определяем откуда брать текущий бит
        if (currentBit < ARBITRATION_FIELD_BITS) {
            // Биты из арбитражного поля
            int bitPositionInArbitration = ARBITRATION_FIELD_BITS - 1 - currentBit;
            inputBit = (bitstream >> bitPositionInArbitration) & 0x01;
        }
        else if (currentBit < ARBITRATION_FIELD_BITS + CONTROL_FIELD_BITS) {
            // Биты из контрольного поля (DLC + reserved)
            // ЗАГЛУШКА - здесь нужно передавать полный контрольный field
            inputBit = false;
        }
        else {
            // Биты из поля данных
            int bitIndexInDataField = currentBit - (ARBITRATION_FIELD_BITS + CONTROL_FIELD_BITS);
            int byteIndex = bitIndexInDataField / BITS_PER_BYTE;
            int bitIndexInByte = BITS_PER_BYTE - 1 - (bitIndexInDataField % BITS_PER_BYTE);

            if (byteIndex < dlc) {
                inputBit = (data[byteIndex] >> bitIndexInByte) & 0x01;
            } else {
                inputBit = false;
            }
        }

        // Сдвигаем CRC и обрабатываем новый бит
        bool crcMostSignificantBit = (crc >> (CRC15_LENGTH - 1)) & 0x01;
        crc = (crc << 1) & CRC15_MASK;

        // Применяем полином если MSB XOR inputBit = 1
        if (crcMostSignificantBit ^ inputBit) {
            crc ^= CRC15_POLYNOMIAL;
        }
    }

    return crc;
}

std::ostream& operator<<(std::ostream& os, const FrameHandler& frame) {
    std::string str = frame.GetStringFrame();
    os << str;

    return os;
}
