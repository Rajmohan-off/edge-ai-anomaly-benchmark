/*
 * stm32f407VGTx.h
 *
 *  Created on: 17-Jun-2026
 *      Author: rajmo
 */

#ifndef INC_STM32F407XX_H_
#define INC_STM32F407XX_H_
#include <stdint.h>

#define __vo volatile

/*................................Processor specific header............................*/
 
 ///config the address for nvic set, interrupt register
#define NVIC_ISER0              ((__vo uint32_t*) 0xE000E100)
#define NVIC_ISER1              ((__vo uint32_t*) 0xE000E104)
#define NVIC_ISER2              ((__vo uint32_t*) 0xE000E108)

//config the address for nvic reset, interrupt register
#define NVIC_ICER0              ((__vo uint32_t*) 0XE000E180)
#define NVIC_ICER1              ((__vo uint32_t*) 0xE000E184)
#define NVIC_ICER2              ((__vo uint32_t*) 0xE000E188)

//define the nvic priorty register address
#define NVIC_IPR_BASE_ADDR      ((__vo uint32_t*) 0xE000E400)

#define NO_PR_BITS              4

/*................................ End of Processor specific header......................*/


#define ENABLE             1
#define DISABLE            0
#define SET                ENABLE
#define RESET              DISABLE
#define GPIO_PIN_SET       SET
#define GPIO_PIN_RESET     RESET

//base address of memory
#define FLASH_BASEADDR     (0x08000000U)
#define SRAM_1_BASEADDR    (0x20000000U)
#define SRAM_2_BASEADDR    (0x2001C000U)
#define ROM_BASEADDR       (0x1FFF0000U)

//base address of bus
#define APB1_BASEADDR      (0x40000000U)
#define APB2_BASEADDR      (0x40010000U)
#define AHB1_BASEADDR      (0x40020000U)
#define AHB2_BASEADDR      (0x50000000U)

//base address of GPIO
#define GPIOA_BASEADDR     (AHB1_BASEADDR + 0x0000)
#define GPIOB_BASEADDR     (AHB1_BASEADDR + 0x0400)
#define GPIOC_BASEADDR     (AHB1_BASEADDR + 0x0800)
#define GPIOD_BASEADDR     (AHB1_BASEADDR + 0x0C00)
#define GPIOE_BASEADDR     (AHB1_BASEADDR + 0x1000)
#define GPIOF_BASEADDR     (AHB1_BASEADDR + 0x1400)
#define GPIOG_BASEADDR     (AHB1_BASEADDR + 0x1800)
#define GPIOH_BASEADDR     (AHB1_BASEADDR + 0x1C00)
#define GPIOI_BASEADDR     (AHB1_BASEADDR + 0x2000)
#define RCC_BASEADDR       (AHB1_BASEADDR + 0x3800)

//base address of APB1 peripherals
#define I2C1_BASEADDR      (APB1_BASEADDR + 0x5400)
#define I2C2_BASEADDR      (APB1_BASEADDR + 0x5800)
#define I2C3_BASEADDR      (APB1_BASEADDR + 0x5C00)

#define SPI2_BASEADDR      (APB1_BASEADDR + 0x3800)
#define SPI3_BASEADDR      (APB1_BASEADDR + 0x3C00)

#define USART2_BASEADDR    (APB1_BASEADDR + 0x4400)
#define USART3_BASEADDR    (APB1_BASEADDR + 0x4800)
#define UART4_BASEADDR     (APB1_BASEADDR + 0x4C00)
#define UART5_BASEADDR     (APB1_BASEADDR + 0x5000)

//base address of APB2 peripherals
#define EXTI_BASEADDR      (APB2_BASEADDR + 0x3C00)
#define SPI1_BASEADDR      (APB2_BASEADDR + 0x3000)
#define SYSCFG_BASEADDR    (APB2_BASEADDR + 0x3800)
#define USART1_BASEADDR    (APB2_BASEADDR + 0x1000)
#define USART6_BASEADDR    (APB2_BASEADDR + 0x1400)

//SPI1 peripheral addresses
#define SPI1_CR1           (SPI_BASEADDR + 0x0000)
#define SPI1_CR2           (SPI_BASEADDR + 0x0004)
#define SPI1_SR            (SPI_BASEADDR + 0x0008)
#define SPI1_DR            (SPI_BASEADDR + 0x000C)
#define SPI1_CRCPR         (SPI_BASEADDR + 0x0010)
#define SPI1_RXCRCR        (SPI_BASEADDR + 0x0014)
#define SPI1_TXCRCR        (SPI_BASEADDR + 0x0018)
#define SPI1_I2SCFGR       (SPI_BASEADDR + 0x001C)
#define SPI1_I2SPR         (SPI_BASEADDR + 0x0020)

//GPIO Peripherals
typedef struct
{
	__vo uint32_t MODER;
	__vo uint32_t OTYPER;
	__vo uint32_t OSPEEDR;
	__vo uint32_t PUPDR;
	__vo uint32_t IDR;
	__vo uint32_t ODR;
	__vo uint32_t BSRR;
	__vo uint32_t LCKR;
	__vo uint32_t AFR[2];
} GPIO_RegDef_t;

//GPIO Typecast with the typedef struct, so the peripherals can be easily accessible
#define GPIOA              ((GPIO_RegDef_t*)GPIOA_BASEADDR)
#define GPIOB              ((GPIO_RegDef_t*)GPIOB_BASEADDR)
#define GPIOC              ((GPIO_RegDef_t*)GPIOC_BASEADDR)
#define GPIOD              ((GPIO_RegDef_t*)GPIOD_BASEADDR)
#define GPIOE              ((GPIO_RegDef_t*)GPIOE_BASEADDR)
#define GPIOF              ((GPIO_RegDef_t*)GPIOF_BASEADDR)
#define GPIOG              ((GPIO_RegDef_t*)GPIOG_BASEADDR)
#define GPIOH              ((GPIO_RegDef_t*)GPIOH_BASEADDR)
#define GPIOI              ((GPIO_RegDef_t*)GPIOI_BASEADDR)

typedef struct{
	__vo uint32_t CR;
	__vo uint32_t PLLCFGR;
	__vo uint32_t CFGR;
	__vo uint32_t CIR;
	__vo uint32_t AHB1RSTR;
	__vo uint32_t AHB2RSTR;
	__vo uint32_t AHB3RSTR;
	uint32_t RESERVED1;
	__vo uint32_t APB1RSTR;
	__vo uint32_t APB2RSTR;
	uint32_t RESERVED2;
	uint32_t RESERVED3;
	__vo uint32_t AHB1ENR;
	__vo uint32_t AHB2ENR;
	__vo uint32_t AHB3ENR;
	uint32_t RESERVED4;
	__vo uint32_t APB1ENR;
	__vo uint32_t APB2ENR;
	uint32_t RESERVED5;
	uint32_t RESERVED6;
	__vo uint32_t AHB1LPENR;
	__vo uint32_t AHB2LPENR;
	__vo uint32_t AHB3LPENR;
	uint32_t RESERVED7;
	__vo uint32_t APB1LPENR;
	__vo uint32_t APB2LPENR;
	uint32_t RESERVED8;
	uint32_t RESERVED9;
	__vo uint32_t BDCR;
	__vo uint32_t CSR;
	uint32_t RESERVED10;
	uint32_t RESERVED11;
	__vo uint32_t SSCGR;
	__vo uint32_t PLLI2SCFGR;

}RCC_RegDef_t;

#define RCC           ((RCC_RegDef_t*)RCC_BASEADDR)



//Peripheral struct for EXTI
typedef struct {
	__vo uint32_t IMR;
	__vo uint32_t EMR;
	__vo uint32_t RTSR;
	__vo uint32_t FTSR;
	__vo uint32_t SWIER;
	__vo uint32_t PR;

}EXTI_RegDef_t;

//EXTI Baseaddr config
#define EXTI         ((EXTI_RegDef_t*)EXTI_BASEADDR)


//config IRQ Number with corresponding EXTI
#define IRQ_NO_EXTI0      6
#define IRQ_NO_EXTI1      7
#define IRQ_NO_EXTI2      8
#define IRQ_NO_EXTI3      9
#define IRQ_NO_EXTI4      10
#define IRQ_NO_EXTI9_5   23
#define IRQ_NO_EXTI15_10  40


//config IRQ Priority level
#define NVIC_IRQ_PRI0    0
#define NVIC_IRQ_PRI1    1
#define NVIC_IRQ_PRI2    2
#define NVIC_IRQ_PRI3    3
#define NVIC_IRQ_PRI4    4
#define NVIC_IRQ_PRI5    5
#define NVIC_IRQ_PRI6    6
#define NVIC_IRQ_PRI7    7
#define NVIC_IRQ_PRI8    8
#define NVIC_IRQ_PRI9    9
#define NVIC_IRQ_PRI10   10
#define NVIC_IRQ_PRI11   11
#define NVIC_IRQ_PRI12   12
#define NVIC_IRQ_PRI13   13
#define NVIC_IRQ_PRI14   14
#define NVIC_IRQ_PRI15   15


//peripheral struct for SYSCFG
typedef struct {
	__vo uint32_t MEMRMP;
	__vo uint32_t PMC;
	__vo uint32_t EXTICR[4];
	uint32_t RESERVED_1[2];
	__vo uint32_t CMPCR;

}SYSCFG_RegDef_t;

#define SYSCFG      ((SYSCFG_RegDef_t*)SYSCFG_BASEADDR)


// SYSCFG Port declaration macros
#define GPIO_To_SYSCFG(x)        ((x == GPIOA) ? 0:\
		                              (x == GPIOB) ? 1:\
				                          (x == GPIOC) ? 2:\
				                          (x == GPIOD) ? 3:\
				                          (x == GPIOE) ? 4:\
  				                        (x == GPIOF) ? 5:\
  				                        (x == GPIOG) ? 6:\
  				                        (x == GPIOH) ? 7:\
  				                        (x == GPIOI) ? 8:0)


//enable clock for GPIO

#define GPIOA_CLK_EN()         (RCC->AHB1ENR |= ( 1 << 0 ))
#define GPIOB_CLK_EN()         (RCC->AHB1ENR |= ( 1 << 1 ))
#define GPIOC_CLK_EN()         (RCC->AHB1ENR |= ( 1 << 2 ))
#define GPIOD_CLK_EN()         (RCC->AHB1ENR |= ( 1 << 3 ))
#define GPIOE_CLK_EN()         (RCC->AHB1ENR |= ( 1 << 4 ))
#define GPIOF_CLK_EN()         (RCC->AHB1ENR |= ( 1 << 5 ))
#define GPIOG_CLK_EN()         (RCC->AHB1ENR |= ( 1 << 6 ))
#define GPIOH_CLK_EN()         (RCC->AHB1ENR |= ( 1 << 7 ))
#define GPIOI_CLK_EN()         (RCC->AHB1ENR |= ( 1 << 8 ))


//enable clcock for I2C

#define I2C1_CLK_EN()          (RCC->APB1ENR |= ( 1 << 21))
#define I2C2_CLK_EN()          (RCC->APB1ENR |= ( 1 << 22))
#define I2C3_CLK_EN()          (RCC->APB1ENR |= ( 1 << 23))

//enable clock for SPI

#define SPI1_CLK_EN()          (RCC->APB2ENR |= ( 1 << 12))
#define SPI2_CLK_EN()          (RCC->APB1ENR |= ( 1 << 14))
#define SPI3_CLK_EN()          (RCC->APB1ENR |= ( 1 << 15))

//enable clock for USART

#define USART1_CLK_EN()        (RCC->APB2ENR |= ( 1 << 4 ))
#define USART2_CLK_EN()        (RCC->APB1ENR |= ( 1 << 17))
#define USART3_CLK_EN()        (RCC->APB1ENR |= ( 1 << 18))
#define USART4_CLK_EN()        (RCC->APB1ENR |= ( 1 << 19))
#define UART5_CLK_EN()         (RCC->APB1ENR |= ( 1 << 20))
#define USART6_CLK_EN()        (RCC->APB2ENR |= ( 1 << 5 ))

//enable clock for SYSCFG

#define SYSCFG_CLK_EN()        (RCC->APB2ENR |= (1 << 14))

//disable clock for GPIO

#define GPIOA_CLK_DI()         (RCC->AHB1ENR &= ~( 1 << 0 ))
#define GPIOB_CLK_DI()         (RCC->AHB1ENR &= ~( 1 << 1 ))
#define GPIOC_CLK_DI()         (RCC->AHB1ENR &= ~( 1 << 2 ))
#define GPIOD_CLK_DI()         (RCC->AHB1ENR &= ~( 1 << 3 ))
#define GPIOE_CLK_DI()         (RCC->AHB1ENR &= ~( 1 << 4 ))
#define GPIOF_CLK_DI()         (RCC->AHB1ENR &= ~( 1 << 5 ))
#define GPIOG_CLK_DI()         (RCC->AHB1ENR &= ~( 1 << 6 ))
#define GPIOH_CLK_DI()         (RCC->AHB1ENR &= ~( 1 << 7 ))
#define GPIOI_CLK_DI()         (RCC->AHB1ENR &= ~( 1 << 8 ))

//Disable clock for I2C

#define I2C1_CLK_DI()          (RCC->APB1ENR &= ~( 1 << 21))
#define I2C2_CLK_DI()          (RCC->APB1ENR &= ~( 1 << 22))
#define I2C3_CLK_DI()          (RCC->APB1ENR &= ~( 1 << 23))

//disable clock for SPI

#define SPI1_CLK_DI()          (RCC->APB2ENR &= ~( 1 << 12))
#define SPI2_CLK_DI()          (RCC->APB1ENR &= ~( 1 << 14))
#define SPI3_CLK_DI()          (RCC->APB1ENR &= ~( 1 << 15))

//disable clock for USART

#define USART1_CLK_DI()        (RCC->APB2ENR &= ~( 1 << 4 ))
#define USART2_CLK_DI()        (RCC->APB1ENR &= ~( 1 << 17))
#define USART3_CLK_DI()        (RCC->APB1ENR &= ~( 1 << 18))
#define USART4_CLK_DI()        (RCC->APB1ENR &= ~( 1 << 19))
#define UART5_CLK_DI()         (RCC->APB1ENR &= ~( 1 << 20))
#define USART6_CLK_DI()        (RCC->APB2ENR &= ~( 1 << 5 ))

//disable clock for SYSCFG

#define SYSCFG_CLK_DI()        (RCC->APB2ENR &= ~(1 << 14))

//Reset GPIO
#define GPIOA_REG_RESET()      do{ (RCC->AHB1RSTR |= (1 << 0)); (RCC->AHB1RSTR &= ~(1 << 0)); }while(0)
#define GPIOB_REG_RESET()      do{ (RCC->AHB1RSTR |= (1 << 1)); (RCC->AHB1RSTR &= ~(1 << 1)); }while(0)
#define GPIOC_REG_RESET()      do{ (RCC->AHB1RSTR |= (1 << 2)); (RCC->AHB1RSTR &= ~(1 << 2)); }while(0)
#define GPIOD_REG_RESET()      do{ (RCC->AHB1RSTR |= (1 << 3)); (RCC->AHB1RSTR &= ~(1 << 3)); }while(0)
#define GPIOE_REG_RESET()      do{ (RCC->AHB1RSTR |= (1 << 4)); (RCC->AHB1RSTR &= ~(1 << 4)); }while(0)
#define GPIOF_REG_RESET()      do{ (RCC->AHB1RSTR |= (1 << 5)); (RCC->AHB1RSTR &= ~(1 << 5)); }while(0)
#define GPIOG_REG_RESET()      do{ (RCC->AHB1RSTR |= (1 << 6)); (RCC->AHB1RSTR &= ~(1 << 6)); }while(0)
#define GPIOH_REG_RESET()      do{ (RCC->AHB1RSTR |= (1 << 7)); (RCC->AHB1RSTR &= ~(1 << 7)); }while(0)
#define GPIOI_REG_RESET()      do{ (RCC->AHB1RSTR |= (1 << 8)); (RCC->AHB1RSTR &= ~(1 << 8)); }while(0)


//Reset I2C
#define I2C1_RESET()           do{ (RCC->APB1RSTR |= ( 1 << 21)); (RCC->APB1RSTR &= ~( 1 << 21)); }while(0)
#define I2C2_RESET()           do{ (RCC->APB1RSTR |= ( 1 << 22)); (RCC->APB1RSTR &= ~( 1 << 22)); }while(0)
#define I2C3_RESET()           do{ (RCC->APB1RSTR |= ( 1 << 23)); (RCC->APB1RSTR &= ~( 1 << 23)); }while(0)


//Config the registers of I2C
typedef struct {
	__vo uint32_t   I2C_CR1;
	__vo uint32_t   I2C_CR2;
	__vo uint32_t   I2C_OAR1;
	__vo uint32_t   I2C_OAR2;
	__vo uint32_t   I2C_DR;
	__vo uint32_t   I2C_SR1;
	__vo uint32_t   I2C_SR2;
	__vo uint32_t   I2C_CCR;
	__vo uint32_t   I2C_TRISE;
	__vo uint32_t   I2C_FLTR;

}I2C_RegDef_t;


//map the I2C registers to the peripherals
#define I2C1              ((I2C_RegDef_t*)I2C1_BASEADDR)
#define I2C2              ((I2C_RegDef_t*)I2C2_BASEADDR)
#define I2C3              ((I2C_RegDef_t*)I2C3_BASEADDR)


//Bit position definition of I2C_CR1
#define I2C_CR1_PE        0
#define I2C_CR1_SMBUS     1
#define I2C_CR1_SMBTYPE   3
#define I2C_CR1_ENARP     4
#define I2C_CR1_ENPEC     5
#define I2C_CR1_ENGC      6
#define I2C_CR1_NOSTRECH  7
#define I2C_CR1_START     8
#define I2C_CR1_STOP      9
#define I2C_CR1_ACK       10
#define I2C_CR1_POS       11
#define I2C_CR1_PEC       12
#define I2C_CR1_ALERT     13
#define I2C_CR1_SWRST     15


//Bit position definition of I2C_CR2
#define I2C_CR1_FREQ      0
#define I2C_CR1_ITERREN   8
#define I2C_CR1_ITEVTEN   9
#define I2C_CR1_ITBUFEN   10
#define I2C_CR1_DMAEN     11
#define I2C_CR1_LAST      12


//Bit position definition of I2C_SR1
#define I2C_SR1_SB        0
#define I2C_SR1_ADDR      1
#define I2C_SR1_BTF       2
#define I2C_SR1_ADD10     3
#define I2C_SR1_STOPF     4
#define I2C_SR1_RXNE      6
#define I2C_SR1_TXE       7
#define I2C_SR1_BERR      8
#define I2C_SR1_ARLO      9
#define I2C_SR1_AF        10
#define I2C_SR1_OVR       11
#define I2C_SR1_PECERR    12
#define I2C_SR1_TIMEOUT   14
#define I2C_SR1_SMBALERT  15


//Bit position definition of I2C_SR2
#define I2C_SR2_MSL           0
#define I2C_SR2_BUSY          1
#define I2C_SR2_TRA           2
#define I2C_SR2_GENCALL       4
#define I2C_SR2_SMBDEFAULT    5
#define I2C_SR2_SMBHOST       6
#define I2C_SR2_DUALF         7
#define I2C_SR2_PEC           8


//Bit position definition of I2C_CCR
#define I2C_CCR_CCR           0
#define I2C_CCR_DUTY          1
#define I2C_CCR_FS            2




#endif /* INC_STM32F407XX_H_ */
