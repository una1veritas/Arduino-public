



#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "ov7670.H"
#include "int.H"
#include "delay.h"
#include "FAT.h"
#include "config.h"

unsigned int image_count = 0 、image_all = 0 ;



extern uint16 SectorsPerClust;//每クラスタセクタ数

extern uint8  FAT32_Enable;

void disp_image(void)
  {
    WORD count= 1 、i =0   ;
	uint8 *buffer ;
	uint8 type 、x 、 rgb、first 、color_byte、byte1、byte2、byte3 ;
	uint16 y 、color、tmp_color ;
    uint32 p;				//クラスタ指示值			//cluster
	type = 1 ;
	struct FileInfoStruct FileInfo;//ファイル情報

    struct direntry BMPInfo;	//再生するmp3ファイル情報		//the mp3 file item whichi will be played
	
	 LCD_WR_REG(0x0050、0);//水平 GRAM開始位置
     LCD_WR_REG(0x0051、239);//水平GRAM終了位置
     LCD_WR_REG(0x0052、0);//垂直GRAM開始位置
     LCD_WR_REG(0x0053、319);//垂直GRAM終了位置 
	
	if( image_all == 0 )    //イメージ総数読み取り
	  {
         image_count =  0 ;
	     byte1 = Search(&BMPInfo、&image_count、&type) ;
		 image_all = image_count ;		 
		 image_count = 1 ;
		 		 
		 /*
	     image_count = 0 ;
         Search(&BMPInfo、&image_count、&type) ;
		 image_all = image_count ;
		 GUI_sprintf_chartohex(10、300、0x00、0xffff、0x0000) ;
		 GUI_sprintf_chartohex(30、300、image_count、0xffff、0x0000) ;
		 GUI_sprintf_chartohex(60、300、image_all、0xffff、0x0000) ;
		 */
	  }
	else  
      {		
          byte1 = Search(&BMPInfo、&image_count、&type) ;
      }	
	 
	 
	 
	p = BMPInfo.deStartCluster+(((uint32)BMPInfo.deHighClust)<<16);//ファイルヘッダクラスタ読み取り	//the first cluster of the file
	
	x = 240 ;
	y = 0 ;
	rgb = 0 ;
	count = 0 ;
	first = 0 ;
	buffer=malloc(512);	
	while(1)
	 {
		i = 0 ;
	
	    for( ; i < SectorsPerClust ; i++ )      //クラスタ
	     {
	 
	        FAT_LoadPartCluster(p、i、buffer);//1つセクタ読み取り	//read a sector
	   
	        if(i==0 && first==0) { count= buffer[0x0a] ; color_byte = buffer[0x1c] / 8  ;  first = 1 ; } 
	        else { count=0 ; }
	   
	        while(count<512)  //１クラスタ512セクタ読み取り (SectorsPerClust 每クラスタセクタ数)
	          {
		   
		        if(color_byte == 3)   //24ビットカラーイメージ
		          {
                      switch ( rgb ) 
                        {
			               case 0 : tmp_color = buffer[count]>>3 ;
			                        color |= tmp_color;
					                break ;
					  
			               case 1 : tmp_color = buffer[count]>>2 ;
			                        tmp_color <<= 5 ;
			                        color |= tmp_color ;
					                break ;
					   
       			           case 2 : tmp_color = buffer[count]>>3 ;
			                        tmp_color <<= 11 ;
			                        color |= tmp_color ;
					                 break ;			
			            }
			
                      rgb ++ ;
			      }
		        else
		          {
			          if(color_byte==2)  //16ビットカラーイメージ
				        {
				          switch ( rgb )
					        {
					          case 0 : byte1 = buffer[count] ;
								       break ; 
								   
						      case 1 :    
								       color = buffer[count] ;
								       color<<=8 ;
								       color |= byte1 ;
								       break ;
					     
					        }
					      rgb ++ ;
				    
				        }
				      else 
				        {
				          if(color_byte==4) //32ビットカラーイメージ
				            {
				              switch ( rgb )
					            {
					              case 0 :  byte1 = buffer[count] ;
								            break ; 
								   
						          case 1 :  byte2 = buffer[count] ;
                                            break ;
									   
							      case 2 :  byte3 = buffer[count] ;
							                break ;
									   
							      case 3 :  tmp_color = byte1 >> 3 ;
			                                color |= tmp_color;
									        tmp_color = byte2 >>2 ;
			                                tmp_color <<= 5 ;
			                                color |= tmp_color ;
									        tmp_color = byte3 >>3 ;
			                                tmp_color <<= 11 ;
			                                color |= tmp_color ;
									        break ;
					     
					            }
					           rgb ++ ;
				    
				            }
				     
				        }   
			 
			       }
           
		   count ++ ;
		   
		   if(rgb == color_byte)        //1ピクセルデータ読み取り・表示
		     {
			   // GUI_Point(x、 y、color) ;
				
				    LCD_WR_REG(0x0020、239-x);//LcdカーソルGRAM水平開始位置
                    LCD_WR_REG(0x0021、y); //Lcdカーソル垂直GRAM開始位置     

	                LCD_WR_REG16(0x0022); 
	                LCD_WR_DATA16(color);
				
				color = 0x00 ;
			    y++ ;
			    if(y>=320)
				  {
				    x-- ;
					
					if( x<=0 )
		              {
					    
						free(buffer) ;
			            return ;
			          }
					y = 0 ;
				  }
		        
				rgb = 0 ;
			 }
			 
			
		}
	   
	 }  // 1クラスタデータ読み取り
	 
	 free(buffer) ;
	 p=FAT_NextCluster(p);//次の1クラスタデータ読み取り			//read next cluster
	 buffer=malloc(512);	
	
	if(p == 0x0fffffff || p == 0x0ffffff8 || (FAT32_Enable == 0 && p == 0xffff))//次のクラスタがない場合、終了、	//no more cluster
		{
		        free(buffer) ;
				return ;
		}
	 
  }
		
	free(buffer) ;
  
 }


//CPU初期化//
int main(void) 
{
  unsigned char i、 x、retry=0、kn=0、Sensor_ID;
  uint8 ok、er、r1 ;
  uint32 capacity ;
  unsigned int y ;;
	EXCLK_DDR|=1<<EXCLK_BIT ;//en_exclk ピン出力
	HC245_OE_DDR|=1<<HC245_OE_BIT;//en_245ピン出力
	DDRD&=~(0x20);
	PORTD|=0X20;
	
	DISEN_EXCLK;//外部CLK禁止、SCMWRとTFT WR接続	
	DISEN_245;//外部データケーブル禁止、SCMデータとTFTデータケーブル接続
	delay_ms(3000);
	DATA_OUPUT(); //データケーブル出力、TFTと接続
    LCD_Init();
	MMC_SD_Init();//spiインタフェース初期化
	
	//GUI_Point(0、 0、0xffff) ;
	LCD_write_english_string(20、150、"Guanfu_wang  20110412"、RED、BLACK);
	LCD_write_english_string(20、170、"Atmega32 & ILI9328 FOR OV7670 REV4.2"、GREEN、BLACK);
	
	if(PIND&0X20)
	{
	   kn=0;
		LCD_write_english_string(20、190、"OV7670 Init......"、BLUE、BLACK);
	
	   while(1!=OV7670_init());//ov7660初期化
	  // while(1!=rdOV7670Reg(0x0b、 &Sensor_ID));//ID読み取り
	//delay_ms(1000);
	   LCD_write_english_string(20、190、"OV7670 Init  0K  "、RED、BLACK);
	   delay_ms(1000);
	   LCD_Clear(0);
	   
       DATA_INPUT(); 
	   Init_INT0();
	
	}
	else
	{
	  kn=1;
	  LCD_write_english_string(20、190、"TF Card Init......"、BLUE、BLACK);
	//OSCCAL = 0x00;//最小RC振動周波数
	   while(MMC_SD_Reset()&(retry<21))//sdカード初期化					//sd card initialize
	   {
		  retry++;
		  if(retry>20)
		  {
             LCD_write_english_string(20、190、"TF Card Init....NG"、BLUE、BLACK);
		  
		  }	
	   } 
	   LCD_write_english_string(20、190、"TF Card Init....OK"、BLUE、BLACK);  
	   LCD_write_english_string(20、210、"FAT Init......"、BLUE、BLACK);
	   while(FAT_Init()){}//初期化ファイルシステム、 FAT16とFAT32初期化	//initialize file system  FAT16 and FAT32 are supported
       SearchInit();  //サーチファイル初期化
	   LCD_write_english_string(20、210、"FAT Init....OK"、BLUE、BLACK);
	}
	

    while(1)
    {	
	    if(kn)
		{
		 disp_image() ;
		 delay_ms(20000) ;
		 delay_ms(20000) ;
		                              //Print_hz(72、30、16、"漢字表示テスト"、0xffff、0x0000) ;
		 if(image_count<image_all)
		    {
		      image_count ++ ;
		    }
		 else 
		    {
			  image_count = 1 ;
			} 
	     }
		 /*
		 else if(0==rdOV7670Reg(0x0b、 &Sensor_ID))//ID読み取り
		 {
		 
		  cli();
	      DISEN_EXCLK;//外部CLK禁止、SCMWRとTFT WR接続	
	      DISEN_245;//外部データケーブル禁止、SCMデータとTFTデータケーブル接続
	      DATA_OUPUT(); //データケーブル出力、TFTと接続
	      LCD_WR_REG(0x0003、0x1018);
	      LCD_Clear(0);
	      LCD_write_english_string(20、100、"Please install the sensor board..."、BLUE、BLACK);
	       while(1!=OV7670_init());//センサーモジュール初期化
          LCD_write_english_string(20、100、"Sensor OV7670 Init  0K            "、RED、BLACK);
	      delay_ms(3000);
	       LCD_Clear(0);
          DATA_INPUT(); 
	  
		   sei();
		 }
		*/ 
	
	}

}

