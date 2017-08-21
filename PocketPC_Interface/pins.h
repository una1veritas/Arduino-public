/*
 * Da Vinci 32U pin assign by Arduino Micro Bootloader
 * From USB connecter, counter clockwise
 * 1. GND
 * 2. Vcc 5V
 * 3. Reset
 * 4. D3 		PD0 SCL
 * 5. D2  		PD1 SDA
 * 6. D0  		PD2 RxD1
 * 7. D1  		PD3 TxD1
 * 8. D4 		PD4
 * 9. NA  		PD5 ~CTS (Tx LED)
 * 10. D12/A11 	PD6
 * 11. D6/A7	PD7
 * 12. D5   	PC6
 * 13. D13 (LED) PC7
 * 14. D23/A5	PF0
 * 15. D22/A4 	PF1
 * 16.    		PE2 ~HWB
 *
 * 17. GND
 * 18. D21/A3 		PF4
 * 19. D20/A2		PF5
 * 20. D19/A1		PF6
 * 21. D18/A0  		PF7
 * 22. D17			PB0 SS
 * 23. D15 			PB1 SCLK
 * 24. D16 			PB2 MOSI
 * 25. D14 			PB3 MISO
 * 26. D8/A8 		PB4
 * 27. D9/A9 		PB5
 * 28. D10/A10 		PB6
 * 29. D11 			PB7 ~RTS
 * 30. D7 			PE6
 * 31. Vref
 * 32. GND
 */

const int PD6_CASOUT = 12; // --> XIN
const int PD7_CASIN = 6;   // <-- XOUT PocketPC/Interface
const int PC7_LED = 13;

const int PF5_SD_CS = 20;
const int PC6_SRAM_CS = 5;
const int PF7_SRAM_HOLD = 18;


