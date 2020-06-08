#include "iot_i2c.h"

/**����i2c
*@param		port:		I2C ���
*@param		param:		��ʼ������
*@return	TRUE:       �ɹ�
*	        FALSE:      ʧ��
**/
BOOL iot_i2c_open(
                        E_AMOPENAT_I2C_PORT  port,         
                        T_AMOPENAT_I2C_PARAM *param         
                  )
{
    return OPENAT_open_i2c(port, param);
}

/**�ر�i2c
*@param		port:		I2C ���
*@return	TRUE:       �ɹ�
*	        FALSE:      ʧ��
**/
BOOL iot_i2c_close(
                        E_AMOPENAT_I2C_PORT  port          
                  )
{
    return OPENAT_close_i2c(port);
}

/**д��i2c����
*@param		port:		    I2C ���
*@param		salveAddr:		���豸��ַ
*@param		pRegAddr:		�Ĵ�����ַ
*@param		buf:		    д�����ݵ�ַ
*@param		bufLen:		    д�����ݳ���
*@return	UINT32:         ʵ��д�볤��
**/
UINT32 iot_i2c_write(                                 
                        E_AMOPENAT_I2C_PORT port,       
                        UINT8 salveAddr,
                        CONST UINT8 *pRegAddr,             
                        CONST UINT8* buf,                  
                        UINT32 bufLen                   
                   )
{
	UINT8 psalveAddr = salveAddr << 1;
    return OPENAT_write_i2c(port, psalveAddr, pRegAddr, buf, bufLen);
}

/**��ȡi2c����
*@param		port:		    I2C ���
*@param		slaveAddr:		���豸��ַ
*@param		pRegAddr:		�Ĵ�����ַ
*@param		buf:		    �洢���ݵ�ַ
*@param		bufLen:		    �洢�ռ䳤��
*@return	UINT32:         ʵ�ʶ�ȡ����
**/
UINT32 iot_i2c_read(                                        
                        E_AMOPENAT_I2C_PORT port,        
                        UINT8 slaveAddr, 
                        CONST UINT8 *pRegAddr,             
                        UINT8* buf,                      
                        UINT32 bufLen                      
                  )
{
	UINT8 psalveAddr = slaveAddr << 1;
	return OPENAT_read_i2c(port, psalveAddr, pRegAddr, buf, bufLen);
}