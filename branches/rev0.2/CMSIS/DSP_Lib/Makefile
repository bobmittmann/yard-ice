#
# Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
# 
# This file is part of the YARD-ICE.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 3.0 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You can receive a copy of the GNU Lesser General Public License from 
# http://www.gnu.org/

#
# File:   Makefile
# Author: Robinson Mittmann <bobmittmann@gmail.com>
# 


LIB_STATIC = DSP_Lib

BASIC_MATH_CFILES = arm_abs_f32.c arm_abs_q15.c arm_abs_q31.c arm_abs_q7.c\
					arm_add_f32.c arm_add_q15.c arm_add_q31.c arm_add_q7.c\
					arm_dot_prod_f32.c arm_dot_prod_q15.c arm_dot_prod_q31.c\
					arm_dot_prod_q7.c arm_mult_f32.c arm_mult_q15.c\
					arm_mult_q31.c arm_mult_q7.c arm_negate_f32.c\
					arm_negate_q15.c arm_negate_q31.c arm_negate_q7.c\
					arm_offset_f32.c arm_offset_q15.c arm_offset_q31.c\
					arm_offset_q7.c arm_scale_f32.c arm_scale_q15.c\
					arm_scale_q31.c arm_scale_q7.c arm_shift_q15.c \
					arm_shift_q31.c arm_shift_q7.c arm_sub_f32.c arm_sub_q15.c\
					arm_sub_q31.c arm_sub_q7.c

COMMOM_TABLES_CFILES = arm_common_tables.c

COMPLEX_MATH_CFILES = arm_cmplx_conj_f32.c arm_cmplx_conj_q15.c\
					  arm_cmplx_conj_q31.c arm_cmplx_dot_prod_f32.c\
					  arm_cmplx_dot_prod_q15.c arm_cmplx_dot_prod_q31.c\
					  arm_cmplx_mag_f32.c arm_cmplx_mag_q15.c\
					  arm_cmplx_mag_q31.c arm_cmplx_mag_squared_f32.c\
					  arm_cmplx_mag_squared_q15.c arm_cmplx_mag_squared_q31.c\
					  arm_cmplx_mult_cmplx_f32.c arm_cmplx_mult_cmplx_q15.c\
					  arm_cmplx_mult_cmplx_q31.c arm_cmplx_mult_real_f32.c\
					  arm_cmplx_mult_real_q15.c arm_cmplx_mult_real_q31.c

CONTROLLER_CFILES = arm_pid_init_f32.c arm_pid_init_q15.c arm_pid_init_q31.c\
					arm_pid_reset_f32.c arm_pid_reset_q15.c\
					arm_pid_reset_q31.c arm_sin_cos_f32.c arm_sin_cos_q31.c

FAST_MATH_CFILES = arm_cos_f32.c arm_cos_q15.c arm_cos_q31.c arm_sin_f32.c\
				   arm_sin_q15.c arm_sin_q31.c arm_sqrt_q15.c arm_sqrt_q31.c

FILTERING_CFILES = arm_biquad_cascade_df1_32x64_init_q31.c\
				   arm_biquad_cascade_df1_32x64_q31.c\
				   arm_biquad_cascade_df1_f32.c\
				   arm_biquad_cascade_df1_fast_q15.c\
				   arm_biquad_cascade_df1_fast_q31.c\
				   arm_biquad_cascade_df1_init_f32.c\
				   arm_biquad_cascade_df1_init_q15.c\
				   arm_biquad_cascade_df1_init_q31.c\
				   arm_biquad_cascade_df1_q15.c arm_biquad_cascade_df1_q31.c\
				   arm_biquad_cascade_df2T_f32.c\
				   arm_biquad_cascade_df2T_init_f32.c arm_conv_f32.c\
				   arm_conv_fast_q15.c arm_conv_fast_q31.c\
				   arm_conv_partial_f32.c arm_conv_partial_fast_q15.c\
				   arm_conv_partial_fast_q31.c arm_conv_partial_q15.c\
				   arm_conv_partial_q31.c arm_conv_partial_q7.c\
				   arm_conv_q15.c arm_conv_q31.c arm_conv_q7.c\
				   arm_correlate_f32.c arm_correlate_fast_q15.c\
				   arm_correlate_fast_q31.c arm_correlate_q15.c\
				   arm_correlate_q31.c arm_correlate_q7.c\
				   arm_fir_decimate_f32.c arm_fir_decimate_fast_q15.c\
				   arm_fir_decimate_fast_q31.c arm_fir_decimate_init_f32.c\
				   arm_fir_decimate_init_q15.c arm_fir_decimate_init_q31.c\
				   arm_fir_decimate_q15.c arm_fir_decimate_q31.c\
				   arm_fir_f32.c arm_fir_fast_q15.c arm_fir_fast_q31.c\
				   arm_fir_init_f32.c arm_fir_init_q15.c arm_fir_init_q31.c\
				   arm_fir_init_q7.c arm_fir_interpolate_f32.c\
				   arm_fir_interpolate_init_f32.c\
				   arm_fir_interpolate_init_q15.c\
				   arm_fir_interpolate_init_q31.c\
				   arm_fir_interpolate_q15.c arm_fir_interpolate_q31.c\
				   arm_fir_lattice_f32.c arm_fir_lattice_init_f32.c\
				   arm_fir_lattice_init_q15.c arm_fir_lattice_init_q31.c\
				   arm_fir_lattice_q15.c arm_fir_lattice_q31.c arm_fir_q15.c\
				   arm_fir_q31.c arm_fir_q7.c arm_fir_sparse_f32.c\
				   arm_fir_sparse_init_f32.c arm_fir_sparse_init_q15.c\
				   arm_fir_sparse_init_q31.c arm_fir_sparse_init_q7.c\
				   arm_fir_sparse_q15.c arm_fir_sparse_q31.c\
				   arm_fir_sparse_q7.c arm_iir_lattice_f32.c\
				   arm_iir_lattice_init_f32.c arm_iir_lattice_init_q15.c\
				   arm_iir_lattice_init_q31.c arm_iir_lattice_q15.c\
				   arm_iir_lattice_q31.c arm_lms_f32.c\
				   arm_lms_init_f32.c arm_lms_init_q15.c\
				   arm_lms_init_q31.c arm_lms_norm_f32.c\
				   arm_lms_norm_init_f32.c arm_lms_norm_init_q15.c\
				   arm_lms_norm_init_q31.c arm_lms_norm_q15.c\
				   arm_lms_norm_q31.c arm_lms_q15.c arm_lms_q31.c

MATRIX_CFILES = arm_mat_add_f32.c arm_mat_add_q15.c arm_mat_add_q31.c\
				arm_mat_init_f32.c arm_mat_init_q15.c arm_mat_init_q31.c\
				arm_mat_inverse_f32.c arm_mat_mult_f32.c arm_mat_mult_fast_q15.c\
				arm_mat_mult_fast_q31.c arm_mat_mult_q15.c arm_mat_mult_q31.c\
				arm_mat_scale_f32.c arm_mat_scale_q15.c arm_mat_scale_q31.c\
				arm_mat_sub_f32.c arm_mat_sub_q15.c arm_mat_sub_q31.c\
				arm_mat_trans_f32.c arm_mat_trans_q15.c arm_mat_trans_q31.c

STATISTICS_CFILES = arm_max_f32.c arm_max_q15.c arm_max_q31.c arm_max_q7.c\
					arm_mean_f32.c arm_mean_q15.c arm_mean_q31.c arm_mean_q7.c\
					arm_min_f32.c arm_min_q15.c arm_min_q31.c arm_min_q7.c\
					arm_power_f32.c arm_power_q15.c arm_power_q31.c\
					arm_power_q7.c arm_rms_f32.c arm_rms_q15.c arm_rms_q31.c\
					arm_std_f32.c arm_std_q15.c arm_std_q31.c arm_var_f32.c\
					arm_var_q15.c arm_var_q31.c

SUPPORT_CFILES = arm_copy_f32.c arm_copy_q15.c arm_copy_q31.c arm_copy_q7.c\
				 arm_fill_f32.c arm_fill_q15.c arm_fill_q31.c arm_fill_q7.c\
				 arm_float_to_q15.c arm_float_to_q31.c arm_float_to_q7.c\
				 arm_q15_to_float.c arm_q15_to_q31.c arm_q15_to_q7.c\
				 arm_q31_to_float.c arm_q31_to_q15.c arm_q31_to_q7.c\
				 arm_q7_to_float.c arm_q7_to_q15.c arm_q7_to_q31.c

TRANSFORM_CFILES = arm_cfft_radix4_f32.c arm_cfft_radix4_init_f32.c\
				   arm_cfft_radix4_init_q15.c arm_cfft_radix4_init_q31.c\
				   arm_cfft_radix4_q15.c arm_cfft_radix4_q31.c arm_dct4_f32.c\
				   arm_dct4_init_f32.c arm_dct4_init_q15.c arm_dct4_init_q31.c\
				   arm_dct4_q15.c arm_dct4_q31.c arm_rfft_f32.c\
				   arm_rfft_init_f32.c arm_rfft_init_q15.c arm_rfft_init_q31.c\
				   arm_rfft_q15.c arm_rfft_q31.c

CFILES = $(addprefix Source/BasicMathFunctions/,$(BASIC_MATH_CFILES))\
		 $(addprefix Source/CommonTables/,$(COMMOM_TABLES_CFILES))\
		 $(addprefix Source/ComplexMathFunctions/,$(COMPLEX_MATH_CFILES))\
		 $(addprefix Source/ControllerFunctions/,$(CONTROLLER_CFILES))\
		 $(addprefix Source/FastMathFunctions/,$(FAST_MATH_CFILES))\
		 $(addprefix Source/FilteringFunctions/,$(FILTERING_CFILES))\
		 $(addprefix Source/MatrixFunctions/,$(MATRIX_CFILES))\
		 $(addprefix Source/StatisticsFunctions/,$(STATISTICS_CFILES))\
		 $(addprefix Source/SupportFunctions/,$(SUPPORT_CFILES))\
		 $(addprefix Source/TransformFunctions/,$(TRANSFORM_CFILES))\

override INCPATH += ../Include

include ../../src/mk/lib.mk

