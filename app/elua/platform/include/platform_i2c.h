
#ifndef _PLATFORM_I2C_H_
#define _PLATFORM_I2C_H_

// *****************************************************************************
// I2C platform interface

// I2C speed
enum
{
    PLATFORM_I2C_SPEED_SLOW = 100000,
    PLATFORM_I2C_SPEED_FAST = 400000
};

#define I2C_NULL_SLAVE_ADDR         (0xffff)

typedef struct PlatformI2CParamTag
{
    u32         speed;
    u16          slaveAddr;    
}PlatformI2CParam;

/*-\NEW\zhuwangbin\2016.4.6\兼容不同版本的g_sensor, 添加获取设备id和设备地址接口*/
void platform_i2c_gSensorParam_get(u8 id, u8 *slave_addr, u8 *slave_id);
/*-\NEW\zhuwangbin\2016.4.6\兼容不同版本的g_sensor, 添加获取设备id和设备地址接口*/

int platform_i2c_exists( unsigned id );
int platform_i2c_setup( unsigned id, PlatformI2CParam *pParam );
int platform_i2c_send_data( unsigned id, u16 slave_addr, const u8 *pRegAddr, const u8 *buf, u32 len );
int platform_i2c_recv_data( unsigned id, u16 slave_addr, const u8 *pRegAddr, u8 *buf, u32 len  );
/*+\NEW\WANGJIAN\2019.4.10\封装i2c.close接口*/
int platform_i2c_close( unsigned id );
/*-\NEW\WANGJIAN\2019.4.10\封装i2c.close接口*/
#endif //_PLATFORM_I2C_H_
