#ifndef __MACH_GPIO_H__
#define __MACH_GPIO_H__

#define GPIO_PIN_MASK(gpio)	(gpio & 0x0F)
#define GPIO_PORT_MASK(gpio)	((gpio & 0xF0)
#define GPIO_ALTCFG_MASK(gpio)	((gpio & 0xF00) >> 8)

#define GPIO_UNDEF	0x00
#define GPIO_PA0	0x10
#define GPIO_PA1	0x11
#define GPIO_PA2	0x12
#define GPIO_PA3	0x13
#define GPIO_PA4	0x14
#define GPIO_PA5	0x15
#define GPIO_PA6	0x16
#define GPIO_PA7	0x17

#define GPIO_PB0	0x20
#define GPIO_PB1	0x21
#define GPIO_PB2	0x22
#define GPIO_PB3	0x23
#define GPIO_PB4	0x24
#define GPIO_PB5	0x25

#define GPIO_PC0	0x30
#define GPIO_PC1	0x31
#define GPIO_PC2	0x32
#define GPIO_PC3	0x33
#define GPIO_PC4	0x34
#define GPIO_PC5	0x35
#define GPIO_PC6	0x36
#define GPIO_PC7	0x37

#define GPIO_PD0	0x40
#define GPIO_PD1	0x41
#define GPIO_PD2	0x42
#define GPIO_PD3	0x43
#define GPIO_PD4	0x44
#define GPIO_PD5	0x45
#define GPIO_PD6	0x46
#define GPIO_PD7	0x47

#define GPIO_PE0	0x50
#define GPIO_PE1	0x51
#define GPIO_PE2	0x52
#define GPIO_PE3	0x53
#define GPIO_PE4	0x54
#define GPIO_PE5	0x55

#define GPIO_PF0	0x60
#define GPIO_PF1	0x61
#define GPIO_PF2	0x62
#define GPIO_PF3	0x63
#define GPIO_PF4	0x64

#define GPIO_PG0	0x70
#define GPIO_PG1	0x71

#define GPIO_PH0	0x80
#define GPIO_PH1	0x81
#define GPIO_PH2	0x82
#define GPIO_PH3	0x83

#define GPIO_PJ0	0x90
#define GPIO_PJ1	0x91

#define GPIO_PK0	0xa0
#define GPIO_PK1	0xa1
#define GPIO_PK2	0xa2
#define GPIO_PK3	0xa3
#define GPIO_PK4	0xa4
#define GPIO_PK5	0xa5
#define GPIO_PK6	0xa6
#define GPIO_PK7	0xa7

#define GPIO_PL0	0xB0
#define GPIO_PL1	0xB1
#define GPIO_PL2	0xB2
#define GPIO_PL3	0xB3
#define GPIO_PL4	0xB4
#define GPIO_PL5	0xB5
#define GPIO_PL6	0xB6
#define GPIO_PL7	0xB7

#define GPIO_PM0	0xC0
#define GPIO_PM1	0xC1
#define GPIO_PM2	0xC2
#define GPIO_PM3	0xC3
#define GPIO_PM4	0xC4
#define GPIO_PM5	0xC5
#define GPIO_PM6	0xC6
#define GPIO_PM7	0xC7

#define GPIO_PN0	0xD0
#define GPIO_PN1	0xD1
#define GPIO_PN2	0xD2
#define GPIO_PN3	0xD3
#define GPIO_PN4	0xD4
#define GPIO_PN5	0xD5

#define GPIO_PP0	0xE0
#define GPIO_PP1	0xE1
#define GPIO_PP2	0xE2
#define GPIO_PP3	0xE3
#define GPIO_PP4	0xE4
#define GPIO_PP5	0xE5

#define GPIO_PQ0	0xF0
#define GPIO_PQ1	0xF1
#define GPIO_PQ2	0xF2
#define GPIO_PQ3	0xF3
#define GPIO_PQ4	0xF4

#endif
