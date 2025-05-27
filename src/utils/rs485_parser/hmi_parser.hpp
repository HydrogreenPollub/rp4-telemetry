#ifndef HMI_PARSER_HPP
#define HMI_PARSER_HPP

#include <functional>
#include <iostream>

#include <boost/crc.hpp>
#include "struct_fields.hpp"

static_assert(CHAR_BIT == 8, "");
static_assert(std::is_same<uint8_t, unsigned char>::value, "");
static_assert(sizeof(uint16_t) == 2, "");
static_assert(sizeof(uint32_t) == 4, "");


struct MasterMeasurements{
    float fuelCellOutputCurrent;
    float supercapacitorCurrent;
    float motorControllerSupply;
    float ai4;
    float ai5;
    float accelPedalVoltage;
    float brakePedalVoltage;
    float accessoryBatteryCurrent;
    float hydrogenHighPressure;
    float hydrogenLeakageSensorVoltage;
    float ai10;
    float ai11;
    float accessoryBatteryVoltage;

    float fuelCellOutputVoltage;
    float supercapacitorVoltage;
    float motorControllerSupplyVoltage;

    uint8_t din;

    float rpm;

    float speed;
};

struct MasterStatus{
    enum State: uint8_t{
        Idle,
        Running,
        Shutdown,
        Failure
    };

    uint8_t state = Idle;

    bool mainValveEnableOutput = false;
    bool motorControllerEnableOutput = false;
    float accelOutputVoltage = 0.0f;
    float brakeOutputVoltage = 0.0f;
};

struct ProtiumValues{
    /*float FC_V;
    float FC_A;
    float FC_W;
    float Energy;
    float FCT1;
    float FAN;
    float H2P1;
    float H2P2;
    float TankP;
    float TankT;
    float V_Set;
    float I_Set;
    float UCB_V;
    float Stasis_selector;
    float STASIS_V1;
    float STASIS_V2;
    float Number_of_cell;
    float FCT2;
    float BLW;
    float BattV;
    float IP;
    float TP;*/
    float FC_V;
    float FC_A;
    float FC_W;
    float Energy;
    float FCT1;
    float FAN;
    float H2P1;
    float H2P2;
    float TankP;
    float TankT;
    float V_Set;
    float I_Set;
    float UCB_V;
    float Stasis_selector;
    float STASIS_V1;
    float STASIS_V2;
    float Number_of_cell;
    float FCT2;
    float BLW;
    float BattV;
    float IP;
    float TP;
};

enum ProtiumOperatingState: uint8_t{
    Disconnected,
    SystemOff,
    FirmwareVersion,
    CommandNotFound,
    EnteringToStartingPhase,
    EnteringToRunningPhase,
    AnodeSupplyPressureCheck,
    TemperatureCheck,
    FCGasSystemCheck,
    FCSealingCheck,
    FCVoltageCheck,
    LowH2Supply,
    ShutdownInitiated,
    AbnormalShutdownInitiated,
    Running
};

struct ProtiumOperatingStateLogEntry{
    uint32_t msClockTickCount = 0;
    uint8_t state = Disconnected;
};


class HmiParser{
public:
    std::function<void (uint32_t msClockTickCount, uint32_t cycleClockTickCount, const MasterMeasurements& measurements)> onMasterMeasurements;
    std::function<void (uint32_t msClockTickCount, uint32_t cycleClockTickCount, const MasterStatus& status)> onMasterStatus;
    std::function<void (uint32_t msClockTickCount, uint32_t cycleClockTickCount, const ProtiumValues& values)> onProtiumValues;
    std::function<void (uint32_t msClockTickCount, uint32_t cycleClockTickCount, ProtiumOperatingState currentOperatingState, const ProtiumOperatingStateLogEntry (&operatingStateLogEntries)[8])> onProtiumOperatingState;


    bool processOctet(uint8_t octet)
    {
        if (_state != State::Start){
            if (_esc){
                _esc = false;

                if (octet == _specialCharSub){
                    octet = _specialChar;
                }else{
                    if (octet == _escCharSub){
                        octet = _escChar;
                    }else{
                        _state = State::Start;
                    }
                }
            }else{
                if (octet == _specialChar){
                    _state = State::Start;
                }else{
                    if (octet == _escChar){
                        _esc = true;
                        return false;
                    }
                }
            }
        }else{
            _esc = false;
        }

        switch(_state){
            case State::Start:{
                if (octet == _specialChar){
                    _state = State::Length;
                    _calcCrc.reset();
                    std::memset(_buffer, 0, sizeof(_buffer));
                }
                break;
            }

            case State::Length:{
                _calcCrc.process_byte(octet);

                _msgLength = octet;
                _writeIndex = 0;

                _state = State::Data;
                break;
            }

            case State::Data:{
                _calcCrc.process_byte(octet);

                if (_writeIndex < sizeof(_buffer))
                    _buffer[_writeIndex] = octet;

                if (++_writeIndex >= _msgLength)
                    _state = State::Crc1;

                break;
            }

            case State::Crc1:{
                _rcvdCrc = octet;
                _state = State::Crc2;
                break;
            }

            case State::Crc2:{
                _rcvdCrc |= uint32_t(octet) << 8;
                _state = State::Crc3;
                break;
            }

            case State::Crc3:{
                _rcvdCrc |= uint32_t(octet) << 16;
                _state = State::Crc4;
                break;
            }

            case State::Crc4:{
                _rcvdCrc |= uint32_t(octet) << 24;

                _state = State::Start;

                uint32_t crc = _calcCrc.checksum();

                if (_rcvdCrc == crc){
                    ++_goodMsgCount;                    
                    return processMessage();
                }else{
                    ++_badMsgCount;
                    std::cout << "HMI: Invalid crc - expected " << crc << ", received " << _rcvdCrc << std::endl;
                }
            }
        }

        return false;
    }



private:
    template <typename T>
    static constexpr auto getSize() -> std::enable_if_t<std::is_class_v<T> == false && std::is_array_v<T> == false, std::size_t>
    {
        return sizeof(T);
    }

    template <typename T>
    static constexpr auto getSize() -> std::enable_if_t<std::is_class_v<T> == true, std::size_t>
    {
        return sizeOfFields<T>();
    }

    template <typename T>
    static constexpr auto getSize() -> std::enable_if_t<std::is_array_v<T> == true, std::size_t>
    {
        return getSize<std::remove_reference_t<decltype(std::declval<T>()[0])>>() * std::extent_v<T, 0>;
    }



    template <typename T>
    auto retrieve(bool& success, std::size_t& readIndex, T& value) -> std::enable_if_t<std::is_class_v<T> == false && std::is_array_v<T> == false>
    {
        if (readIndex + sizeof(T) > _msgLength){
            success = false;
            return;
        }

        std::memcpy(&value, _buffer + readIndex, sizeof(T));
        readIndex += sizeof(T);
    }

    template <typename T>
    auto retrieve(bool& success, std::size_t& readIndex, T& value) -> std::enable_if_t<std::is_class_v<T> == true>
    {
        applyToFields(value, [&](auto& field){
            retrieve(success, readIndex, field);
        });
    }

    template <typename T>
    auto retrieve(bool& success, std::size_t& readIndex, T& value) -> std::enable_if_t<std::is_array_v<T> == true>
    {
        for (auto& element: value)
            retrieve(success, readIndex, element);        
    }


    template <typename... Args>
    auto receive(Args&... args) -> std::enable_if_t<((... + sizeof(Args)) < 255), bool>
    {
        std::size_t length = std::integral_constant<std::size_t, (... + getSize<Args>())>::value;

        if (_writeIndex < length || _msgLength < length || length > sizeof(_buffer))
            return false;

        std::size_t readIndex = 0;
        bool success = true;

        (... , retrieve(success, readIndex, args));

        return success;
    }

    bool processMasterMeasurementsMsg()
    {
        MasterMeasurements measurements = {};

        uint32_t messageId = 0;
        uint32_t msClockTickCount = 0;
        uint32_t cycleClockTickCount = 0;

        if (!receive(messageId, msClockTickCount, cycleClockTickCount, measurements)){
            std::cout << "HMI: Could not parse master measurements message" << std::endl;
            return false;
        }

        if (onMasterMeasurements)
            onMasterMeasurements(msClockTickCount, cycleClockTickCount, measurements);        

        return true;
    }

    bool processMasterStatusMsg()
    {
        MasterStatus status = {};

        uint32_t messageId = 0;
        uint32_t msClockTickCount = 0;
        uint32_t cycleClockTickCount = 0;

        if (!receive(messageId, msClockTickCount, cycleClockTickCount, status)){
            std::cout << "HMI: Could not parse master status message" << std::endl;
            return false;
        }

        if (onMasterStatus)
            onMasterStatus(msClockTickCount, cycleClockTickCount, status);

        return true;
    }

    bool processProtiumValuesMsg()
    {
        ProtiumValues values = {};

        uint32_t messageId = 0;
        uint32_t msClockTickCount = 0;
        uint32_t cycleClockTickCount = 0;

        if (!receive(messageId, msClockTickCount, cycleClockTickCount, values)){
            std::cout << "HMI: Could not parse protium values message";
            return false;
        }

        if (onProtiumValues)
            onProtiumValues(msClockTickCount, cycleClockTickCount, values);

        return true;
    }

    bool processProtiumOperatingStateMsg()
    {
        ProtiumOperatingStateLogEntry log[8] = {};
        ProtiumOperatingState currentOperatingState = {Disconnected};

        uint32_t messageId = 0;
        uint32_t msClockTickCount = 0;
        uint32_t cycleClockTickCount = 0;

        if (!receive(messageId, msClockTickCount, cycleClockTickCount, currentOperatingState, log)){
            std::cout << "HMI: Could not parse protium operating state message";
            return false;
        }

        if (onProtiumOperatingState)
            onProtiumOperatingState(msClockTickCount, cycleClockTickCount, currentOperatingState, log);

        return true;
    }

    bool processMessage()
    {
        uint32_t messageId = 0;

        if (!receive(messageId)){
            std::cout << "HMI: Message with no ID";
            return false;
        }

        switch(static_cast<HmiMessageId>(messageId)){
            case HmiMessageId::MasterMeasurements: return processMasterMeasurementsMsg();
            case HmiMessageId::MasterStatus: return processMasterStatusMsg();
            case HmiMessageId::ProtiumValues: return processProtiumValuesMsg();
            case HmiMessageId::ProtiumOperatingState: return processProtiumOperatingStateMsg();
        }

        std::cout << "HMI: Message with unknown ID: " << messageId << std::endl;
        return false;
    }




    enum struct HmiMessageId: uint32_t{
        MasterMeasurements      = 0xE7696EFE,
        MasterStatus            = 0x5D79ED90,
        ProtiumValues           = 0xB1236E43,
        ProtiumOperatingState   = 0x093C3952
    };

    enum class State: uint8_t{
        Start,
        Length,
        Data,
        Crc1,
        Crc2,
        Crc3,
        Crc4
    };  

    static constexpr uint8_t _specialChar = 0xFE;
    static constexpr uint8_t _escChar = 0xDE;
    static constexpr uint8_t _specialCharSub = 0xFC;
    static constexpr uint8_t _escCharSub = 0xDC;

    static constexpr std::size_t    _dataSize = 1024;
    boost::crc_32_type              _calcCrc;
    uint32_t                        _rcvdCrc = 0;
    State                           _state = State::Start;
    bool                            _esc = false;
    std::size_t                     _msgLength = 0;
    std::uint8_t                    _buffer[_dataSize] = {};
    std::size_t                     _writeIndex = 0;
    std::size_t                     _goodMsgCount = 0, _badMsgCount = 0;
};




#endif
