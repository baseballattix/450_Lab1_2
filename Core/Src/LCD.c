#include "LCD.h"
#include "main.h"

unsigned int LCD_RS = 0; 
unsigned int LCD_E = 0;  

void OutPort() {
    GPIOC->ODR = (LCD_RS << 4) | (LCD_E << 5);
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


