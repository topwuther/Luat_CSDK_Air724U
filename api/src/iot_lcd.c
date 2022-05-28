#include "iot_lcd.h"


/**写入 lcd命令
*@param		cmd: 命令
**/
VOID iot_lcd_write_cmd(                          
                        UINT8 cmd 
                   )
{
    return  IVTBL(send_color_lcd_command)(cmd);
}

/**lcd 写入lcd数据 
*@param	 	data: 数据
**/
VOID iot_lcd_write_data(                               
                        UINT8 data                
                )
{
    return IVTBL(send_color_lcd_data)(data);
}

/**lcd初始化
*@param		param: lcd初始化参数
*@return	TRUE: 	    成功
*           FALSE:      失败
**/	
BOOL iot_lcd_color_init(T_AMOPENAT_COLOR_LCD_PARAM *param )
{
    return IVTBL(init_color_lcd)(  param );
}

/**  刷新lcd
*@param		rect: 需要刷新的区域
*@param		pDisplayBuffer: 刷新的缓冲区
*@return	TRUE: 	    成功
*           FALSE:      失败
**/
VOID iot_lcd_update_color_screen(
				T_AMOPENAT_LCD_RECT_T* rect,        /* 需要刷新的区域 */
				UINT16 *pDisplayBuffer    )
{
    IVTBL(update_color_lcd_screen)(                       
                            rect,      
                            pDisplayBuffer       
                                   );
}
/** 解码jpg格式图片
*@param		buffer: 缓存显示buffer
*@param		len: 缓存显示buffer长度
*@param		imageinfo: 文件格式
*@return	INT32: 解码状态码
**/
INT32 iot_decode_jpeg(
                    UINT8 * buffer,
                    UINT32 len, 
                    T_AMOPENAT_IMAGE_INFO * imageinfo
                    )
{
    return IVTBL(ImgsDecodeJpeg)(buffer, len, imageinfo);
}

/** 释放jpg格式解码数据
*@param		imageinfo: 文件格式
*@return	INT32: 释放状态码
**/
INT32 iot_free_jpeg_decodedata(
                    T_AMOPENAT_IMAGE_INFO * imageinfo
                    )
{
    return IVTBL(ImgsFreeJpegDecodedata)(imageinfo);
}
    

