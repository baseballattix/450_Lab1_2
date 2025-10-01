#include "LCD.h"
#include "main.h"

unsigned int LCD_RS = 0; 
unsigned int LCD_E = 0;  
#define DELAY_US(x)  for (volatile uint32_t _i = 0; _i < (x * 8); _i++) { __NOP(); }

// or the precise SysTick version if you want


void OutPort() {
    GPIOC->ODR = (LCD_RS << 4) | (LCD_E << 5);
}

void SendPulse() {
    OutPort();
    DELAY_US(6);
    LCD_E = 1;
    OutPort();
    DELAY_US(6);
    LCD_E = 0;
    OutPort();
}

void SendChar() {
    LCD_E = 0;
    LCD_RS = 1;
    SendPulse();
    DELAY_US(1600);
}

void SendCmd() {
    LCD_E = 0;
    LCD_RS = 0;
    SendPulse();
    DELAY_US(40);
}

void LCD_Init(void) {  
    HAL_Init();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIOC->MODER &= ~(0xFFF);
    GPIOC->MODER |= (0x555); 
    LCD_E = 0;
    LCD_RS = 0;
    OutPort();
    LCD_OutCMD(0x30);
    HAL_Delay(5);
    LCD_OutCMD(0x30);
    HAL_Delay(1);
    LCD_OutCMD(0x30);
    HAL_Delay(1);

    LCD_OutCMD(0x28);
    LCD_Clear();
    LCD_OutCMD(0x10);
    LCD_OutCMD(0x06);
}

void LCD_OutCMD(unsigned char command) {
    unsigned char com_high = (command >> 4) & 0x0F;
    unsigned char com_low = 0x0F&command;
    GPIOC->ODR = (GPIOC->ODR & ~0x0F) | com_high;
    SendCmd();
    GPIOC->ODR = (GPIOC->ODR & ~0x0F) | com_low;
    SendCmd();
    DELAY_US(1000);
}

void LCD_OutChar(unsigned char data) {
    unsigned char let_high = (data >> 4) & 0x0F;
    unsigned char let_low = (0x0F&data);
    GPIOC->ODR = (GPIOC->ODR & ~0x0F) | let_high;
    SendChar();
    GPIOC->ODR = (GPIOC->ODR & ~0x0F) | let_low;
    SendChar(); 
}

void LCD_OutString(char *string) {
    while(*string) {
        LCD_OutChar((unsigned char)(*string++)); 
    }
 }

 void LCD_Clear() {
    LCD_OutCMD(0x01);
    DELAY_US(100);
 }
