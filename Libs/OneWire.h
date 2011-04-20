#ifndef ONEWIRE_H_INCLUDED
#define ONEWIRE_H_INCLUDED



#endif // ONEWIRE_H_INCLUDED

 #define uchar unsigned char
    #define uint unsigned int
    #define bit uchar
    #define idata
    #define code

    #define DEBOUNCE	256L		// debounce clock (256Hz = 4msec)


    #define MATCH_ROM	0x55
    #define SKIP_ROM	0xCC
    #define	SEARCH_ROM	0xF0

    #define CONVERT_T	0x44		// DS1820 commands
    #define READ		0xBE
    #define WRITE		0x4E
    #define EE_WRITE	0x48
    #define EE_RECALL	0xB8

    #define	SEARCH_FIRST	0xFF		// start new search
    #define	PRESENCE_ERR	0xFF
    #define	DATA_ERR	0xFE
    #define LAST_DEVICE	0x00


    #define _delay_h_


    #define W1_PIN	PD5
    #define W1_IN	PIND
    #define W1_OUT	PORTD
    #define W1_DDR	DDRD


void start_meas( void );

uint read_meas( void );
