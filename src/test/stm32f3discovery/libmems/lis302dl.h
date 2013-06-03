/*   lis320dl.h -  ST LIS302DL Accelerometer */

#ifndef __LIS302DL_H__
#define __LIS302DL_H__

#define LIS302_WHO_AM_I         0x0f
#define LIS302_CTRL_REG1        0x20

#define CTRL_DR   (1 << 7)
#define CTRL_PD   (1 << 6)
#define CTRL_FS   (1 << 5)
#define CTRL_STP  (1 << 4)
#define CTRL_STM  (1 << 3)
#define CTRL_ZEN  (1 << 2)
#define CTRL_YEN  (1 << 1)
#define CTRL_XEN  (1 << 0)

#define LIS302_CTRL_REG2        0x21

#define CTRL_SIM  (1 << 7)
#define CTRL_BOOT (1 << 6)
#define CTRL_FDS  (1 << 4)

#define LIS302_CTRL_REG3        0x22
#define LIS302_HP_FILTER_RESET  0x23

#define LIS302_STATUS_REG       0x27

#define STAT_ZYXOR  (1 << 7)
#define STAT_ZOR    (1 << 6)
#define STAT_YOR    (1 << 5)
#define STAT_XOR    (1 << 4)

#define STAT_ZYXDA  (1 << 3)
#define STAT_ZDA    (1 << 2)
#define STAT_YDA    (1 << 1)
#define STAT_XDA    (1 << 0)

#define LIS302_OUT_X            0x29
#define LIS302_OUT_Y            0x2b
#define LIS302_OUT_Z            0x2d

#define LIS302_FF_WU_CFG_1      0x30
#define LIS302_FF_WU_SRC_1      0x31
#define LIS302_FF_WU_THS_1      0x32
#define LIS302_FF_WU_DURATION_1 0x33

#define LIS302_FF_WU_CFG_2      0x34
#define LIS302_FF_WU_SRC_2      0x35
#define LIS302_FF_WU_THS_2      0x36
#define LIS302_FF_WU_DURATION_2 0x37

#define LIS302_CLICK_CFG        0x38
#define LIS302_CLICK_SRC        0x39
#define LIS302_CLICK_THSY_X     0x3b
#define LIS302_CLICK_THSZ       0x3c
#define LIS302_CLICK_TIME_LIMIT 0x3d
#define LIS302_CLICK_LATENCY    0x3e
#define LIS302_CLICK_WINDOW     0x3f

#endif // __LIS302DL_H__

