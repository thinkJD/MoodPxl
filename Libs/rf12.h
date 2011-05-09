extern unsigned short rf12_trans(unsigned short wert);					
extern void rf12_init(void);											
extern void rf12_setfreq(unsigned short freq);							
extern void rf12_setbaud(unsigned short baud);							
extern void rf12_setpower(unsigned char power, unsigned char mod);		
extern void rf12_setbandwidth(unsigned char bandwidth, unsigned char gain, unsigned char drssi);	
extern void rf12_txdata(unsigned char *data, unsigned char number);		
extern void rf12_rxdata();												
extern void rf12_ready(void);											
void rf12_getData(uint8_t* text);
void rf_data_reset();
uint8_t rf12_getStatus();
void rf12_Poll();

#define RF12FREQ(freq)	((freq-430.0)/0.0025)						

typedef struct 
{
	char Status;
	uint8_t Length;
	uint8_t Count;
    char Adress;      
    unsigned char Data[10]; // Text, maximal TEXT_LEN Zeichen lang
} rf_data;

#define rf12_data_status_empty 0	//Puffer bereit sum empfang
#define rf12_data_status_nio 	1	//Header oder Datencheck nio
#define rf12_data_status_ready 2	//Puffer bereit zum lesen
#define rf12_data_status_progress 3	//Mudul empfängt gerade
