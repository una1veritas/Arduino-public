
#include"english_16x8.h"
#include<lcd.h>
#include<delay.C>

void LCD_WR_DATA16(unsigned int data)//レジスタ書き込みデータ
{
     LCD_RD_H();
	 LCD_RS_H();
     LCD_CS_L();  
     
	 
     DATA_LCD_PORT=data>>8;
	 
	 LCD_WR_L();
     delay_us(1);
     LCD_WR_H();
	 
	 DATA_LCD_PORT=data&0XFF; 
     LCD_WR_L();
	 delay_us(1);
     
	 delay_us(1);
     LCD_WR_H();
     LCD_CS_H();
  
}
void LCD_WR_REG16(unsigned int index)//レジスタ書き込みコマンド
{
     LCD_RD_H();
	 LCD_RS_L();
     LCD_CS_L();
	 
	 DATA_LCD_PORT=index>>8;   
     LCD_WR_L();
	 delay_us(1);
     
	 delay_us(1);
     LCD_WR_H();
	 
	 DATA_LCD_PORT=index&0XFF; 
     LCD_WR_L();
	 delay_us(1);
     
	 delay_us(1);
     LCD_WR_H();
     LCD_CS_H();
}

void LCD_WR_REG(unsigned int index、unsigned int data)
{
     LCD_WR_REG16(index);
     LCD_WR_DATA16(data);
}
void LCD_Init(void)
{
	     LCD_PORT_INI();
	     LCD_RST_L();//ハードウェアリセット
delay_ms(2000); 	   
	     LCD_RST_H();
	//initializing funciton 1 
		 //LCD_WR_REG(0x00E5、0x8000);        
	    // LCD_WR_REG(0x00E7、0x0012);
	    //LCD_WR_REG(0x00Ef、0x1231); 	// Set the internal timing;    
		 LCD_WR_REG(0xe5、0x8000); 
         LCD_WR_REG(0x00、 0x0001);
		 delay_ms(200); 
         LCD_WR_REG(0x01、 0x0000); // set SS and SM bit
         LCD_WR_REG(0x02、 0x0700); // set 1 line inversion
        // LCD_WR_REG(0x03、 0x10B0); // set GRAM write direction and BGR=1.
#if   ID_AM==000       
	     LCD_WR_REG(0x0003、0x1000);//スクリーン回転制御 TFM=0、TRI=0、SWAP=1、16 bits system interface  swap RGB to BRG、ORGとHWM 为0
#elif ID_AM==001        
	     LCD_WR_REG(0x0003、0x1008);      
#elif ID_AM==010  
	     LCD_WR_REG(0x0003、0x1010);        
#elif ID_AM==011
	     LCD_WR_REG(0x0003、0x1018);
#elif ID_AM==100  
	     LCD_WR_REG(0x0003、0x1020);      
#elif ID_AM==101  
	     LCD_WR_REG(0x0003、0x1028);      
#elif ID_AM==110  
	     LCD_WR_REG(0x0003、0x1030);      
#elif ID_AM==111  
	     LCD_WR_REG(0x0003、0x1038);
#endif    
         LCD_WR_REG(0x04、 0x0000); // Resize register
         LCD_WR_REG(0x08、 0x0404); // set the back porch and front porch
         LCD_WR_REG(0x09、 0x0000); // set non-display area refresh cycle ISC[3:0]
         LCD_WR_REG(0x0A、 0x0000); // FMARK function
         LCD_WR_REG(0x0C、 0x0000); // RGB interface setting
         LCD_WR_REG(0x0D、 0x0000); // Frame marker Position
         LCD_WR_REG(0x0F、 0x0000); // RGB interface polarity
//Power On sequence //
         LCD_WR_REG(0x10、 0x0000); // SAP、 BT[3:0]、 AP、 DSTB、 SLP、 STB
         LCD_WR_REG(0x11、 0x0000); // DC1[2:0]、 DC0[2:0]、 VC[2:0]
         LCD_WR_REG(0x12、 0x0000); // VREG1OUT voltage
         LCD_WR_REG(0x13、 0x0000); // VDV[4:0] for VCOM amplitude
//	 LCD_WR_REG(0x07、 0x0001); // 
		 
         delay_ms(1000); // Dis-charge capacitor power voltage
         
         LCD_WR_REG(0x10、 0x1590); // SAP、 BT[3:0]、 AP、 DSTB、 SLP、 STB
         LCD_WR_REG(0x11、 0x0337); // DC1[2:0]、 DC0[2:0]、 VC[2:0]
         delay_ms(1000); // Delay 50ms
        
         LCD_WR_REG(0x12、 0x0028); // Internal reference voltage= Vci;
         delay_ms(1000); // Delay 50ms
         LCD_WR_REG(0x13、 0x1600); // Set VDV[4:0] for VCOM amplitude
         LCD_WR_REG(0x29、 0x002a); // Set VCM[5:0] for VCOMH
         LCD_WR_REG(0x2B、 0x000d); // Set Frame Rate
         delay_ms(1000); // Delay 50ms
         
#if   ID_AM==000         
	     LCD_WR_REG(0x0020、0x00ef);//GRAM水平開始位置
	     LCD_WR_REG(0x0021、0x013f);      
#elif ID_AM==001
	     LCD_WR_REG(0x0020、0x00ef);
	     LCD_WR_REG(0x0021、0x013f);      
#elif ID_AM==010
	     LCD_WR_REG(0x0020、0x0000);
	     LCD_WR_REG(0x0021、0x013f);      
#elif ID_AM==011
	     LCD_WR_REG(0x0020、0x0000);
	     LCD_WR_REG(0x0021、0x013f);       
#elif ID_AM==100
	     LCD_WR_REG(0x0020、0x00ef);
	     LCD_WR_REG(0x0021、0x0000);      
#elif ID_AM==101  
	     LCD_WR_REG(0x0020、0x00ef);
	     LCD_WR_REG(0x0021、0x0000);      
#elif ID_AM==110
	     LCD_WR_REG(0x0020、0x0000);
	     LCD_WR_REG(0x0021、0x0000);      
#elif ID_AM==111
	     LCD_WR_REG(0x0020、0x0000);
	     LCD_WR_REG(0x0021、0x0000);         
#endif  
// ----------- Adjust the Gamma Curve ----------//

        LCD_WR_REG(0x0030、 0x0101);
	     LCD_WR_REG(0x0031、 0x0506);
	     LCD_WR_REG(0x0032、 0x0101);
	     LCD_WR_REG(0x0035、 0x0204);
	     LCD_WR_REG(0x0036、 0x0707);
	     LCD_WR_REG(0x0037、 0x0707);
	     LCD_WR_REG(0x0038、 0x0406);
	     LCD_WR_REG(0x0039、 0x0707);
	     LCD_WR_REG(0x003C、 0x0402);
	     LCD_WR_REG(0x003D、 0x0707);
/*	
	LCD_WR_REG(0x0030、0x0000);
	LCD_WR_REG(0x0031、0x0507);
	LCD_WR_REG(0x0032、0x0104);
	LCD_WR_REG(0x0035、0x0105);
	LCD_WR_REG(0x0036、0x0404);
	LCD_WR_REG(0x0037、0x0603);
	LCD_WR_REG(0x0038、0x0104);
	LCD_WR_REG(0x0039、0x0107);
	LCD_WR_REG(0x003c、0x0501);
	LCD_WR_REG(0x003d、0x0707);
 */
		 
//------------------ Set GRAM area ---------------//
         LCD_WR_REG(0x0050、 0x0000); // Horizontal GRAM Start Address
         LCD_WR_REG(0x0051、 0x00EF); // Horizontal GRAM End Address
         LCD_WR_REG(0x0052、 0x0000); // Vertical GRAM Start Address
         LCD_WR_REG(0x0053、 0x013F); // Vertical GRAM Start Address
         LCD_WR_REG(0x0060、 0x2700); // Gate Scan Line
         LCD_WR_REG(0x0061、 0x0001); // NDL、VLE、 REV
         LCD_WR_REG(0x006A、 0x0000); // set scrolling line
//-------------- Partial Display Control ---------//

//-------------- Panel Control -------------------//
         LCD_WR_REG(0x90、 0x0010);
         LCD_WR_REG(0x92、 0x0000);
         LCD_WR_REG(0x93、 0x0003);
         LCD_WR_REG(0x95、 0x0110);
         LCD_WR_REG(0x97、 0x0000);
         LCD_WR_REG(0x98、 0x0000);

		 
		// LCD_WR_REG(0x07、 0x0031); // 262K color and display ON   0133
		 delay_ms(500); // Delay 5ms
		 
         LCD_WR_REG(0x07、 0x0173); // 262K color and display ON   0133
		 LCD_Clear(0);
		 delay_ms(500); // Delay 5ms
		 
		 LCD_BL_H();
  
}


/**********************************************
関数名：Lcdクリア関数
機能：Lcdのカラー指定
入口パラメータ：color 指定 、Lcdカラー RGB(5-6-5)
戻り值：無し
***********************************************/
void LCD_Clear(unsigned int Color)
{
  unsigned long index = 0;
  
  LCD_WR_REG(0x0020、0x0000);//GRAM水平開始位置
  LCD_WR_REG(0x0021、00000);      

  LCD_WR_REG(0x0050、0x00);//水平 GRAM開始位置
  LCD_WR_REG(0x0051、239);//水平GRAM終了位置
  LCD_WR_REG(0x0052、0x00);//垂直GRAM開始位置
  LCD_WR_REG(0x0053、319);//垂直GRAM終了位置 
  LCD_WR_REG16(0x0022);
  
  LCD_RD_H();
  LCD_RS_H();
  LCD_CS_L();   
  for(index = 0; index < 76800; index++)
  {
   // LCD_WR_DATA16(Color);
 
     LCD_WR_L();
     DATA_LCD_PORT=Color>>8; 
     LCD_WR_H();
     LCD_WR_L();
     DATA_LCD_PORT=Color&0XFF; 
     LCD_WR_H();
 
   
  } 
   LCD_CS_H();
}

void LCD_write_english(unsigned char data、unsigned int color、unsigned int xcolor)//文字書き込み
{

  unsigned char avl、i、n;
   LCD_WR_REG16(0x0022);  
  for (i=0;i<16;i++)
  { 
      avl=pgm_read_byte( &english[data-32][i]);
	  for (n=0;n<8;n++)
	   {
	     if(avl&0x80) LCD_WR_DATA16(color);
             else LCD_WR_DATA16(xcolor);
           
	     avl<<=1;
	   }
	}
}
void LCD_write_english_string(unsigned int x、unsigned int y、unsigned char *s、unsigned int color、unsigned int xcolor)//英語文字列表示
{
 unsigned int k=0;
 while (*s) 
  {
     Lcd_SetBox(y、x+k、15、8);
     LCD_write_english( *s、color、xcolor);
     k=k+8;
     s++;
  
  }

}  

void LCD_write_SUM(unsigned int x、unsigned int y、unsigned char SUM、unsigned int color、unsigned int xcolor)//書き込み数字
{

  unsigned char avl、i、n;
   Lcd_SetBox(y、x、15、8);
   LCD_WR_REG16(0x0022);  
  for (i=0;i<16;i++)
  { 
      avl=pgm_read_byte( &english[SUM+16][i]);
	  for (n=0;n<8;n++)
	   {
	     if(avl&0x80) LCD_WR_DATA16(color);
             else LCD_WR_DATA16(xcolor);
           
	     avl<<=1;
	   }
	}
}
/**********************************************
関数名：Lcdブロックセレクト関数
機能：Lcd指定を長方形エリア選定

注：xStartと yStartはスクリーンの回転により変更する、位置は長方形の４つの角

入口パラメータ：xStart x方向の開始ポイント
          ySrart y方向の終了ポイント
          xLong 選定する長方形のx方向長度
          yLong  選定する長方形のy方向長度
戻り值：無し
***********************************************/
void Lcd_SetBox(unsigned int xStart、unsigned int yStart、unsigned int xLong、unsigned int yLong)
{
  
#if ID_AM==000    
	LCD_SetCursor(xStart+xLong-1、312-yStart+yLong-1);

#elif ID_AM==001
	LCD_SetCursor(xStart+xLong-1、312-yStart+yLong-1);
     
#elif ID_AM==010
	LCD_SetCursor(xStart、312-yStart+yLong-1);
     
#elif ID_AM==011 
	LCD_SetCursor(xStart、312-yStart+yLong-1);
     
#elif ID_AM==100
	LCD_SetCursor(xStart+xLong-1、312-yStart);     
     
#elif ID_AM==101
	LCD_SetCursor(xStart+xLong-1、312-yStart);     
     
#elif ID_AM==110
	LCD_SetCursor(xStart、312-yStart); 
     
#elif ID_AM==111
	LCD_SetCursor(xStart、312-yStart);  
     
#endif
     
	LCD_WR_REG(0x0050、xStart);//水平 GRAM開始位置
	LCD_WR_REG(0x0051、xStart+xLong-1);//水平GRAM終了位置
	LCD_WR_REG(0x0052、312-yStart);//垂直GRAM開始位置
	LCD_WR_REG(0x0053、312-yStart+yLong-1);//垂直GRAM終了位置 
}


/*************************************************
関数名：Lcdカーソルスタートポイントロケート関数
機能：320240液晶上の1つのpointとデータの開始ポイントと指定する
入口パラメータ：x 座標 0~239
          y 座標 0~319
戻り值：無し
*************************************************/
void LCD_SetCursor(unsigned int Xpos、 unsigned int Ypos)
{
 // CLR_RS;
  LCD_WR_REG(0x20、 Xpos);
  LCD_WR_REG(0x21、 Ypos);
}

/*
void Test_tu(void)
{
   unsigned int i;
  // unsigned char  val;
  LCD_WR_REG(0x0020、20);//LcdカーソルGRAM水平開始位置
  LCD_WR_REG(0x0021、105); //Lcdカーソル垂直GRAM開始位置     

  LCD_WR_REG(0x0050、20);//水平 GRAM開始位置
  LCD_WR_REG(0x0051、207);//水平GRAM終了位置
  LCD_WR_REG(0x0052、105);//垂直GRAM開始位置
  LCD_WR_REG(0x0053、207);//垂直GRAM終了位置 
  LCD_WR_REG16(0x0022);
    	
   LCD_RD_H();
   LCD_RS_H();
   LCD_CS_L(); 
  for (i=0;i<21218;)
  { 

     LCD_WR_L();
	 //LCD_WR_L();
     DATA_LCD_PORT=pgm_read_byte(&gImage[i++]);
     LCD_WR_H();

     LCD_WR_L();
	 //LCD_WR_L();
     DATA_LCD_PORT=pgm_read_byte(&gImage[i++]);
     LCD_WR_H(); 

	}
   LCD_CS_H();
}

*/

 void  GUI_Point(unsigned int x、 unsigned int y、unsigned int color) 
 {
     LCD_WR_REG(0x0020、x);//LcdカーソルGRAM水平開始位置
     LCD_WR_REG(0x0021、y); //Lcdカーソル垂直GRAM開始位置     

   //  LCD_WR_REG(0x0050、x);//水平 GRAM開始位置
   //  LCD_WR_REG(0x0051、x);//水平GRAM終了位置
   //  LCD_WR_REG(0x0052、y);//垂直GRAM開始位置
   //  LCD_WR_REG(0x0053、y);//垂直GRAM終了位置 
	 LCD_WR_REG16(0x0022); 
	 LCD_WR_DATA16(color);   
	 
	 
 
 }

void Test(void)
{       
        unsigned long n、i;
       LCD_WR_REG(0x0020、0x0000);//GRAM水平開始位置
       LCD_WR_REG(0x0021、0x0000); 
        for(i=0;i<7;i++)
          
        {       
        
            
	        LCD_WR_REG(0x0050、0x00);//水平 GRAM開始位置
	        LCD_WR_REG(0x0051、239);//水平GRAM終了位置
	        LCD_WR_REG(0x0052、0x00);//垂直GRAM開始位置
	        LCD_WR_REG(0x0053、319);//垂直GRAM終了位置 
           
            LCD_WR_REG16(0x0022);       
                for(n=0;n<76800;n++)
                { 
                  
                  
                  if(i==0)LCD_WR_DATA16(BLUE);   
                  if(i==1)LCD_WR_DATA16(RED); 
                  if(i==2)LCD_WR_DATA16(GREEN);
                  if(i==3)LCD_WR_DATA16(CYAN); 
                  if(i==4)LCD_WR_DATA16(MAGENTA); 
                  if(i==5)LCD_WR_DATA16(YELLOW);
				  
                  if(i==6)LCD_WR_DATA16(BLACK); 
                }    
                 delay_ms(100);                   
        }       
}

