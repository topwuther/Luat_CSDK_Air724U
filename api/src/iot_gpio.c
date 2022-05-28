#include "iot_gpio.h"

/**配置gpio 
*@param		port:		GPIO编号
*@param		cfg:		配置信息
*@return	TRUE: 	    成功
*           FALSE:      失败
**/
BOOL iot_gpio_open(                          
                        E_AMOPENAT_GPIO_PORT port, 
                        T_AMOPENAT_GPIO_CFG *cfg    
                   )
{
    return OPENAT_config_gpio(port, cfg);
}

/**设置gpio 
*@param		port:		GPIO编号
*@param		value:		0 or 1
*@return	TRUE: 	    成功
*           FALSE:      失败
**/
BOOL iot_gpio_set(                               
                        E_AMOPENAT_GPIO_PORT port,  
                        UINT8 value                
                )
{
    return OPENAT_set_gpio(port, value);
}

/**读取gpio 
*@param		port:		GPIO编号
*@param		value:		0 or 1
*@return	TRUE: 	    成功
*           FALSE:      失败
**/				
BOOL iot_gpio_read(                            
                        E_AMOPENAT_GPIO_PORT port, 
                        UINT8* value                
                  )
{
    return OPENAT_read_gpio(port, value);
}

/**关闭gpio 
*@param		port:		GPIO编号
*@return	TRUE: 	    成功
*           FALSE:      失败
**/	
BOOL iot_gpio_close(                            
                        E_AMOPENAT_GPIO_PORT port
                  )
{
    return OPENAT_close_gpio(port);
}