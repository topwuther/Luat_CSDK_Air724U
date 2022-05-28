// Platform-specific functions

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "type.h"
#include "elua_int.h"
#include "platform_i2c.h"
#include "platform_rtos.h"

// Error / status codes
enum
{
  PLATFORM_ERR,
  PLATFORM_OK,
  PLATFORM_UNDERFLOW = -1
};

// Platform initialization
int platform_init( void );
void platform_int_init();

// *****************************************************************************
// PIO subsection
// "Virtual ports": 16 ports (PA...PF), 32-bits each for a total of 512 I/O pins.
// They are coded within a single integer, where the high part encodes the port
// number, while the lower part encodes the pin number

typedef u32 pio_type;
typedef u32 pio_code;
#define PLATFORM_IO_PORTS                     16
#define PLATFORM_IO_PORTS_BITS                4
#define PLATFORM_IO_PINS                      32
#define PLATFORM_IO_PINS_BITS                 5
#define PLATFORM_IO_FULL_PORT_BIT             14
#define PLATFORM_IO_FULL_PORT_MASK            ( 1 << PLATFORM_IO_FULL_PORT_BIT )
#define PLATFORM_IO_ENCODE( port, pin, full ) ( ( ( port ) << PLATFORM_IO_PINS_BITS ) | ( pin ) | ( ( full ) ? PLATFORM_IO_FULL_PORT_MASK : 0 ) )
#define PLATFORM_IO_GET_PORT( code )          ( ( ( code ) >> PLATFORM_IO_PINS_BITS ) & ( ( 1 << PLATFORM_IO_PORTS_BITS ) - 1 ) )
#define PLATFORM_IO_GET_PIN( code )           ( ( code ) & ( ( 1 << PLATFORM_IO_PINS_BITS ) - 1 ) )
#define PLATFORM_IO_IS_PORT( code )           ( ( ( code ) & PLATFORM_IO_FULL_PORT_MASK ) != 0 )
#define PLATFORM_IO_ALL_PINS                  0xFFFFFFFFUL
#define PLATFORM_IO_UNKNOWN_PIN               0xFFFFFFFEUL
#define PLATFORM_IO_ENC_PORT                  1
#define PLATFORM_IO_ENC_PIN                   0

#define PLATFORM_IO_READ_IN_MASK              0
#define PLATFORM_IO_READ_OUT_MASK             1

enum
{
  // Pin operations
  PLATFORM_IO_PIN_SET,
  PLATFORM_IO_PIN_CLEAR,
  PLATFORM_IO_PIN_GET,
  PLATFORM_IO_PIN_DIR_INPUT,
  PLATFORM_IO_PIN_DIR_OUTPUT,
  /*+\NEW\zhutianhua\2014.10.22\增加GPIO输出配置1，默认拉高*/
  PLATFORM_IO_PIN_DIR_OUTPUT1,
  /*-\NEW\zhutianhua\2014.10.22\增加GPIO输出配置1，默认拉高*/
  /*+\NEW\liweiqiang\2013.4.5\增加lua gpio 中断配置*/
  PLATFORM_IO_PIN_DIR_INT, 
  /*-\NEW\liweiqiang\2013.4.5\增加lua gpio 中断配置*/
  PLATFORM_IO_PIN_PULLUP,
  PLATFORM_IO_PIN_PULLDOWN,
  PLATFORM_IO_PIN_NOPULL,
/*+\NEW\liweiqiang\2013.4.11\增加pio.pin.close接口*/
  PLATFORM_IO_PIN_CLOSE,
/*-\NEW\liweiqiang\2013.4.11\增加pio.pin.close接口*/
  // Port operations
  PLATFORM_IO_PORT_SET_VALUE,
  PLATFORM_IO_PORT_GET_VALUE,
  PLATFORM_IO_PORT_DIR_INPUT,
  PLATFORM_IO_PORT_DIR_OUTPUT
};




// The platform I/O functions
int platform_pio_has_port( unsigned port );
const char* platform_pio_get_prefix( unsigned port );
int platform_pio_has_pin( unsigned port, unsigned pin );
pio_type platform_pio_op( unsigned port, pio_type pinmask, int op );

// *****************************************************************************
// CAN subsection

// Maximum length for any CAN message
#define PLATFORM_CAN_MAXLEN                   8

// eLua CAN ID types
enum
{
  ELUA_CAN_ID_STD = 0,
  ELUA_CAN_ID_EXT
};

int platform_can_exists( unsigned id );
u32 platform_can_setup( unsigned id, u32 clock );
void platform_can_send( unsigned id, u32 canid, u8 idtype, u8 len, const u8 *data );
int platform_can_recv( unsigned id, u32 *canid, u8 *idtype, u8 *len, u8 *data );

// *****************************************************************************
// SPI subsection

// There are 4 "virtual" SPI ports (SPI0...SPI3).
#define PLATFORM_SPI_TOTAL                    4

// SPI mode
#define PLATFORM_SPI_MASTER                   1
#define PLATFORM_SPI_SLAVE                    0
// SS values
#define PLATFORM_SPI_SELECT_ON                1
#define PLATFORM_SPI_SELECT_OFF               0
// SPI enable/disable
#define PLATFORM_SPI_ENABLE                   1
#define PLATFORM_SPI_DISABLE                  0

// Data types
typedef u32 spi_data_type;

// The platform SPI functions
int platform_spi_exists( unsigned id );
u32 platform_spi_setup( unsigned id, int mode, u32 clock, unsigned cpol, unsigned cpha, unsigned databits );
spi_data_type platform_spi_send_recv( unsigned id, spi_data_type data );
void platform_spi_select( unsigned id, int is_select );

// *****************************************************************************
// UART subsection

// There are 4 "virtual" UART ports (UART0...UART3).
#define PLATFORM_UART_TOTAL                   4

// Parity
enum
{
  PLATFORM_UART_PARITY_EVEN,
  PLATFORM_UART_PARITY_ODD,
  PLATFORM_UART_PARITY_NONE
};

// Stop bits
enum
{
  PLATFORM_UART_STOPBITS_1,
  PLATFORM_UART_STOPBITS_1_5,
  PLATFORM_UART_STOPBITS_2
};

// "Infinite timeout" constant for recv
#define PLATFORM_UART_INFINITE_TIMEOUT        (-1)

// Flow control types (this is a bit mask, one can specify PLATFORM_UART_FLOW_RTS | PLATFORM_UART_FLOW_CTS )
#define PLATFORM_UART_FLOW_NONE               0
#define PLATFORM_UART_FLOW_RTS                1
#define PLATFORM_UART_FLOW_CTS                2

// The platform UART functions
int platform_uart_exists( unsigned id );
/*+\NEW\liweiqiang\2013.4.22\增加uart消息提示或者轮询读取数据选择 */
/*+\NEW\zhuwangbin\2018.8.31\添加参数判断是否上报UART TXDONE*/
u32 platform_uart_setup( unsigned id, u32 baud, int databits, int parity, int stopbits, u32 mode, u32 txDoneReport);
/*-\NEW\zhuwangbin\2018.8.31\添加参数判断是否上报UART TXDONE*/
/*-\NEW\liweiqiang\2013.4.22\增加uart消息提示或者轮询读取数据选择 */
/*+\NEW\zhutianhua\2018.12.27 14:56\新增uart.set_rs485_oe接口，可配置rs485 io使能*/
/*+\bug4024\zhuwangbin\2020.12.25\uart.set_rs485_oe添加可选参数,用来配置485延迟时间*/
u32 platform_uart_setup_rs485_oe(unsigned id, u32 rs485IO, u32 rs485ValidLevel, u32 rs485DelayTime);
/*-\bug4024\zhuwangbin\2020.12.25\uart.set_rs485_oe添加可选参数,用来配置485延迟时间*/
/*-\NEW\zhutianhua\2018.12.27 14:56\新增uart.set_rs485_oe接口，可配置rs485 io使能*/
/*+\NEW\liweiqiang\2013.4.20\增加uart.close接口 */
u32 platform_uart_close( unsigned id );
/*-\NEW\liweiqiang\2013.4.20\增加uart.close接口 */
int platform_uart_set_buffer( unsigned id, unsigned size );
u32 platform_uart_send( unsigned id, u8 data );
u32 platform_s_uart_send( unsigned id, u8 data );
/*+\NEW\liweiqiang\2013.4.7\修改uart数据发送为buffer方式 */
u32 platform_uart_send_buff( unsigned id, const u8 *buff, u16 len );
u32 platform_s_uart_send_buff( unsigned id, const u8 *buff, u16 len );
/*-\NEW\liweiqiang\2013.4.7\修改uart数据发送为buffer方式 */
int platform_uart_recv( unsigned id, unsigned timer_id, s32 timeout );
int platform_s_uart_recv( unsigned id, s32 timeout );
int platform_uart_set_flow_control( unsigned id, int type );
int platform_s_uart_set_flow_control( unsigned id, int type );

// *****************************************************************************
// Timer subsection

// There are 16 "virtual" timers (TMR0...TMR15)
#define PLATFORM_TIMER_TOTAL                  16

// Data types
typedef u32 timer_data_type;

// Interrupt types
#define PLATFORM_TIMER_INT_ONESHOT            1
#define PLATFORM_TIMER_INT_CYCLIC             2

// Match interrupt error codes
#define PLATFORM_TIMER_INT_OK                 0
#define PLATFORM_TIMER_INT_TOO_SHORT          1
#define PLATFORM_TIMER_INT_TOO_LONG           2
#define PLATFORM_TIMER_INT_INVALID_ID         3  

// Timer operations
enum
{
  PLATFORM_TIMER_OP_START,
  PLATFORM_TIMER_OP_READ,
  PLATFORM_TIMER_OP_SET_CLOCK,
  PLATFORM_TIMER_OP_GET_CLOCK,
  PLATFORM_TIMER_OP_GET_MAX_DELAY,
  PLATFORM_TIMER_OP_GET_MIN_DELAY
};

// The platform timer functions
int platform_timer_exists( unsigned id );
void platform_timer_delay( unsigned id, u32 delay_us );
void platform_s_timer_delay( unsigned id, u32 delay_us );
u32 platform_timer_op( unsigned id, int op, u32 data );
u32 platform_s_timer_op( unsigned id, int op, u32 data );
int platform_timer_set_match_int( unsigned id, u32 period_us, int type );
int platform_s_timer_set_match_int( unsigned id, u32 period_us, int type );
u32 platform_timer_get_diff_us( unsigned id, timer_data_type end, timer_data_type start );

// *****************************************************************************
// PWM subsection

// There are 16 "virtual" PWM channels (PWM0...PWM15)
#define PLATFORM_PWM_TOTAL                    16

// PWM operations
enum
{
  PLATFORM_PWM_OP_START,
  PLATFORM_PWM_OP_STOP,
  PLATFORM_PWM_OP_SET_CLOCK,
  PLATFORM_PWM_OP_GET_CLOCK
};

// The platform PWM functions
int platform_pwm_exists( unsigned id );
u32 platform_pwm_setup( unsigned id, u32 frequency, unsigned duty );
u32 platform_pwm_op( unsigned id, int op, u32 data );

// *****************************************************************************
// CPU specific functions

#define PLATFORM_CPU_DISABLE            0
#define PLATFORM_CPU_ENABLE             1

// Interrupt functions return status
#define PLATFORM_INT_OK                 0
#define PLATFORM_INT_GENERIC_ERROR      ( -1 )
#define PLATFORM_INT_INVALID            ( -2 )
#define PLATFORM_INT_NOT_HANDLED        ( -3 )
#define PLATFORM_INT_BAD_RESNUM         ( -4 )

int platform_cpu_set_global_interrupts( int status );
int platform_cpu_get_global_interrupts();
int platform_cpu_set_interrupt( elua_int_id id, elua_int_resnum resnum, int status );
int platform_cpu_get_interrupt( elua_int_id id, elua_int_resnum resnum );
int platform_cpu_get_interrupt_flag( elua_int_id id, elua_int_resnum resnum, int clear );
u32 platform_cpu_get_frequency();

// *****************************************************************************
// The platform ADC functions

enum
{
  PLATFORM_ADC_GET_MAXVAL,
  PLATFORM_ADC_SET_SMOOTHING,
  PLATFORM_ADC_SET_BLOCKING,
  PLATFORM_ADC_SET_FREERUNNING,
  PLATFORM_ADC_IS_DONE,
  PLATFORM_ADC_OP_SET_TIMER,
  PLATFORM_ADC_OP_SET_CLOCK,
};

// Functions requiring platform-specific implementation
int platform_adc_update_sequence();
int platform_adc_start_sequence();
void platform_adc_stop( unsigned id );
u32 platform_adc_setclock( unsigned id, u32 frequency);

// ADC Common Functions
int platform_adc_exists( unsigned id );
int platform_adc_check_timer_id( unsigned id, unsigned timer_id );
u32 platform_adc_op( unsigned id, int op, u32 data );
/*+\NEW\RUFEI\2015.8.27\Add adc fuction*/
int platform_adc_open(unsigned id, unsigned mode);
int platform_adc_read(unsigned id, int *adc, int *volt);
int platform_adc_close(void);
/*-\NEW\RUFEI\2015.8.27\Add adc fuction*/

/*+\NEW\RUFEI\2015.9.8\Add pwm function */
int platform_pwm_open(unsigned id);
int platform_pwm_close(unsigned id);
int platform_pwm_set(unsigned id, int freq, int duty);
/*-\NEW\RUFEI\2015.9.8\Add pwm function */

// *****************************************************************************
// Ethernet specific functions

void platform_eth_send_packet( const void* src, u32 size );
u32 platform_eth_get_packet_nb( void* buf, u32 maxlen );
void platform_eth_force_interrupt();
u32 platform_eth_get_elapsed_time();

// *****************************************************************************
// Allocator support

void* platform_get_first_free_ram( unsigned id );
void* platform_get_last_free_ram( unsigned id );

// *****************************************************************************
// rtos support
void platform_os_sleep(u32 ms);

// *****************************************************************************
// console
void platform_set_console_port( unsigned char id );
unsigned char platform_get_console_port(void);

#endif
