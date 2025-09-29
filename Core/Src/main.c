#include <main.h>

const struct State {
uint32_t Out;
uint32_t Time; // 10 ms units
uint32_t Next[8]; // list of next states
};
typedef const struct State STyp;
#define goN 0
#define waitN 1
#define goE 2
#define waitE 3
#define checkN 4
#define goNtoWalk 5
#define checkE 6
#define goEtoWalk 7
#define preWalk 8
#define goWalk 9
#define hurryWalk1 10
#define hurryWalk2 11
#define hurryWalk3 12
#define hurryWalk4 13
#define stopWalk 14
STyp FSM[15] = {
{0x85,1000,{goN,checkN,waitN,checkN,goN,checkN,waitN,checkN}},
{0x89, 250,{goE,goE,goE,goE,goE,goE,goE,goE}},
{0x31,1000,{goE,checkE,goE,checkE,waitE,checkE,waitE,checkE}},
{0x51, 250,{goN,goN,goN,goN,goN,goN,goN,goN}},
{0x85, 2000, {goN,goNtoWalk,goN,goNtoWalk,goN,goNtoWalk,goN,goNtoWalk}}, 
{0x89, 250, {preWalk,preWalk,preWalk,preWalk,preWalk,preWalk,preWalk,preWalk}},
{0x31, 2000, {goE,goEtoWalk,goE,goEtoWalk,goE,goEtoWalk,goE,goEtoWalk}}, 
{0x51, 250, {preWalk,preWalk,preWalk,preWalk,preWalk,preWalk,preWalk,preWalk}},
{0x91, 500, {goWalk,goWalk,goWalk,goWalk,goWalk,goWalk,goWalk,goWalk}},
{0x92, 1000, {hurryWalk1,hurryWalk1,hurryWalk1,hurryWalk1,hurryWalk1,hurryWalk1,hurryWalk1,hurryWalk1}},
{0x91, 250, {hurryWalk2,hurryWalk2,hurryWalk2,hurryWalk2,hurryWalk2,hurryWalk2,hurryWalk2,hurryWalk2}},
{0x90, 250, {hurryWalk3,hurryWalk3,hurryWalk3,hurryWalk3,hurryWalk3,hurryWalk3,hurryWalk3,hurryWalk3}},
{0x91,250, {hurryWalk4,hurryWalk4,hurryWalk4,hurryWalk4,hurryWalk4,hurryWalk4,hurryWalk4,hurryWalk4}},
{0x90,250, {stopWalk,stopWalk,stopWalk,stopWalk,stopWalk,stopWalk,stopWalk,stopWalk}},
{0x91,1000, {goN,goWalk,goE,goWalk,goN,goWalk,goN,goWalk}},
};


int main(void){
    HAL_Init();

    
    GPIO_InitTypeDef GPIOA_InitStructure = {};
    __HAL_RCC_GPIOB_CLK_ENABLE();   
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIOB->MODER &= (~(0xFFFF << 2));
    GPIOB->MODER |= (0x5555 << 2);
    
    GPIOA_InitStructure.Pin = GPIO_PIN_15;
    GPIOA_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIOA_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIOA_InitStructure);
    
    GPIOA_InitStructure.Pin = GPIO_PIN_14;
    HAL_GPIO_Init(GPIOA, &GPIOA_InitStructure);

    GPIOA_InitStructure.Pin = GPIO_PIN_5;
    HAL_GPIO_Init(GPIOA, &GPIOA_InitStructure);
    
    uint32_t Pt; 
    uint32_t Input = 0;
    
    Pt = goN;
    
while(1) {

uint8_t north = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15);
uint8_t east  = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_14);
uint8_t ped   = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);

if(!north && !east && !ped)          Input = 0; 
else if(!north && !east && ped)      Input = 1; 
else if(!north && east && !ped)      Input = 2; 
else if(!north && east && ped)       Input = 3; 
else if(north && !east && !ped)      Input = 4; 
else if(north && !east && ped)       Input = 5; 
else if(north && east && !ped)       Input = 6; 
else if(north && east && ped)        Input = 7; 


   
    Pt = FSM[Pt].Next[Input];

    
    int32_t bMask = 0x1FE;  // PB1-PB8
    uint32_t oValue = (FSM[Pt].Out << 1) & bMask;
    GPIOB->ODR = (GPIOB->ODR & ~bMask) | oValue;

    // Wait for the specified state time
    HAL_Delay(FSM[Pt].Time);
}
}