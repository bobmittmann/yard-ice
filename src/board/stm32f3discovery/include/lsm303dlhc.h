/* 
   lsm320dlhc.h - ST LSM303DLHC: 3D accelerometer and 3D magnetometer 
 */

#ifndef __LSM303DLHC_H__
#define __LSM303DLHC_H__

/* -------------------------------------------------------------------------
   3D accelerometer
   ------------------------------------------------------------------------- */
#define LSM303_ACC_ADDR         0x19

#define LSM303_CTRL_REG1_A      0x20

#define CTRL_ODR3       (1 << 7)
#define CTRL_ODR2       (1 << 6)
#define CTRL_ODR1       (1 << 5)
#define CTRL_ODR0       (1 << 4)
#define CTRL_LPEN       (1 << 3)
#define CTRL_ZEN        (1 << 2)
#define CTRL_YEN        (1 << 1)
#define CTRL_XEN        (1 << 0)

#define ODR_POWER_DOWN  (0x0 << 4)
#define ODR_1HZ         (0x1 << 4)
#define ODR_10HZ        (0x2 << 4)
#define ODR_25HZ        (0x3 << 4)
#define ODR_50HZ        (0x4 << 4)
#define ODR_100HZ       (0x5 << 4)
#define ODR_200HZ       (0x6 << 4)
#define ODR_400HZ       (0x7 << 4)
#define ODR_1620HZ      (0x8 << 4)
#define ODR_5376HZ      (0x9 << 4)

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
/* AOI1 interrupt on INT1. Default value 0
(0: disable, 1: enable) */
#define CTRL_I1_AOI1    (1 << 6)
/* AOI2 interrupt on INT1. Default value 0
(0: disable, 1: enable) */
#define CTRL_I1_AOI2    (1 << 5)
/* DRDY1 interrupt on INT1. Default value 0
(0: disable, 1: enable) */
#define CTRL_I1_DRDY1   (1 << 4)
/* DRDY2 interrupt on INT1. Default value 0
(0: disable, 1: enable) */
#define CTRL_I1_DRDY2   (1 << 3)
/* FIFO watermark interrupt on INT1. Default value 0
(0: disable, 1: enable) */
#define CTRL_I1_WTM     (1 << 2)
/* FIFO overrun interrupt on INT1. Default value 0
(0: disable, 1: enable) */
#define CTRL_I1_OVERRUN (1 << 1)

#define LSM303_CTRL_REG4_A      0x23

/* Block data update. Default value: 0
(0: continuous update, 1: output registers not updated until MSB and
LSB have been read) */
#define CTRL_BDU        (1 << 7)
/* Big/little endian data selection. Default value 0.
(0: data LSB @ lower address, 1: data MSB @ lower address) */
#define CTRL_BLE        (1 << 6)
/* Full-scale selection. Default value: 00
(00: ±2 g, 01: ±4 g, 10: ±8 g, 11: ±16 g) */
#define CTRL_FS1        (1 << 5)
#define CTRL_FS0        (1 << 4)
#define CTRL_FS_2G      (0 << 4)
#define CTRL_FS_4G      (1 << 4)
#define CTRL_FS_8G      (2 << 4)
#define CTRL_FS_16G     (3 << 4)
 /* High-resolution output mode: Default value: 0
(0: high-resolution disable, 1: high-resolution enable) */
#define CTRL_HR         (1 << 3)
/* SPI serial interface mode selection. Default value: 0
(0: 4-wire interface, 1: 3-wire interface). */
#define CTRL_SIM        (1 << 0)

#define LSM303_CTRL_REG5_A      0x24

/* Reboot memory content. Default value: 0
(0: normal mode, 1: reboot memory content) */
#define CTRL_BOOT       (1 << 7)
/* FIFO enable. Default value: 0
(0: FIFO disable, 1: FIFO enable) */
#define CTRL_FIFO_EN    (1 << 6)
/* Latch interrupt request on INT1_SRC register, with INT1_SRC register cleared by
reading INT1_SRC itself. Default value: 0.
(0: interrupt request not latched, 1: interrupt request latched) */
#define CTRL_LIR_INT1   (1 << 3)
/* 4D enable: 4D detection is enabled on INT1 when 6D bit on INT1_CFG is set to 1. */
#define CTRL_D4D_INT1   (1 << 2)
/*  Latch interrupt request on INT2_SRC register, with INT2_SRC register cleared by
reading INT2_SRC itself. Default value: 0.
(0: interrupt request not latched, 1: interrupt request latched) */
#define CTRL_LIR_INT2   (1 << 1)
/* 4D enable: 4D detection is enabled on INT2 when 6D bit on INT2_CFG is set to 1. */
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

#define LSM303_MAG_ADDR         0x1e

#define LSM303_CRA_REG_M        0x00
#define CRA_TEMP_EN       (1 << 7)
#define CRA_DO2           (1 << 4)
#define CRA_DO1           (1 << 3)
#define CRA_DO0           (1 << 2)

#define CRA_DO_0_75_HZ    (0 << 2)
#define CRA_DO_1_5_HZ     (1 << 2)
#define CRA_DO_3_0_HZ     (2 << 2)
#define CRA_DO_7_5_HZ     (3 << 2)
#define CRA_DO_15_HZ      (4 << 2)
#define CRA_DO_30_HZ      (5 << 2)
#define CRA_DO_75_HZ      (6 << 2)
#define CRA_DO_220_HZ     (7 << 2)

#define LSM303_CRB_REG_M        0x01
#define CRB_GN2           (1 << 7)
#define CRB_GN1           (1 << 6)
#define CRB_GN0           (1 << 5)
#define CRB_GN_1_3        (1 << 5) /* ±1.3 Gauss */
#define CRB_GN_1_9        (2 << 5) /* ±1.9 Gauss */
#define CRB_GN_2_5        (3 << 5) /* ±2.5 Gauss */
#define CRB_GN_4_0        (4 << 5) /* ±4.0 Gauss */
#define CRB_GN_4_7        (5 << 5) /* ±4.7 Gauss */
#define CRB_GN_5_6        (6 << 5) /* ±5.6 Gauss */
#define CRB_GN_8_1        (7 << 5) /* ±8.1 Gauss */

#define LSM303_MR_REG_M         0x02
#define MR_MD1           (1 << 1)
#define MR_MD0           (1 << 0)
#define MR_MD_CONTINOUS  (0 << 0)
#define MR_MD_SINGLE     (1 << 0)
#define MR_MD_SLEEP      (2 << 0)

#define LSM303_OUT_X_H_M        0x03
#define LSM303_OUT_X_L_M        0x04
#define LSM303_OUT_Y_H_M        0x05
#define LSM303_OUT_Y_L_M        0x06
#define LSM303_OUT_Z_H_M        0x07
#define LSM303_OUT_Z_L_M        0x08

#define LSM303_SR_REG_M         0x09
#define SR_LOCK          (1 << 1)
#define SR_DRDY          (1 << 0)

#define LSM303_IRA_REG_M        0x0a
#define LSM303_IRB_REG_M        0x0b
#define LSM303_IRC_REG_M        0x0c

#define LSM303_TEM_OUT_H_M      0x31
#define LSM303_TEM_OUT_L_M      0x32

struct vector {
	int x;
	int y;
	int z;
};

#ifdef __cplusplus
extern "C" {
#endif

int lsm303_init(void);

/* ----------------------------------------------------------------------
 * Register level API
 * ----------------------------------------------------------------------
 */
int lsm303_acc_wr(unsigned int reg, const void * buf, unsigned int len);

int lsm303_acc_rd(unsigned int reg, void * buf, unsigned int len);

int lsm303_mag_wr(unsigned int reg, const void * buf, unsigned int len);

int lsm303_mag_rd(unsigned int reg, void * buf, unsigned int len);

/* ----------------------------------------------------------------------
 * Magnetomer driver API
 * ----------------------------------------------------------------------
 */
void lsm303_mag_vec_get(struct vector * v);

void lsm303_mag_init(void);

/* ----------------------------------------------------------------------
 * Accelerometer driver API
 * ----------------------------------------------------------------------
 */

void lsm303_acc_vec_get(struct vector * v);

void lsm303_acc_init(void);

#ifdef __cplusplus
}
#endif	

#endif // __LSM303DLHC_H__


