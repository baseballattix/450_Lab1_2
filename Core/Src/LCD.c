#include "stm32f0xx_hal.h"
#include "main.h"

// simple software delay (about 1 µs * x at 8 MHz)
#define DELAY_US(x)  for (volatile uint32_t _i = 0; _i < ((x) * 8); _i++) { __NOP(); }

/* --- Pin map on GPIOC ---
   PC0..PC3 -> LCD D4..D7
   PC4      -> LCD RS
   PC5      -> LCD E
*/

#define LCD_DMASK     0x0F
#define LCD_RS_BIT    (1U << 4)
#define LCD_E_BIT     (1U << 5)

static inline void set_rs(int rs) {
  if (rs) GPIOC->BSRR = LCD_RS_BIT; else GPIOC->BRR = LCD_RS_BIT;
}
static inline void set_e(int e) {
  if (e) GPIOC->BSRR = LCD_E_BIT; else GPIOC->BRR = LCD_E_BIT;
}

/* Write 4-bit nibble to PC0..PC3 */
static inline void put_nibble(uint8_t nib) {
  uint32_t odr = GPIOC->ODR;
  odr &= ~LCD_DMASK;
  odr |= (nib & 0x0F);
  GPIOC->ODR = odr;
}

/* Generate enable pulse */
static inline void pulse_enable(void) {
  set_e(1);
  DELAY_US(1);
  set_e(0);
  DELAY_US(1);
}

/* Write 8-bit command/data */
static void write8(uint8_t val, int rs) {
  set_rs(rs);
  put_nibble(val >> 4);
  pulse_enable();
  put_nibble(val);
  pulse_enable();

  // inline delays, no macro dependency
  if (val == 0x01 || val == 0x02) {
    for (volatile uint32_t i = 0; i < (2000 * 8); i++) __NOP();  // ~2ms
  } else {
    for (volatile uint32_t i = 0; i < (50 * 8); i++) __NOP();    // ~50us
  }
}

/* Public functions */
void LCD_OutCMD(unsigned char command) {
  write8(command, 0);
}

void LCD_OutChar(unsigned char data) {
  write8(data, 1);
}

void LCD_OutString(char *s) {
  while (*s) LCD_OutChar((unsigned char)*s++);
}

void LCD_Clear(void) {
  LCD_OutCMD(0x01);
}

/* Wakeup sequence to switch to 4-bit mode */
static void lcd_4bit_wakeup(void) {
  set_rs(0);
  put_nibble(0x03); pulse_enable(); HAL_Delay(5);
  put_nibble(0x03); pulse_enable(); HAL_Delay(1);
  put_nibble(0x03); pulse_enable(); DELAY_US(150);
  put_nibble(0x02); pulse_enable(); DELAY_US(150);
}

/* LCD initialization */
void LCD_Init(void) {
  HAL_Init();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIOC->MODER &= ~(0xFFF);
  GPIOC->MODER |=  (0x555);

  put_nibble(0x0);
  set_rs(0);
  set_e(0);
  HAL_Delay(20);

  lcd_4bit_wakeup();

  LCD_OutCMD(0x28); // 4-bit, 2-line, 5x8
  LCD_OutCMD(0x08); // display off
  LCD_Clear();
  LCD_OutCMD(0x06); // entry mode set
  LCD_OutCMD(0x0C); // display on, cursor off, blink off
}
