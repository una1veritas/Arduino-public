


#include "SCCB.h"
#include "delay.h"

/*
-----------------------------------------------
   機能: シミュレーションSCCBインタフェース初期化
   パラメータ: 無し
 戻り值: 無し
-----------------------------------------------
*/
void InitSCCB(void)
{
	SCCB_DDR|=(1<<SCCB_SIO_C)|(1<<SCCB_SIO_D);
	SCCB_PORT|=(1<<SCCB_SIO_C)|(1<<SCCB_SIO_D);
}
/*
-----------------------------------------------
   機能: startコマンド、SCCBの開始信号
   パラメータ: 無し
 戻り值: 無し
-----------------------------------------------
*/
void startSCCB(void)
{
	SIO_D_SET;     //データラインハイレベル
    delay_us(100);

    SIO_C_SET;	   //クロックハイの時、データラインはハイ→ロー
    delay_us(100);
 
    SIO_D_CLR;
    delay_us(100);

    SIO_C_CLR;	 //データラインローレベル、単動作関数
    delay_us(100);


}
/*
-----------------------------------------------
   機能: stopコマンド、SCCBの終了信号
   パラメータ: 無し
 戻り值: 無し
-----------------------------------------------
*/
void stopSCCB(void)
{
	SIO_D_CLR;
    delay_us(100);
 
    SIO_C_SET;
    delay_us(100);
  

    SIO_D_SET;
    delay_us(100);
 //   SCCB_DDR&= ~(SCCB_SIO_D);

}

/*
-----------------------------------------------
   機能: noAck、連続読み取り中の最後の終了周期
   パラメータ: 無し
 戻り值: 無し
-----------------------------------------------
*/
void noAck(void)
{
	
	SIO_D_SET;
	delay_us(100);
	
	SIO_C_SET;
	delay_us(100);
	
	SIO_C_CLR;
	delay_us(100);
	
	SIO_D_CLR;
	delay_us(100);

}

/*
-----------------------------------------------
   機能: 1バイトのデータをSCCB書き込み
   パラメータ:データ書き込み
 戻り值: 送信成功1戻り、送信失敗0戻り
-----------------------------------------------
*/
uchar SCCBwriteByte(uchar m_data)
{
	unsigned char j、tem;

	for(j=0;j<8;j++) //データ8回ループ送信
	{
		if((m_data<<j)&0x80)
		{
			SIO_D_SET;
		}
		else
		{
			SIO_D_CLR;
		}
		delay_us(100);
		SIO_C_SET;
		delay_us(100);
		SIO_C_CLR;
		delay_us(100);

	}
	delay_us(100);
	
	SIO_D_IN;/*SDAを入力に設定*/
	delay_us(100);
	SIO_C_SET;
	delay_us(1000);
	if(SIO_D_STATE)
	{
		tem=0;   //SDA=1送信失敗、0戻り
	}
	else
	{
		tem=1;   //SDA=0送信成功、1戻り
	}
	SIO_C_CLR;
	delay_us(100);	
    SIO_D_OUT;/*SDAを出力に設定*/

	return(tem);  
}

/*
-----------------------------------------------
   機能: 1バイトデータ読み取り、戻り
   パラメータ: 無し
 戻り值: 読み取りのデータ
-----------------------------------------------
*/
uchar SCCBreadByte(void)
{
	unsigned char read、j;
	read=0x00;
	
	SIO_D_IN;/*SDAを入力に設定*/
	delay_us(100);
	for(j=8;j>0;j--) //データ8回ループ受信
	{		     
		delay_us(100);
		SIO_C_SET;
		delay_us(100);
		read=read<<1;
		if(SIO_D_STATE) 
		{
			read=read+1;
		}
		SIO_C_CLR;
		delay_us(100);
	}
    SIO_D_OUT;/*SDAを出力に設定*/	
	return(read);
}
