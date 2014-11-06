/*   tlv320.h -  Texas Instruments TLV320 Audio Codec */

#ifndef __TLV320_H__
#define __TLV320_H__

/* TLV320 Control Register 1 */
#define TLV320_CR1   0x01 

#define CR1_ADOF     (1 << 7) // ADC overflow

#define CR1_CX       (1 << 6) // Continuous data transfer mode

#define CR1_IIR      (1 << 5)
#define CR1_FIR      (0 << 5)

#define CR1_DAOVF    (1 << 4) // DAC overflow

#define CR1_BIASV    (1 << 3) // Bias voltage
#define CR1_BIASV_HI (0 << 3) // 2.35V
#define CR1_BIASV_LO (1 << 3) // 1.35V

#define CR1_ALB      (1 << 2) // Analog loopback
#define CR1_DLB      (1 << 1) // Digital loopback

#define CR1_DAC15    (0 << 0) // 15 bits DAC format
#define CR1_DAC16    (1 << 0) // 16 bits DAC format

/* TLV320 Control Register 2 */
#define TLV320_CR2     0x02

#define CR2_TURBO      (1 << 7)

#define CR2_DIFBP      (1 << 6)

#define CR2_I2CX_SET(A)  (((A) & 0x7) << 3)

#define CR2_HPC_I2C    (0 << 0)
#define CR2_HPC_S2C    (1 << 0)
#define CR2_HPC_IO     (2 << 0)
#define CR2_HPC_CFF    (3 << 0)

/* TLV320 Control Register 3 */
#define TLV320_CR3     0x03

#define CR3_PWDN_NO    (0 << 6)
#define CR3_PWDN_ADC   (1 << 6)
#define CR3_PWDN_DAC   (2 << 6)
#define CR3_PWDN_DEV   (3 << 6)

#define CR3_SWRS       (1 << 5)

#define CR3_OSR_128    (0 << 3)
#define CR3_OSR_512    (1 << 3)
#define CR3_OSR_256    (2 << 3)

#define CR3_ASRF_1      (1 << 0)
#define CR3_ASRF_2      (2 << 0)
#define CR3_ASRF_3      (3 << 0)
#define CR3_ASRF_4      (4 << 0)
#define CR3_ASRF_5      (5 << 0)
#define CR3_ASRF_6      (6 << 0)
#define CR3_ASRF_7      (7 << 0)
#define CR3_ASRF_8      (0 << 0)

/* TLV320 Control Register 4 */
#define TLV320_CR4     0x03

#define CR4_FSDIV_NP    (0 << 7)
#define CR4_FSDIV_M     (1 << 7)

#define CR4_M_SET(M)  (CR4_FSDIV_M | ((M) & 0x7f))
#define CR4_NP_SET(N, P)  (CR4_FSDIV_NP | (((N) & 0xf) << 3) | ((P) & 0x7))
#define CR4_N_SET(N)  (CR4_FSDIV_NP | ((N) & 0x7))
#define CR4_P_SET(P)  (CR4_FSDIV_NP | (((P) & 0xf) << 3))

/* TLV320 Control Register 5 */
#define TLV320_CR5     0x05

#define CR5_A (0 << 6)

#define CR5A_ADGAIN_MUTE (0x3f << 0)
// Set the ADC gain in dB (-42 to 20)
#define CR5A_ADGAIN_DB(VAL) CR5_A | (((VAL) + 42) & 0x3f)


#define CR5_B (1 << 6)

#define CR5B_DAGAIN_MUTE (0x3f << 0)
/* Set the DAC gain in dB (-42 to 20) */
#define CR5B_DAGAIN_DB(VAL) CR5_B | (((VAL) + 42) & 0x3f)


#define CR5_C (2 << 6)

#define CR5C_DGSTG_MUTE (0x7 << 3)
/* Set the DAC gain in dB (-21 to -3) */
#define CR5C_DGSTG_DB(VAL) CR5_C | ((((-3 - (VAL)) / 3) & 0x3f) << 3)

/* Input Buffer Gain */
#define CR5C_INBG_0DB  CR5_C | (0 << 0)
#define CR5C_INBG_6DB  CR5_C | (1 << 0)
#define CR5C_INBG_12DB CR5_C | (2 << 0)
#define CR5C_INBG_24DB CR5_C | (3 << 0)

#define CR5_D (3 << 6)

/* TLV320 Control Register 6 */
#define TLV320_CR6     0x06

#define CR3_PSDO       (1 << 7)

#define CR3_MUTE2      (1 << 6)
#define CR3_MUTE3      (1 << 5)

#define CR3_OUTP3_ON   (1 << 4)
#define CR3_OUTP2_ON   (1 << 3)

#define CR3_AINSEL_INP_M1  (0 << 1)
#define CR3_AINSEL_MIC_SB  (1 << 1)
#define CR3_AINSEL_MIC_ECM (2 << 1)
#define CR3_AINSEL_INP_M2  (3 << 1)


#define TLV320_NOP 0, 0
#define TLV320_WR(REG, DATA) 0, ((REG) << 13) | (0x7 << 8) | ((DATA) & 0xff)
#define TLV320_RD(REG, OPT)  0, (((REG) << 13) | (1 << 12) | \
                                 (0x7 << 8) | ((OPT) & 0xff))


#endif // __TLV320_H__

