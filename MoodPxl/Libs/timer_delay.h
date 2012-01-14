//Diese Klasse stellt acht Timer mit einer 1sec
//Basis zur verfügung. Die Timer können unabhängig voneinander gesetzt werden.

extern void td_init();
extern void td_setTimer(uint8_t index, uint8_t sec);
extern uint8_t td_timeout(uint8_t index);
