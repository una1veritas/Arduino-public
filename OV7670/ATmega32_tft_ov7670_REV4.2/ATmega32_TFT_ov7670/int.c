

#include "int.h"
#include "lcd.h"
volatile unsigned char val1=0、val2=0;

void Init_INT0(void)
{
	SREG |= 0x80 ;//グローバル割り込み有効
	MCUCR |= 0x02 ;//立ち下がりエッジ割り込み
	GICR=(1<<INT0) ;//
	
}


SIGNAL( SIG_INTERRUPT0 )
{
	cli();
	if(PIND&0X20){val2++;if(val2>100)val2=100;}
	else if(val2>5)
	{
	 if(val1)val1=0;
	 else val1=1;
	 val2=0;
    }
	
	
    if(val1==0)
	{
	// val1=0;
	 DISEN_EXCLK;//外部CLK禁止、SCMWRとTFT WR接続	
	 DISEN_245;//外部データライン禁止、SCMデータとTFTデータライン接続	
     DATA_OUPUT();//データライン出力、TFTと接続
	 LCD_WR_REG(0x0020、239);//
	 LCD_WR_REG(0x0021、0);//
	 LCD_WR_REG(0x0050、0x00);//水平 GRAM開始位置
	 LCD_WR_REG(0x0051、239);//水平GRAM終了位置
	 LCD_WR_REG(0x0052、0x00);//垂直GRAM開始位置
	 LCD_WR_REG(0x0053、319);//垂直GRAM終了位置 
	 LCD_WR_REG(0x0003、0x1028); 

	 LCD_WR_REG16(0x0022);
	 
     
	 DATA_INPUT();//データライン出力、准备连接外部データライン到TFT	
	 LCD_RS_H();//データ有効
	 LCD_WR_L();//外部クロック有効
	 EN_EXCLK;//外部CLK有効
	 EN_245;//外部データライン有効、TFTデータラインと接続	
	 LCD_CS_L();//データ有効
	 }
	 else 
	 {

	 DISEN_EXCLK;//外部CLK禁止、SCMWRとTFT WR接続	
	 DISEN_245;//外部データライン禁止、SCMデータとTFTデータライン接続	

	 
	 }
	
	 sei();
}
