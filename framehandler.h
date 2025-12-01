#ifndef FRAMEHANDLER_H
#define FRAMEHANDLER_H

#include <QVector>
#include <string>

class FrameHandler
{
private:
#pragma pack(push, 1)
    struct frame {
        uint16_t sof : 1;            // 1 бит - Start Of Frame
        uint16_t id : 11;            // 11 бит - Identifier
        uint16_t rtr : 1;            // 1 бит - Remote Transmission Request
        uint16_t ide : 1;            // 1 бит - Identifier Extension
        uint16_t r0 : 1;             // 1 бит - Reserved bit 0
        uint16_t dlc : 4;            // 4 бита - Data Length Code
        uint16_t data[8];            // Данные
        uint16_t crc : 15;           // 15 бит - Cyclic Redundancy Check
        uint16_t crc_delimeter : 1;  // 1 бит - CRC Delimiter
        uint16_t ack : 1;            // 1 бит - ACK Slot
        uint16_t ack_delimeter : 1;  // 1 бит - ACK Delimiter
        uint16_t eof : 7;            // 7 бит - End Of Frame
    };
#pragma pack(pop)

public:
    FrameHandler(int id, bool rtr, int dlc, QVector<uint8_t> data) :
        id_(id), rtr_(rtr), dlc_(dlc), data_(data) {};
    void CreateFrame();

    friend std::ostream& operator<<(std::ostream& os, const FrameHandler& frame);
    std::string GetStringFrame() const;

private:
    void SetDataFromVector(uint16_t data[8], const QVector<uint8_t>& initial_data, uint16_t dlc) const;
    std::string ToBinaryString(uint16_t number, size_t length) const;
    void AddFillBit(std::string& frame) const;
    uint16_t calculateCRC15(const uint16_t data[8], uint8_t dlc, uint16_t initialBitstream) const;


private:
    int id_;
    bool rtr_;
    int dlc_;
    frame frame_;
    QVector<quint8> data_;

};

#endif // FRAMEHANDLER_H
