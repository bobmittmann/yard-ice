/* l3gd20.h */
/* ST L3GD20 -  motion sensor: three-axis digital output gyroscope */

#ifndef __L3GD20_H__
#define __L3GD20_H__

#define L3GD20_WHO_AM_I         0x0f
#define L3GD20_ID   0xd4

#define L3GD20_CTRL_REG1        0x20

#define CTRL_DR1  (1 << 7)
#define CTRL_DR0  (1 << 6)
#define CTRL_BW1  (1 << 5)
#define CTRL_BW0  (1 << 4)
#define CTRL_PD   (1 << 3)
#define CTRL_ZEN  (1 << 2)
#define CTRL_XEN  (1 << 1)
#define CTRL_YEN  (1 << 0)

#define L3GD20_CTRL_REG2        0x21

#define CTRL_HPM1   (1 << 5)
#define CTRL_HPM0   (1 << 4)
#define CTRL_HPCF3  (1 << 3)
#define CTRL_HPCF2  (1 << 2)
#define CTRL_HPCF1  (1 << 1)
#define CTRL_HPCF0  (1 << 0)

#define L3GD20_CTRL_REG3        0x22

#define CTRL_I2_INT1    (1 << 7)
#define CTRL_I2_BOOT    (1 << 6)
#define CTRL_H_ACTIVE   (1 << 5)
#define CTRL_PP_OD      (1 << 4)
#define CTRL_I2_DRDY    (1 << 3)
#define CTRL_I2_WTM     (1 << 2)
#define CTRL_I2_ORUN    (1 << 1)
#define CTRL_I2_EMPTY   (1 << 0)

#define L3GD20_CTRL_REG4        0x23

#define CTRL_BDU        (1 << 7)
#define CTRL_BLE        (1 << 6)
#define CTRL_FS1        (1 << 5)
#define CTRL_FS0        (1 << 4)
#define CTRL_SIM        (1 << 0)

#define L3GD20_CTRL_REG5        0x24

#define CTRL_BOOT       (1 << 7)
#define CTRL_FIFO_EN    (1 << 6)
#define CTRL_INT_SEL1   (1 << 3)
#define CTRL_INT_SEL0   (1 << 2)
#define CTRL_OUT_SEL1   (1 << 1)
#define CTRL_OUT_SEL0   (1 << 0)

#define L3GD20_REFERENCE        0x25
#define L3GD20_OUT_TEMP         0x26

#define L3GD20_STATUS_REG       0x27

#define STAT_ZYXOR  (1 << 7)
#define STAT_ZOR    (1 << 6)
#define STAT_YOR    (1 << 5)
#define STAT_XOR    (1 << 4)

#define STAT_ZYXDA  (1 << 3)
#define STAT_ZDA    (1 << 2)
#define STAT_YDA    (1 << 1)
#define STAT_XDA    (1 << 0)

#define L3GD20_OUT_X_L          0x28
#define L3GD20_OUT_X_H          0x29
#define L3GD20_OUT_Y_L          0x2a
#define L3GD20_OUT_Y_H          0x2b
#define L3GD20_OUT_Z_L          0x2c
#define L3GD20_OUT_Z_H          0x2d

#define L3GD20_FIFO_CTRL_REG    0x2e

#define FIFO_CTRL_FM2    (1 << 7)
#define FIFO_CTRL_FM1    (1 << 6)
#define FIFO_CTRL_FM0    (1 << 5)
#define FIFO_CTRL_WTM4   (1 << 4)
#define FIFO_CTRL_WTM3   (1 << 3)
#define FIFO_CTRL_WTM2   (1 << 2)
#define FIFO_CTRL_WTM1   (1 << 1)
#define FIFO_CTRL_WTM0   (1 << 0)

#define L3GD20_FIFO_SRC_REG     0x2f

#define FIFO_SRC_WTM    (1 << 7)
#define FIFO_SRC_OVRN   (1 << 6)
#define FIFO_SRC_EMPTY  (1 << 5)
#define FIFO_SRC_FSS4   (1 << 4)
#define FIFO_SRC_FSS3   (1 << 3)
#define FIFO_SRC_FSS2   (1 << 2)
#define FIFO_SRC_FSS1   (1 << 1)
#define FIFO_SRC_FSS0   (1 << 0)

#define L3GD20_INT1_CFG         0x30
#define L3GD20INT1_SRC          0x31

#define L3GD20_INT1_TSH_XL      0x32
#define L3GD20_INT1_TSH_XH      0x33
#define L3GD20_INT1_TSH_YL      0x34
#define L3GD20_INT1_TSH_YH      0x35
#define L3GD20_INT1_TSH_ZL      0x36
#define L3GD20_INT1_TSH_ZH      0x37
#define L3GD20_INT1_DURATION    0x38


#endif // __L3GD20_H__

