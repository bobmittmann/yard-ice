/* 
   lsm320dlhc.h - ST LSM303DLHC: 3D accelerometer and 3D magnetometer 
 */

#ifndef __LSM303DL_H__
#define __LSM303DL_H__

/* -------------------------------------------------------------------------
   3D accelerometer and 3D magnetometer 
   ------------------------------------------------------------------------- */

#define LSM303_CTRL_REG1_A      0x20

#define CTRL_ODR3       (1 << 7)
#define CTRL_ODR2       (1 << 6)
#define CTRL_ODR1       (1 << 5)
#define CTRL_ODR0       (1 << 4)
#define CTRL_LPEN       (1 << 3)
#define CTRL_ZEN        (1 << 2)
#define CTRL_YEN        (1 << 1)
#define CTRL_XEN        (1 << 0)

#define ODR_LOW_POWER   0x0
#define ODR_1HZ         0x1
#define ODR_10HZ        0x2
#define ODR_25HZ        0x3
#define ODR_50HZ        0x4
#define ODR_100HZ       0x5
#define ODR_200HZ       0x6
#define ODR_400HZ       0x7
#define ODR_1620HZ      0x8
#define ODR_5376HZ      0x9

#define LSM303_CTRL_REG2_A      0x21

#define CTRL_HPM1       (1 << 7)
#define CTRL_HPM0       (1 << 6)
#define CTRL_HPFC2      (1 << 5)
#define CTRL_HPFC1      (1 << 4)
#define CTRL_FDS        (1 << 3)
#define CTRL_HPCLICK    (1 << 2)
#define CTRL_HPIS2      (1 << 1)
#define CTRL_HPIS1      (1 << 0)

#define LSM303_CTRL_REG3_A      0x22

#define CTRL_I1_CLICK   (1 << 7)
#define CTRL_I1_AOI1    (1 << 6)
#define CTRL_I1_AOI2    (1 << 5)
#define CTRL_I1_DRDY1   (1 << 4)
#define CTRL_I1_DRDY2   (1 << 3)
#define CTRL_I1_WTM     (1 << 2)
#define CTRL_I1_OVERRUN (1 << 1)

#define LSM303_CTRL_REG4_A      0x23

#define CTRL_BDU        (1 << 7)
#define CTRL_BLE        (1 << 6)
#define CTRL_FS1        (1 << 5)
#define CTRL_FS0        (1 << 4)
#define CTRL_HR         (1 << 3)
#define CTRL_SIM        (1 << 0)

#define LSM303_CTRL_REG5_A      0x24

#define CTRL_BOOT       (1 << 7)
#define CTRL_FIFO_EN    (1 << 6)
#define CTRL_LIR_INT1   (1 << 3)
#define CTRL_D4D_INT1   (1 << 2)
#define CTRL_LIR_INT2   (1 << 1)
#define CTRL_D4DINT2    (1 << 0)

#define LSM303_CTRL_REG6_A      0x25

#define CTRL_I2_CKICK   (1 << 7)
#define CTRL_I2_INT1    (1 << 6)
#define CTRL_I2_INT2    (1 << 5)
#define CTRL_BOOT_I1    (1 << 4)
#define CTRL_P2_ACT     (1 << 3)
#define CTRL_H_ACTIVE   (1 << 1)

#define LSM303_REFERENCE_A      0x26

#define LSM303_STATUS_REG_A     0x27

#define STAT_ZYXOR      (1 << 7)
#define STAT_ZOR        (1 << 6)
#define STAT_YOR        (1 << 5)
#define STAT_XOR        (1 << 4)

#define STAT_ZYXDA      (1 << 3)
#define STAT_ZDA        (1 << 2)
#define STAT_YDA        (1 << 1)
#define STAT_XDA        (1 << 0)

#define LSM303_OUT_X_L_A        0x28
#define LSM303_OUT_X_H_A        0x29
#define LSM303_OUT_Y_L_A        0x2a
#define LSM303_OUT_Y_H_A        0x2b
#define LSM303_OUT_Z_L_A        0x2c
#define LSM303_OUT_Z_H_A        0x2d

#define LSM303_FIFO_CTRL_REG_A  0x2e

#define FIFO_FM1        (1 << 7)
#define FIFO_FM0        (1 << 6)

#define FIFO_FM_BYPASS  (0 << 6)
#define FIFO_FM_FIFO    (1 << 6)
#define FIFO_FM_STREAM  (2 << 6)
#define FIFO_FM_TRIGGER (3 << 6)

#define FIFO_TR         (1 << 5)
#define FIFO_FTH4       (1 << 4)
#define FIFO_FTH3       (1 << 3)
#define FIFO_FTH2       (1 << 2)
#define FIFO_FTH1       (1 << 1)
#define FIFO_FTH0       (1 << 0)

#define LSM303_FIFO_SRC_REG_A   0x2f

#define FIFO_WTM        (1 << 7)
#define FIFO_OVRN       (1 << 6)
#define FIFO_EMPTY      (1 << 5)
#define FIFO_FSS4       (1 << 4)
#define FIFO_FSS3       (1 << 3)
#define FIFO_FSS2       (1 << 2)
#define FIFO_FSS1       (1 << 1)
#define FIFO_FSS0       (1 << 0)

#define LSM303_INT1_CFG_A       0x30

#define INT_AOI         (1 << 7)
#define INT_6D          (1 << 6)
#define INT_ZHIE        (1 << 5)
#define INT_ZLIE        (1 << 4)
#define INT_YHIE        (1 << 3)
#define INT_YLIE        (1 << 2)
#define INT_XHIE        (1 << 1)
#define INT_XLIE        (1 << 0)

#define LSM303_INT1_SRC_A       0x31

#define INT_IA          (1 << 6)
#define INT_ZH          (1 << 5)
#define INT_ZL          (1 << 4)
#define INT_YH          (1 << 3)
#define INT_YL          (1 << 2)
#define INT_XH          (1 << 1)
#define INT_XL          (1 << 0)

#define LSM303_INT1_THS_A       0x32
#define LSM303_INT1_DURATION_A  0x33

#define LSM303_INT2_CFG_A       0x34
#define LSM303_INT2_SRC_A       0x35
#define LSM303_INT2_THS_A       0x36
#define LSM303_INT2_DURATION_A  0x37

#define LSM303_CLICK_CFG_A      0x38

#define CLICK_ZD        (1 << 5)
#define CLICK_ZS        (1 << 4)
#define CLICK_YD        (1 << 3)
#define CLICK_YS        (1 << 2)
#define CLICK_XD        (1 << 1)
#define CLICK_XS        (1 << 0)

#define LSM303_CLICK_SRC_A      0x39
#define LSM303_CLICK_THS_A      0x3a
#define LSM303_TIME_LIMIT_A     0x3b
#define LSM303_TIME_LATENCY_A   0x3c
#define LSM303_TIME_WINDOW_A    0x3d

/* -------------------------------------------------------------------------
   3D magnetometer 
   ------------------------------------------------------------------------- */

#define LSM303_CRA_REG_M        0x00
#define LSM303_CRB_REG_M        0x01
#define LSM303_MR_REG_M         0x02

#define LSM303_OUT_X_H_M        0x03
#define LSM303_OUT_X_L_M        0x04
#define LSM303_OUT_Y_H_M        0x05
#define LSM303_OUT_Y_L_M        0x06
#define LSM303_OUT_Z_H_M        0x07
#define LSM303_OUT_Z_L_M        0x08

#define LSM303_SR_REG_M         0x09
#define LSM303_IRA_REG_M        0x0a
#define LSM303_IRB_REG_M        0x0b
#define LSM303_IRC_REG_M        0x0c

#define LSM303_TEM_OUT_H_M      0x31
#define LSM303_TEM_OUT_L_M      0x32

#ifdef __cplusplus
extern "C" {
#endif

int lsm303_init(void);

#ifdef __cplusplus
}
#endif	

#endif // __LSM303DL_H__


