#include <Arduino.h>
#include "REGO637.h"

#define NUM_SENSORS 11
#define NUM_DEVICES 8
#define NUM_CONTROLDATA 7
#define NUM_SETTINGS 22

int readtoggle = 1;

const uint16_t RegsControlData[NUM_CONTROLDATA] =
    {
        REGO637_GT1_TGT_VALUE,
        REGO637_GT1_ON_VALUE,
        REGO637_GT1_OFF_VALUE,
        REGO637_GT3_ON_VALUE,
        REGO637_GT3_OFF_VALUE,
        REGO637_GT4_TGT_VALUE,
        REGO637_ADD_HEAT_PCT

};

const uint16_t Regs[NUM_SENSORS] =
    {
        REGO637_REG_RADRTN,
        REGO637_REG_OUTDOOR,
        REGO637_REG_HOTWATER,
        REGO637_REG_FORWARD,
        REGO637_REG_ROOM,
        REGO637_REG_COMPRESSOR,
        REGO637_REG_HEAT_FLUID_OUT,
        REGO637_REG_HEAT_FLUID_IN,
        REGO637_REG_COLD_FLUID_OUT,
        REGO637_REG_COLD_FLUID_IN,
        REGO637_REG_EXTRN_HOT_WATER

};

const uint16_t RegsDevices[NUM_DEVICES] =
    {
        REGO637_REG_GND_LOOP_PMP,
        REGO637_REG_COMPR_ST,
        REGO637_ADDNL_HT_3KW,
        REGO637_ADDNL_HT_6KW,
        REGO637_RADIATOR_PUMP,
        REGO637_HEAT_CARRIER_PUMP,
        REGO637_THREEWAY_VALVE,
        REGO637_ALARM

};

const uint16_t RegsSettings[NUM_SETTINGS] =
    {
        REGO637_SETTINGS_HEAT_CURVE,
        REGO637_SETTINGS_HEAT_CURVE_FINE_ADJ,
        REGO637_SETTINGS_HEAT_CURVE_COUPL_DIFF,
        REGO637_SETTINGS_HEAT_CURVE_TWO,
        REGO637_SETTINGS_HEAT_CURVE_TW_FINE_ADJ,
        REGO637_SETTINGS_ADJ_CURVE_20,
        REGO637_SETTINGS_ADJ_CURVE_15,
        REGO637_SETTINGS_ADJ_CURVE_10,
        REGO637_SETTINGS_ADJ_CURVE_5,
        REGO637_SETTINGS_ADJ_CURVE_0,
        REGO637_SETTINGS_ADJ_CURVE_MINUS_5,
        REGO637_SETTINGS_ADJ_CURVE_MINUS_10,
        REGO637_SETTINGS_ADJ_CURVE_MINUS_15,
        REGO637_SETTINGS_ADJ_CURVE_MINUS_20,
        REGO637_SETTINGS_ADJ_CURVE_MINUS_25,
        REGO637_SETTINGS_ADJ_CURVE_MINUS_30,
        REGO637_SETTINGS_ADJ_CURVE_MINUS_35,
        REGO637_SETTINGS_INDOOR_TEMP_SETTING,
        REGO637_SETTINGS_CURVE_INF_BY_INTEMP,
        REGO637_SETTINGS_SUMMER_DISCONNECTION,
        REGO637_SETTINGS_HOT_WATER_TARGET,
        REGO637_SETTINGS_HOT_WATER_HYSTERESIS

};

const char *ControlDataNames[NUM_CONTROLDATA] =
    {
        "GT1 Target value...:",
        "GT1 ON value.......:",
        "GT1 OFF value......:",
        "GT3 ON value.......:",
        "GT3 OFF value......:",
        "GT4 Target Value...:",
        "ADD HEAT OCT.......:"

};

const char *SensorNames[NUM_SENSORS] =
    {
        "Radiator Return....: ",
        "Outdoor............: ",
        "Hot water..........: ",
        "Forward............: ",
        "Room...............: ",
        "Compressor.........: ",
        "Heat fluid out.....: ",
        "Heat fluid in......: ",
        "Cold fluid out.....: ",
        "Cold fluid in......: ",
        "External hot water.: "

};

const char *DeviceNames[NUM_DEVICES] =
    {
        "Ground loop pump...: ",
        "Compressor on/off..: ",
        "Additional 3Kw.....: ",
        "Additional 6Kw.....: ",
        "Radiator pump [P1].: ",
        "Heat Carrier pump [P2].:",
        "3-way valve on/off.:",
        "Alarm..............:"

};

const char *SettingNames[NUM_SETTINGS] =
    {
        "Heat curve...................:",
        "Heat Curve fine adj..........:",
        "Heat curve coupl. diff.......:",
        "Heat curve two...............:",
        "Heat curve two fine adj......:",
        "Adjustment curve at 20.......:",
        "Adjustment curve at 15.......:",
        "Adjustment curve at 10.......:",
        "Adjustment curve at 5........:",
        "Adjustment curve at 0........:",
        "Adjustment curve at min 5....:",
        "Adjustment curve at min 10...:",
        "Adjustment curve at min 15...:",
        "Adjustment curve at min 20...:",
        "Adjustment curve at min 25...:",
        "Adjustment curve at min 30...:",
        "Adjustment curve at min 35...:",
        "Indoor temp setting..........:",
        "Curve infl. by intemp........:",
        "Summer Disconnection.........:",
        "Hot water setpoint value.....:",
        "Hot water hysteresis.........:"

};

Rego637 Heatpump(Serial);

byte
    nSensors;
byte
    nDevices;
byte
    nControlData;
byte
    nSettings;

void CheckPumpSensorMessage( void );
void CheckPumpDeviceMessage( void );
void CheckPumpControlDataMessage( void );
void CheckPumpSettingMessage( void );

void setup()
{
  // put your setup code here, to run once:
  Serial1.begin(115200);

  Serial1.println("Setting up heatpump..");
  Heatpump.start(19200);

  nSensors = 0;
  nDevices = 0;
  nControlData = 0;
  nSettings = 0;
}

void loop()
{
  static unsigned long timeRead = 0;

  if (millis() - timeRead > 1000)
  {
    timeRead = millis();

    if (readtoggle == 0)
    {
      if (Heatpump.GetMsgStatus() == MSG_IDLE)
        Heatpump.ReadSensor(Regs[nSensors]);
    }

    else if (readtoggle == 1)
    {
      if (Heatpump.GetMsgStatus() == MSG_IDLE)
        Heatpump.ReadDevice(RegsDevices[nDevices]);
    }
    else if (readtoggle == 2)
    {
      if (Heatpump.GetMsgStatus() == MSG_IDLE)
        Heatpump.ReadControlData(RegsControlData[nControlData]);
    }
    else if (readtoggle == 3)
    {
      if (Heatpump.GetMsgStatus() == MSG_IDLE)
        Heatpump.ReadSetting(RegsSettings[nSettings]);
    }
  }

  if (readtoggle == 0)
    CheckPumpSensorMessage();
  else if (readtoggle == 1)
    CheckPumpDeviceMessage();
  else if (readtoggle == 2)
    CheckPumpControlDataMessage();
  else if (readtoggle == 3)
    CheckPumpSettingMessage();
}

void CheckPumpSensorMessage( void )
{
    uint8_t
        chkRtn;
    int16_t
        sensorval;
        
    //while spinning in main, keep an eye out for a message
    chkRtn = Heatpump.CheckSensorMessage( &sensorval );
    switch( chkRtn )
    {
        case    MSG_COMPLETE:
            //message is complete; print what was received
            Heatpump.ClrMsgStatus();
            Serial1.print( SensorNames[nSensors] );
            Serial1.print( (float)sensorval/10.0, 1 );       //<-- change to display oC
            Serial1.println( "o C" );
            nSensors++;
            if( nSensors == NUM_SENSORS )
            {
                readtoggle = 1;
                nSensors = 0;
            }
        break;

        case    MSG_IDLE:
        case    MSG_WAITING:
            //do nothing if just waiting or in idle state...
            
        break;

        default:
            //something else is going on; print verbose message of what the problem is
            //print a message to the monitor identifying the error
            Serial1.print( "Message error.....: " );
            Serial1.println( RegoErrorMsgs[chkRtn] );
            
        break;
            
    }//switch
    
}//CheckSensorMessage

void CheckPumpDeviceMessage( void )
{
    uint8_t
        chkRtn;
    int16_t
        deviceval;
        
    //while spinning in main, keep an eye out for a message
    chkRtn = Heatpump.CheckDeviceMessage( &deviceval );
    switch( chkRtn )
    {
        case    MSG_COMPLETE:
            //message is complete; print what was received
            Heatpump.ClrMsgStatus();
            Serial1.print( DeviceNames[nDevices] );
            Serial1.println( deviceval );       //<-- change to display oC
             nDevices++;
            if( nDevices == NUM_DEVICES )
            {
                readtoggle = 2;
                nDevices = 0;
            }
        break;

        case    MSG_IDLE:
        case    MSG_WAITING:
            //do nothing if just waiting or in idle state...
            
        break;

        default:
            //something else is going on; print verbose message of what the problem is
            //print a message to the monitor identifying the error
            Serial1.print( "Message error.....: " );
            Serial1.println( RegoErrorMsgs[chkRtn] );
            
        break;
            
    }//switch
    
}//CheckSensorMessage

void CheckPumpControlDataMessage( void )
{
    uint8_t
        chkRtn;
    int16_t
        controldataval;
        
    //while spinning in main, keep an eye out for a message
    chkRtn = Heatpump.CheckControlDataMessage( &controldataval );
    switch( chkRtn )
    {
        case    MSG_COMPLETE:
            //message is complete; print what was received
            Heatpump.ClrMsgStatus();
            Serial1.print( ControlDataNames[nControlData] );
            Serial1.print( (float)controldataval/10.0, 1 );       //<-- change to display oC
            Serial1.println( "o C" );
            nControlData++;
            if( nControlData == NUM_CONTROLDATA )
            {
                readtoggle = 3;
                nControlData = 0;
            }
        break;

        case    MSG_IDLE:
        case    MSG_WAITING:
            //do nothing if just waiting or in idle state...
            
        break;

        default:
            //something else is going on; print verbose message of what the problem is
            //print a message to the monitor identifying the error
            Serial1.print( "Message error.....: " );
            Serial1.println( RegoErrorMsgs[chkRtn] );
            
        break;
            
    }//switch
    
}//CheckSensorMessage

void CheckPumpSettingMessage( void )
{
    uint8_t
        chkRtn;
    int16_t
        settingsval;
        
    //while spinning in main, keep an eye out for a message
    chkRtn = Heatpump.CheckRSettingMessage( &settingsval );
    switch( chkRtn )
    {
        case    MSG_COMPLETE:
            //message is complete; print what was received
            Heatpump.ClrMsgStatus();
            Serial1.print( SettingNames[nSettings] );
            Serial1.print( (float)settingsval/10.0, 1 );       //<-- change to display oC
            Serial1.println( "o C" );
            nSettings++;
            if( nSettings == NUM_SETTINGS )
            {
                readtoggle = 0;
                nSettings = 0;
            }
        break;

        case    MSG_IDLE:
        case    MSG_WAITING:
            //do nothing if just waiting or in idle state...
            
        break;

        default:
            //something else is going on; print verbose message of what the problem is
            //print a message to the monitor identifying the error
            Serial1.print( "Message error.....: " );
            Serial1.println( RegoErrorMsgs[chkRtn] );
            
        break;
            
    }//switch
    
}//CheckSettingsMessage
