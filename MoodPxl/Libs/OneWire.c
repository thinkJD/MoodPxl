//-------------------------------------------------------
// 1 Wire
// getestet mit DS 18B20 TempSensor
//-------------------------------------------------------

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include "OneWire.h"
#include "uart.h"


    uchar prescaler;
    uchar volatile second;			// count seconds


bit w1_reset(void)
{
  bit err;

  W1_OUT &= ~(1<<W1_PIN);
  W1_DDR |= 1<<W1_PIN;
  _delay_us(480);

  W1_DDR &= ~(1<<W1_PIN);
  _delay_us(66);
  err = W1_IN & (1<<W1_PIN);			// no presence detect

  _delay_us(480-66);
  if( (W1_IN & (1<<W1_PIN)) == 0 )		// short circuit
    err = 1;
  return err;
}
uchar w1_bit_io( bit b )
{

  W1_DDR |= 1<<W1_PIN;
  _delay_us(1);

  if( b )
    W1_DDR &= ~(1<<W1_PIN);
    _delay_us(15-1);

  if( (W1_IN & (1<<W1_PIN)) == 0 )
    b = 0;
    _delay_us(60-15);

  W1_DDR &= ~(1<<W1_PIN);

  return b;
}


uint w1_byte_wr( uchar b )
{
  uchar i = 8, j;
  do{
    j = w1_bit_io( b & 1 );
    b >>= 1;
    if( j )
      b |= 0x80;
  }while( --i );
  return b;
}


uint w1_byte_rd( void )
{
  return w1_byte_wr( 0xFF );
}


uchar w1_rom_search( uchar diff, uchar idata *id )
{
  uchar i, j, next_diff;
  bit b;

  if( w1_reset() )
    return PRESENCE_ERR;			// error, no device found
  w1_byte_wr( SEARCH_ROM );			// ROM search command
  next_diff = LAST_DEVICE;			// unchanged on last device
  i = 8 * 8;					// 8 bytes
  do{
    j = 8;					// 8 bits
    do{
      b = w1_bit_io( 1 );			// read bit
      if( w1_bit_io( 1 ) ){			// read complement bit
	if( b )					// 11
	  return DATA_ERR;			// data error
      }else{
	if( !b ){				// 00 = 2 devices
	  if( diff > i ||
	    ((*id & 1) && diff != i) ){
	    b = 1;				// now 1
	    next_diff = i;			// next pass 0
	  }
	}
      }
      w1_bit_io( b );     			// write bit
      *id >>= 1;
      if( b )					// store bit
	*id |= 0x80;
      i--;
    }while( --j );
    id++;					// next byte
  }while( i );
  return next_diff;				// to continue search
}

void w1_command( uchar command, uchar idata *id )
{
  uchar i;
  w1_reset();
  if( id ){
    w1_byte_wr( MATCH_ROM );			// to a single device
    i = 8;
    do{
      w1_byte_wr( *id );
      id++;
    }while( --i );
  }else{
    w1_byte_wr( SKIP_ROM );			// to all devices
  }
  w1_byte_wr( command );
}



void start_meas( void ){
  if( W1_IN & (1<< W1_PIN)){
    w1_command( CONVERT_T, NULL );
    W1_OUT |= 1<< W1_PIN;
    W1_DDR |= 1<< W1_PIN;			// parasite power on

  }else{
    uart1_puts("Short Circuit !");
  }
}


uint read_meas( void )
{
  uchar id[8], diff;
  uchar i;
  uint temp;

  for( diff = SEARCH_FIRST; diff != LAST_DEVICE; ){
    diff = w1_rom_search( diff, id );

    if( diff == PRESENCE_ERR ){
      //printf( "No Sensor found" );
      break;
    }
    if( diff == DATA_ERR ){
      //printf( "Bus Error" );
      break;
    }
    if( id[0] == 0x28 || id[0] == 0x10 ){	// temperature sensor
      //LCD_setpos(0,0);

      for( i = 0; i < 8; i++ ){
//	printf("%02X", id[i] );    ID

      }
      w1_byte_wr( READ );			// read command
      temp = w1_byte_rd();			// low byte
      temp |= (uint)w1_byte_rd() << 8;		// high byte
      if( id[0] == 0x10 )			// 9 -> 12 bit
        temp <<= 3;
      //LCD_setpos(1,0);

      return temp;

    }
  }
  return 0;
}
