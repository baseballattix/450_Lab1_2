#ifndef LCD_H
#define LCD_H

void LCD_Init(void);
void LCD_Clear(void);
void LCD_OutCMD(unsigned char command);
void LCD_OutChar(unsigned char data);
void LCD_OutString(char *string);

#endif



