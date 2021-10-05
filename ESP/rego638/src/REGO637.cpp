
#include "REGO637.h"

const char *RegoErrorMsgs[] =
{
    "Idle state",
    "Message timeout",
    "Waiting for message",
    "Message had wrong header",
    "Message had bad checksum",
    "Message complete."

};

//public stuff///////////////////////////////////////////////////////
void Rego637::start( uint32_t baudrate )
{
    if( _hwRegoPort )
    {
        _hwRegoPort->begin( baudrate, SERIAL_8N1, 16, 17 );
        _RegoStream = (Stream *)_hwRegoPort;

    }//if
    /*else
    {
        _swRegoPort->begin( baudrate );
        _RegoStream = (Stream *)_swRegoPort;

    }*///else

    ClrMsgStatus();

}//start

void Rego637::ReadSensor( uint16_t Sensor )
{
    ReadSysReg( Sensor );

}//ReadSensor

void Rego637::ReadDevice( uint16_t Device )
{
    ReadSysReg( Device );

}//ReadSensor

void Rego637::ReadControlData( uint16_t ControlData )
{
    ReadSysReg( ControlData );

}//ReadControlData

void Rego637::ReadSetting( uint16_t Setting )
{
    ReadSysReg( Setting );

}//ReadSetting

void Rego637::ClrMsgStatus( void )
{
    _statusRego = MSG_IDLE;
    _rxIdx = 0;
    flush();

}//ClrMsgStatus

uint8_t Rego637::GetMsgStatus( void )
{
    return( _statusRego );

}//GetStatus

uint8_t Rego637::CheckSensorMessage( int16_t *Value )
{
    uint8_t
        msgError;

    //check if we're even waiting for a message...
    if( _statusRego == MSG_IDLE )
        return _statusRego;

    RxShortMsgSensor();

    switch( _statusRego )
    {
        case    MSG_COMPLETE:
            *Value = _sensorValue;
            ClrMsgStatus();
            return MSG_COMPLETE;

        break;

        case    MSG_WAITING:
            if( (millis() - _timeMsgTimeout) >= REGO_MSG_TOUT )
            {
                ClrMsgStatus();
                return MSG_TIMEOUT;

            }//if

            return _statusRego;

        break;

        case    MSG_BADCHECKSUM:
        case    MSG_WRONGHEADER:
            msgError = _statusRego;
            ClrMsgStatus();
            return msgError;

        break;

    }//switch
    return 0;
}//CheckSensorMessage

uint8_t Rego637::CheckDeviceMessage( int16_t *Value )
{
    uint8_t
        msgError;

    //check if we're even waiting for a message...
    if( _statusRego == MSG_IDLE )
        return _statusRego;

    RxShortMsgDevice();

    switch( _statusRego )
    {
        case    MSG_COMPLETE:
            *Value = _deviceValue;
            ClrMsgStatus();
            return MSG_COMPLETE;

        break;

        case    MSG_WAITING:
            if( (millis() - _timeMsgTimeout) >= REGO_MSG_TOUT )
            {
                ClrMsgStatus();
                return MSG_TIMEOUT;

            }//if

            return _statusRego;

        break;

        case    MSG_BADCHECKSUM:
        case    MSG_WRONGHEADER:
            msgError = _statusRego;
            ClrMsgStatus();
            return msgError;

        break;

    }//switch
    return 0;
}//ChecDevicekMessage

uint8_t Rego637::CheckControlDataMessage( int16_t *Value )
{
    uint8_t
        msgError;

    //check if we're even waiting for a message...
    if( _statusRego == MSG_IDLE )
        return _statusRego;

    RxShortMsgControlData();

    switch( _statusRego )
    {
        case    MSG_COMPLETE:
            *Value = _controldataValue;
            ClrMsgStatus();
            return MSG_COMPLETE;

        break;

        case    MSG_WAITING:
            if( (millis() - _timeMsgTimeout) >= REGO_MSG_TOUT )
            {
                ClrMsgStatus();
                return MSG_TIMEOUT;

            }//if

            return _statusRego;

        break;

        case    MSG_BADCHECKSUM:
        case    MSG_WRONGHEADER:
            msgError = _statusRego;
            ClrMsgStatus();
            return msgError;

        break;

    }//switch
    return 0;
}//ChecControlDatakMessage

uint8_t Rego637::CheckRSettingMessage( int16_t *Value )
{
    uint8_t
        msgError;

    //check if we're even waiting for a message...
    if( _statusRego == MSG_IDLE )
        return _statusRego;

    RxShortMsgRSetting();

    switch( _statusRego )
    {
        case    MSG_COMPLETE:
            *Value = _rsettingValue;
            ClrMsgStatus();
            return MSG_COMPLETE;

        break;

        case    MSG_WAITING:
            if( (millis() - _timeMsgTimeout) >= REGO_MSG_TOUT )
            {
                ClrMsgStatus();
                return MSG_TIMEOUT;

            }//if

            return _statusRego;

        break;

        case    MSG_BADCHECKSUM:
        case    MSG_WRONGHEADER:
            msgError = _statusRego;
            ClrMsgStatus();
            return msgError;

        break;

    }//switch
    return 0;
}//ChecRSettingkMessage

//private stuff///////////////////////////////////////////////////////
void Rego637::flush( void )
{
    DEBUG_SERIAL.print( "Flushing RX" );
    while( _RegoStream->available() > 0 ) {
        DEBUG_SERIAL.print( _RegoStream->read(), HEX );
    }
    DEBUG_SERIAL.println();

}//flush

void Rego637::ReadSysReg( uint16_t ReadRegister )
{
    uint8_t
        checksum;

    if( _statusRego != MSG_IDLE )
        return;

    _TxBuff[0] = REGO637_ADDRESS;
    _TxBuff[1] = REGO637_CMD_READSYSREG;
    //pack register address
    _TxBuff[2] = (uint8_t)((ReadRegister >> 14) & 0x03);
    _TxBuff[3] = (uint8_t)((ReadRegister >> 7) & 0x7f);
    _TxBuff[4] = (uint8_t)(ReadRegister & 0x7f);
    //for read requests, data field is zero
    _TxBuff[5] = 0x00;
    _TxBuff[6] = 0x00;
    _TxBuff[7] = 0x00;
    //"checksum"
    checksum = _TxBuff[2];
    for( int i=3; i<8; i++ )
        checksum ^= _TxBuff[i];
    _TxBuff[8] = checksum;

    //debug message showing received message //<-- start here
     DEBUG_SERIAL.print( "Tx Bufferz: " );
    for( int i=0; i<9; i++ )
    {
        if( _TxBuff[i] < 0x10 )
             DEBUG_SERIAL.print( "0" );
         DEBUG_SERIAL.print( _TxBuff[i], HEX );
         DEBUG_SERIAL.print( " " );
    }//for
     DEBUG_SERIAL.println("");
    //debug 

    //before sending the message, flush the receive buffer
    flush();

    //TX message
    for( int i=0; i<9; i++ )
        _RegoStream->write( _TxBuff[i] );

    _statusRego = MSG_WAITING;
    _timeMsgTimeout = millis();

}//ReadSysReg

void Rego637::RxShortMsgSensor( void )
{
    uint8_t
        checksum;

    if( _statusRego == MSG_IDLE )
        return;

    if( _RegoStream->available() == 0 )
        return;

    _RxBuff[_rxIdx] = _RegoStream->read();
    DEBUG_SERIAL.println();
    DEBUG_SERIAL.print("RX GOT: ");
    DEBUG_SERIAL.print( _RxBuff[_rxIdx], HEX );
    DEBUG_SERIAL.println();
    _rxIdx++;
    if( _rxIdx < 5 )
        return;

    //complete message? check message health
    if( _RxBuff[0] != 0x01 )
    {
        _statusRego = MSG_WRONGHEADER;
        return;

    }//if

    //checksum
    checksum = _RxBuff[1];
    for( int i=2; i<4; i++ )
        checksum ^= _RxBuff[i];
    if( checksum != _RxBuff[4] )
    {
        _statusRego = MSG_BADCHECKSUM;
        return;           //checksum error

    }//if

    //message seems good...unpack the data
    //unpack the data field
    _sensorValue = 0;
    _sensorValue |= (_RxBuff[3] & 0x7f);
    _sensorValue |= ((_RxBuff[2] & 0x7f) << 7) & 0x3fff;
    _sensorValue |= ((_RxBuff[1] & 0x03) << 14);
    

    //debug message showing received message
     DEBUG_SERIAL.print( "Rx Buffer: " );
    for( int i=0; i<5; i++ )
    {
        if( _RxBuff[i] < 0x10 )
             DEBUG_SERIAL.print( "0" );
         DEBUG_SERIAL.print( _RxBuff[i], HEX );
         DEBUG_SERIAL.print( " " );
    }//for
     DEBUG_SERIAL.println("");
    
    _statusRego = MSG_COMPLETE;

}//RxMsgSensor

void Rego637::RxShortMsgDevice( void )
{
    uint8_t
        checksum;

    if( _statusRego == MSG_IDLE )
        return;

    if( _RegoStream->available() == 0 )
        return;

    _RxBuff[_rxIdx] = _RegoStream->read();
    _rxIdx++;
    if( _rxIdx < 5 )
        return;

    //complete message? check message health
    if( _RxBuff[0] != 0x01 )
    {
        _statusRego = MSG_WRONGHEADER;
        return;

    }//if

    //checksum
    checksum = _RxBuff[1];
    for( int i=2; i<4; i++ )
        checksum ^= _RxBuff[i];
    if( checksum != _RxBuff[4] )
    {
        _statusRego = MSG_BADCHECKSUM;
        return;           //checksum error

    }//if

    //message seems good...unpack the data
    //unpack the data field
    _deviceValue = 0;
    _deviceValue |= (_RxBuff[3] & 0x7f);
    _deviceValue |= ((_RxBuff[2] & 0x7f) << 7) & 0x3fff;
    _deviceValue |= ((_RxBuff[1] & 0x03) << 14);
    

    //debug message showing received message
     DEBUG_SERIAL.print( "Rx Buffer: " );
    for( int i=0; i<5; i++ )
    {
        if( _RxBuff[i] < 0x10 )
             DEBUG_SERIAL.print( "0" );
         DEBUG_SERIAL.print( _RxBuff[i], HEX );
         DEBUG_SERIAL.print( " " );
    }//for
     DEBUG_SERIAL.println("");
    
    _statusRego = MSG_COMPLETE;

}//RxMsgDevice

void Rego637::RxShortMsgControlData( void )
{
    uint8_t
        checksum;

    if( _statusRego == MSG_IDLE )
        return;

    if( _RegoStream->available() == 0 )
        return;

    _RxBuff[_rxIdx] = _RegoStream->read();
    _rxIdx++;
    if( _rxIdx < 5 )
        return;

    //complete message? check message health
    if( _RxBuff[0] != 0x01 )
    {
        _statusRego = MSG_WRONGHEADER;
        return;

    }//if

    //checksum
    checksum = _RxBuff[1];
    for( int i=2; i<4; i++ )
        checksum ^= _RxBuff[i];
    if( checksum != _RxBuff[4] )
    {
        _statusRego = MSG_BADCHECKSUM;
        return;           //checksum error

    }//if

    //message seems good...unpack the data
    //unpack the data field
    _controldataValue = 0;
    _controldataValue |= (_RxBuff[3] & 0x7f);
    _controldataValue |= ((_RxBuff[2] & 0x7f) << 7) & 0x3fff;
    _controldataValue |= ((_RxBuff[1] & 0x03) << 14);
    

    //debug message showing received message
     DEBUG_SERIAL.print( "Rx Buffer: " );
    for( int i=0; i<5; i++ )
    {
        if( _RxBuff[i] < 0x10 )
             DEBUG_SERIAL.print( "0" );
         DEBUG_SERIAL.print( _RxBuff[i], HEX );
         DEBUG_SERIAL.print( " " );
    }//for
     DEBUG_SERIAL.println("");
    
    _statusRego = MSG_COMPLETE;

}//RxMsgControlData

void Rego637::RxShortMsgRSetting( void )
{
    uint8_t
        checksum;

    if( _statusRego == MSG_IDLE )
        return;

    if( _RegoStream->available() == 0 )
        return;

    _RxBuff[_rxIdx] = _RegoStream->read();
    _rxIdx++;
    if( _rxIdx < 5 )
        return;

    //complete message? check message health
    if( _RxBuff[0] != 0x01 )
    {
        _statusRego = MSG_WRONGHEADER;
        return;

    }//if

    //checksum
    checksum = _RxBuff[1];
    for( int i=2; i<4; i++ )
        checksum ^= _RxBuff[i];
    if( checksum != _RxBuff[4] )
    {
        _statusRego = MSG_BADCHECKSUM;
        return;           //checksum error

    }//if

    //message seems good...unpack the data
    //unpack the data field
    _rsettingValue = 0;
    _rsettingValue |= (_RxBuff[3] & 0x7f);
    _rsettingValue |= ((_RxBuff[2] & 0x7f) << 7) & 0x3fff;
    _rsettingValue |= ((_RxBuff[1] & 0x03) << 14);
    

    //debug message showing received message
     DEBUG_SERIAL.print( "Rx Buffer: " );
    for( int i=0; i<5; i++ )
    {
        if( _RxBuff[i] < 0x10 )
             DEBUG_SERIAL.print( "0" );
         DEBUG_SERIAL.print( _RxBuff[i], HEX );
         DEBUG_SERIAL.print( " " );
    }//for
     DEBUG_SERIAL.println("");
    
    _statusRego = MSG_COMPLETE;

}//RxMsgControlData


