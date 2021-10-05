///////////////////////////////////////////////////////////////////////////////////
//  Filename    : REGO637.h
//  Date created: 03/02/2019
//  Comments    : Class for basic communications with REGO637 heatpump
//
//  Programmer  : Blackfin
//
//  Mod History :
//      03/02/2019 - supports only short-reply sensor reads
//		06/02/2019 - Added device registers and Control Data registers, by SX3000
//		07/03/2019 - Added Settings registers, by SX3000
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef _REGO637_03022019
#define _REGO637_03022019

#include <HardwareSerial.h>
//#include <SoftwareSerial.h>
#include "Arduino.h"

#define DEBUG_SERIAL Serial1

#define REGO_MSG_TOUT               2000ul   //2000mS timeout waiting for reply from heatpump

//RX/TX status
#define MSG_IDLE            0
#define MSG_TIMEOUT         1
#define MSG_WAITING         2
#define MSG_WRONGHEADER     3
#define MSG_BADCHECKSUM     4
#define MSG_COMPLETE        5

extern const char *RegoErrorMsgs[];

#define REGO637_ADDRESS             0x81

//commands
#define REGO637_CMD_READFRNTPNL     0x00
#define REGO637_CMD_WRITEFRNTPNL    0x01
#define REGO637_CMD_READSYSREG      0x02
#define REGO637_CMD_WRITESYSREG     0x03
#define REGO637_CMD_READSYSTIMERS   0x04
#define REGO637_CMD_WRITESYSTIMERS  0x05
//
//
#define REGO637_CMD_READFROMDISP    0x20
#define REGO637_CMD_READLSTERRLINE  0x40
#define REGO637_CMD_READPREVERRLINE 0x42
#define REGO637_CMD_READREGOVER     0x7F

//control data registers
#define REGO637_GT1_TGT_VALUE       0x006E
#define REGO637_GT1_ON_VALUE        0x006F
#define REGO637_GT1_OFF_VALUE       0x0070
#define REGO637_GT3_ON_VALUE        0x0073
#define REGO637_GT3_OFF_VALUE       0x0074
#define REGO637_GT4_TGT_VALUE       0x006D
#define REGO637_ADD_HEAT_PCT        0x006C

//devices
#define REGO637_REG_GND_LOOP_PMP    0x01FD
#define REGO637_REG_COMPR_ST        0x01FE
#define REGO637_ADDNL_HT_3KW        0x01FF
#define REGO637_ADDNL_HT_6KW        0x0200
#define REGO637_RADIATOR_PUMP       0x0203
#define REGO637_HEAT_CARRIER_PUMP   0x0204
#define REGO637_THREEWAY_VALVE      0x0205
#define REGO637_ALARM               0x0206

//sensors
#define REGO637_REG_RADRTN          0x0209
#define REGO637_REG_OUTDOOR         0x020A
#define REGO637_REG_HOTWATER        0x020B
#define REGO637_REG_FORWARD         0x020C
#define REGO637_REG_ROOM            0x020D
#define REGO637_REG_COMPRESSOR      0x020E
#define REGO637_REG_HEAT_FLUID_OUT  0x020F
#define REGO637_REG_HEAT_FLUID_IN   0x0210
#define REGO637_REG_COLD_FLUID_IN   0x0211
#define REGO637_REG_COLD_FLUID_OUT  0x0212
#define REGO637_REG_EXTRN_HOT_WATER 0x0213

//settings
#define REGO637_SETTINGS_HEAT_CURVE					0x0000
#define REGO637_SETTINGS_HEAT_CURVE_FINE_ADJ		0x0001
#define REGO637_SETTINGS_HEAT_CURVE_COUPL_DIFF		0x0002
#define REGO637_SETTINGS_HEAT_CURVE_TWO				0x0003
#define REGO637_SETTINGS_HEAT_CURVE_TW_FINE_ADJ		0x0004
#define REGO637_SETTINGS_ADJ_CURVE_20				0x001e
#define REGO637_SETTINGS_ADJ_CURVE_15				0x001c
#define REGO637_SETTINGS_ADJ_CURVE_10				0x001a
#define REGO637_SETTINGS_ADJ_CURVE_5				0x0018
#define REGO637_SETTINGS_ADJ_CURVE_0				0x0016
#define REGO637_SETTINGS_ADJ_CURVE_MINUS_5			0x0014
#define REGO637_SETTINGS_ADJ_CURVE_MINUS_10			0x0012
#define REGO637_SETTINGS_ADJ_CURVE_MINUS_15			0x0010
#define REGO637_SETTINGS_ADJ_CURVE_MINUS_20			0x000e
#define REGO637_SETTINGS_ADJ_CURVE_MINUS_25			0x000c
#define REGO637_SETTINGS_ADJ_CURVE_MINUS_30			0x000a
#define REGO637_SETTINGS_ADJ_CURVE_MINUS_35			0x0008
#define REGO637_SETTINGS_INDOOR_TEMP_SETTING		0x0021
#define REGO637_SETTINGS_CURVE_INF_BY_INTEMP		0x0022
#define REGO637_SETTINGS_SUMMER_DISCONNECTION		0x0024
#define REGO637_SETTINGS_HOT_WATER_TARGET			0x002b
#define REGO637_SETTINGS_HOT_WATER_HYSTERESIS		0x002c


class Rego637
{

public:
    Rego637( HardwareSerial &sp ) {_hwRegoPort = &sp;}
//    Rego637( SoftwareSerial &sp ) {_swRegoPort = &sp;}

    ~Rego637(){;}

    void start( uint32_t baudrate );
    void ReadSensor( uint16_t Sensor );
    void ReadDevice( uint16_t Device );
    void ReadControlData( uint16_t ControlData );
	void ReadSetting( uint16_t Rsetting );
    void ClrMsgStatus( void );
    uint8_t GetMsgStatus( void );
    uint8_t CheckSensorMessage( int16_t *Value );
    uint8_t CheckDeviceMessage( int16_t *Value );
    uint8_t CheckControlDataMessage( int16_t *Value );
	uint8_t CheckRSettingMessage( int16_t *Value );

private:
    uint8_t         _rxIdx;
    int16_t         _sensorValue;
    int16_t         _deviceValue;
    int16_t         _controldataValue;
    int16_t         _rsettingValue;
    uint8_t         _statusRego;
    uint32_t        _timeMsgTimeout;
    uint8_t         _TxBuff[10];    //seems like Tx msg is 9 bytes
    uint8_t         _RxBuff[50];    //long messages could be 42 chars

    void flush( void );
    void ReadSysReg( uint16_t ReadRegister );
    void RxShortMsgSensor( void );
    void RxShortMsgDevice( void );
    void RxShortMsgControlData( void );
	void RxShortMsgRSetting( void );

    Stream          *_RegoStream;
    HardwareSerial  *_hwRegoPort;
//    SoftwareSerial  *_swRegoPort;

};//class Rego637


#endif // _REGO637_03022019
