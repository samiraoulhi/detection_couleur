/*############################## INCLUDES ##########################*/
#include "tcs34725.h"
#include "lcd16x2_i2c.h"
/*############################## CONNECTIONS
################################################################*/
/*
- Sensor of colors -> I2C1:
SCL -> Wire orange from TCS to PB10 I2C1_SCL Serial clock

line

SDA -> Wire yellow from TCS to PB11 I2C1_SDA Serial data

line

LED -> Wire bleu from TCS to PB9 GPIO_Output output level ->

Low

3v3 -> Wire white from TCS to protoboard 3.3V or VIN -> 5v
GND -> Wire gree from TCS to protoboard GND

*/
/*############################## VARIABLES
###################################################################*/
extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim3;
extern uint8_t buttoncalibrate;
unsigned int Color_Sensor_Address = 0x52;
uint8_t rojo=0, verde=0, azul=0, morado=0, total=0;
uint16_t Clear_value, Red_value, Green_value, Blue_value;
uint32_t color = 0;
uint32_t ColorsThreshold[4] = {0, 0, 0, 0};
uint32_t NR,NG,NB,c;
char red_str[10];
/*############################# FUNCTIONS
#######################################################################
*/
void Test_cts34725(void)
{
unsigned char buffer[1] = {0x92};
HAL_I2C_Master_Transmit(&hi2c2, Color_Sensor_Address, buffer, 1,
100);
unsigned char data[1] = {0};
HAL_I2C_Master_Receive(&hi2c2, Color_Sensor_Address, data, 1,
100);

if(data[0] == 68)
{
HAL_GPIO_WritePin(BlueLed_GPIO_Port, BlueLed_Pin,
GPIO_PIN_RESET);
}else
{
HAL_GPIO_WritePin(BlueLed_GPIO_Port, BlueLed_Pin,

GPIO_PIN_SET);
}
}
/*#####################################################################
#####################################*/
/*
B. Initialize the color sensor
*/
void Read_cts34725(void)
{
HAL_GPIO_WritePin(LedSensor_GPIO_Port, LedSensor_Pin,
GPIO_PIN_SET);
//1) Read 2 bytes of CLEAR Data (address code: 148 = 0x94)
unsigned char Clear_register[1] = {0x94};
HAL_I2C_Master_Transmit(&hi2c2, Color_Sensor_Address,
Clear_register, 1, 100);
unsigned char Clear_data[2] = {0,0};
HAL_I2C_Master_Receive(&hi2c2, Color_Sensor_Address, Clear_data,
2, 100);
//16-bits, but we got it into 2 bytes (8-bits)
Clear_value = (((int) Clear_data[1]) << 8) | Clear_data[0];
//2) Read 2 bytes of RED Data (address code: 150 = 0x96)
unsigned char Red_register[1] = {0x96};
HAL_I2C_Master_Transmit(&hi2c2, Color_Sensor_Address,
Red_register, 1, 100);
unsigned char Red_data[2] = {0,0};
HAL_I2C_Master_Receive(&hi2c2, Color_Sensor_Address, Red_data, 2,
100);
//16-bits, but we got it into 2 bytes (8-bits)
Red_value = (((int) Red_data[1]) << 8) | Red_data[0];
//3) Read 2 bytes of GREEN Data (address code: 152 = 0x98)
unsigned char Green_register[1] = {0x98};
HAL_I2C_Master_Transmit(&hi2c2, Color_Sensor_Address,
Green_register, 1, 100);
unsigned char Green_data[2] = {0,0};

HAL_I2C_Master_Receive(&hi2c2, Color_Sensor_Address, Green_data,
2, 100);
//16-bits, but we got it into 2 bytes (8-bits)
Green_value = (((int) Green_data[1]) << 8) | Green_data[0];
//4) Read 2 bytes of BLUE Data (address code: 154 = 0x9A)
unsigned char Blue_register[1] = {0x9A};
HAL_I2C_Master_Transmit(&hi2c2, Color_Sensor_Address,
Blue_register, 1, 100);
unsigned char Blue_data[2] = {0,0};
HAL_I2C_Master_Receive(&hi2c2, Color_Sensor_Address, Blue_data,
2, 100);
//16-bits, but we got it into 2 bytes (8-bits)
Blue_value = (((int) Blue_data[1]) << 8) | Blue_data[0];
//5) Claculate theshold of color in sensor.
color = ((Red_value*65536 + Green_value*256 +
Blue_value)/Clear_value)/10;
}
/*#####################################################################
#####################################*/
void affich(void){
DefineColour(color);
}
void LCD(void){
lcd16x2_i2c_clear();
lcd16x2_i2c_printf("R:%d G:%d B:%d",NR,NG,NB);
}
void DefineColour(uint32_t colour)
{
char uartComAT[100];

if(Red_value>Blue_value && Red_value >Green_value)
{
sprintf(uartComAT, "= Red");
HAL_GPIO_WritePin(Affi_Red_GPIO_Port, Affi_Red_Pin,

GPIO_PIN_SET);

HAL_GPIO_WritePin(Affi_Green_GPIO_Port, Affi_Green_Pin,

GPIO_PIN_RESET);

HAL_GPIO_WritePin(Affi_blue_GPIO_Port, Affi_blue_Pin,

GPIO_PIN_RESET);

HAL_UART_Transmit(&huart2, (uint8_t *)uartComAT,

strlen(uartComAT), 100);
if(c!=1){
NR++;
LCD();
}
c=1;

}
else if(Green_value>Red_value && Green_value >Blue_value)
{
sprintf(uartComAT, "= Green");
HAL_GPIO_WritePin(Affi_Red_GPIO_Port, Affi_Red_Pin,

GPIO_PIN_RESET);

HAL_GPIO_WritePin(Affi_Green_GPIO_Port, Affi_Green_Pin,

GPIO_PIN_SET);

HAL_GPIO_WritePin(Affi_blue_GPIO_Port, Affi_blue_Pin,

GPIO_PIN_RESET);

HAL_UART_Transmit(&huart2, (uint8_t *)uartComAT,

strlen(uartComAT), 100);
if(c!=2){
NG++;
LCD();
}
c=2;
}
else if( Blue_value>Red_value && Blue_value >Green_value)
{
sprintf(uartComAT, "= Blue");
HAL_GPIO_WritePin(Affi_Red_GPIO_Port, Affi_Red_Pin,

GPIO_PIN_RESET);

HAL_GPIO_WritePin(Affi_Green_GPIO_Port, Affi_Green_Pin,

GPIO_PIN_RESET);

HAL_GPIO_WritePin(Affi_blue_GPIO_Port, Affi_blue_Pin,

GPIO_PIN_SET);

HAL_UART_Transmit(&huart2, (uint8_t *)uartComAT,

strlen(uartComAT), 100);
if(c!=3){
NB++;
LCD();
}
c=3;
}
else
{

HAL_GPIO_WritePin(Affi_Red_GPIO_Port, Affi_Red_Pin,

GPIO_PIN_RESET);

HAL_GPIO_WritePin(Affi_Green_GPIO_Port, Affi_Green_Pin,

GPIO_PIN_RESET);

HAL_GPIO_WritePin(Affi_blue_GPIO_Port, Affi_blue_Pin,

GPIO_PIN_RESET);
}
}
/*#####################################################################
#####################################*/
void rest_cal(void){
NR=0;
NG=0;
NB=0;
LCD();
}
/*#####################################################################
#####################################*/
void resetvar(void)
{
rojo=0;
verde=0;
azul=0;
morado=0;
total=0;
NR=0;
NG=0;
NB=0;
c=0;
}
/*#####################################################################
#####################################*/
