#
# altera.mk
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


THISDIR := $(dir $(lastword $(MAKEFILE_LIST)))
MKDIR := $(realpath $(THISDIR))
BASEDIR := $(realpath $(THISDIR)/..)
TOOLSDIR := $(realpath $(THISDIR)/../../tools)

#------------------------------------------------------------------------------ 
# default output directories
#------------------------------------------------------------------------------ 
RELEASE_DIR = release
DEBUG_DIR = debug

#------------------------------------------------------------------------------ 
# parameters 
#------------------------------------------------------------------------------ 
ifdef V
  verbose = $(V)
else
  verbose = 0
endif

ifdef D
  dbg_level = $(D)
else
  dbg_level = 0
endif

ifndef OUTDIR
  ifdef O
    OUTDIR = $(abspath $(O))
  else
    ifneq ($(dbg_level),0)
      OUTDIR = $(abspath $(DEBUG_DIR))
    else
      OUTDIR = $(abspath $(RELEASE_DIR))
    endif
  endif
export OUTDIR 
endif

ifndef Q
  ifeq ($(verbose),0) 
    Q = @
    ACTION = @echo
  else
    Q =
    ACTION = @\#
  endif
export Q ACTION
endif

ifndef INSTALL_DIR
INSTALL_DIR := $(OUTDIR)
endif

###################################################################
# Altera Configuration
###################################################################

FAMILY := Cyclone
DEVICE := EP1C3T100C8
OPTIMIZE := speed
EFFORT := fast

QT_MAP = quartus_map
QT_FIT = quartus_fit 
QT_ASM = quartus_asm

###################################################################
# Executable Configuration
###################################################################

#MAP_ARGS := --optimize=$(OPTIMIZE) --effort=$(EFFORT) --parallel
#FIT_ARGS := --part=$(DEVICE) --parallel --effort=$(EFFORT) --64bit
MAP_ARGS := 
FIT_ARGS := 
ASM_ARGS :=
ASM_ARGS :=

FPGA_QPF = $(OUTDIR)/$(FPGA_PROJ).qpf
FPGA_QSF = $(OUTDIR)/$(FPGA_PROJ).qsf
FPGA_QDF = $(OUTDIR)/$(FPGA_PROJ).qdf
FPGA_RBF = $(OUTDIR)/$(FPGA_PROJ).rbf
FPGA_MAP = $(OUTDIR)/map.done
FPGA_FIT = $(OUTDIR)/fit.done
FPGA_ASM = $(OUTDIR)/asm.done
FPGA_STA = $(OUTDIR)/sta.done
MAP_LOG = $(OUTDIR)/map.log
FIT_LOG = $(OUTDIR)/fit.log
ASM_LOG = $(OUTDIR)/asm.log
STA_LOG = $(OUTDIR)/sta.log
MAP_RPT = $(OUTDIR)/$(FPGA_PROJ).map.rpt
FIT_RPT = $(OUTDIR)/$(FPGA_PROJ).fit.rpt
ASM_RPT = $(OUTDIR)/$(FPGA_PROJ).asm.rpt
STA_RPT = $(OUTDIR)/$(FPGA_PROJ).sta.rpt
FLOW_RPT = $(OUTDIR)/$(FPGA_PROJ).flow.rpt

VHDL_FILES := $(abspath $(VHDL_FILES))
LOCATIONS := $(abspath $(LOCATIONS))
INSTANCES := $(abspath $(INSTANCES))

###################################################################
# Targets
###################################################################
fpga-all: $(FPGA_RBF)

fpga-clean:
	$(Q)rm -f $(FPGA_QPF) $(FPGA_QSF) $(FPGA_QDF) $(FPGA_MAP) $(FPGA_FIT); \
	rm -f $(FPGA_ASM) $(FPGA_STA) $(FPGA_RBF); \
	rm -f $(MAP_LOG) $(FIT_LOG) $(ASM_LOG) $(STA_LOG); \
	rm -f $(MAP_RPT) $(FIT_RPT) $(ASM_RPT) $(STA_RPT) $(FLOW_RPT); \
	cd $(OUTDIR); \
	rm -rf *.summary *.smsg *.eqn *.pin *.sof \
	   *.pof *.qws db incremental_db

fpga-install:
	$(Q)if [ ! -d $(INSTALL_DIR) ]; then\
		mkdir $(INSTALL_DIR);\
	fi;
	$(Q)echo "INSTALL: $(INSTALL_DIR)"
	$(Q)cp -f $(FPGA_RBF) $(INSTALL_DIR)

fpga-qpf: $(FPGA_QPF)

fpga-qsf: $(FPGA_QSF)

fpga-qdf: $(FPGA_QDF)

fpga-map: $(FPGA_MAP)

fpga-fit: $(FPGA_FIT)

fpga-asm: $(FPGA_ASM)

fpga-sta: $(FPGA_STA)

.PHONY: fpga-all fpga-clean fpga-install fpga-qpf fpga-qsf fpga-qdf \
		fpga-map fpga-fit fpga-asm

###################################################################
# Target implementations
###################################################################

$(FPGA_QPF): 
	$(Q)echo "PROJECT_REVISION = \"$(FPGA_PROJ)\"" > $@;\
	echo >> $@

$(FPGA_QDF): $(VHDL_FILES) $(LOCATIONS) $(INSTANCES) $(OUTDIR)
	$(Q)echo "set_global_assignment -name FAMILY \"$(FAMILY)\"" > $@;\
	echo "set_global_assignment -name DEVICE \"$(DEVICE)\"" >> $@;\
	echo "set_global_assignment -name TOP_LEVEL_ENTITY $(FPGA_PROJ)" >> $@;\
	echo "set_global_assignment -name OPTIMIZE_HOLD_TIMING $(OPTIMIZE_HOLD_TIMING)" >> $@;\
	echo "set_global_assignment -name OPTIMIZE_MULTI_CORNER_TIMING $(OPTIMIZE_POWER_DURING_SYNTHESIS)" >> $@;\
	echo "set_global_assignment -name HDL_MESSAGE_LEVEL $(HDL_MESSAGE_LEVEL)" >> $@;\
	echo "set_global_assignment -name STATE_MACHINE_PROCESSING \"$(STATE_MACHINE_PROCESSING)\"" >> $@;\
	echo "set_global_assignment -name SYNTH_MESSAGE_LEVEL $(SYNTH_MESSAGE_LEVEL)" >> $@;\
	echo "set_global_assignment -name ENABLE_INIT_DONE_OUTPUT OFF" >> $@;\
	echo "set_global_assignment -name USE_CONFIGURATION_DEVICE OFF" >> $@;\
	echo "set_global_assignment -name RESERVE_ASDO_AFTER_CONFIGURATION \"USE AS REGULAR IO\"" >> $@;\
	echo "set_global_assignment -name CYCLONE_CONFIGURATION_SCHEME \"PASSIVE SERIAL\"" >> $@;\
	echo "set_global_assignment -name GENERATE_RBF_FILE ON" >> $@;\
	for f in $(VHDL_FILES) ; do\
		echo "set_global_assignment -name VHDL_FILE $$f" >> $@;\
	done;\
	cat $(LOCATIONS) | sed -n "/[ \t]*#/!s/ *\(\<[A-Z0-9_]\+\) \+\([][A-Za-z0-9_]\+\) */set_location_assignment \1 -to \2/p" >> $@;\
	cat $(INSTANCES) | sed -n "/[ \t]*#/!s/ *\(\<[A-Za-z0-9_ ]\+\), \+\([][A-Za-z0-9_ ]\+\) */set_instance_assignment -name \1 -to \2/p" >> $@;\
	echo >> $@
	@cp $@ $(FPGA_QSF)

$(FPGA_QSF): $(FPGA_QDF)
	$(Q)cp $(FPGA_QDF) $@

$(FPGA_MAP): $(FPGA_QDF)
	$(Q)cd $(OUTDIR);\
	rm -f $@;\
	$(QT_MAP) $(MAP_ARGS) $(FPGA_PROJ) >  $(MAP_LOG) && echo done > $@;\
	cat $(MAP_LOG) |\
	sed "s/File:.*Line: [0-9]\+//" |\
	sed -n "/^[A-Z][a-z]\+ ([0-9]\+):/s/\([A-Za-z]\+\) ([0-9]\+): *\(.*\) \(at\|in\) \([A-Za-z0-9._-]\+\)(\([0-9]\+\))[ :]*\(.*\)/\4:\5: \L\1\E \2 \6/p";\
	if [ ! -f $@ ]; then exit 1; fi

$(FPGA_FIT): $(FPGA_MAP)
	$(Q)cd $(OUTDIR);\
	rm -f $@;\
	$(QT_FIT) $(FIT_ARGS) $(FPGA_PROJ) > $(FIT_LOG) && echo done > $@;\
	cat $(FIT_LOG) | sed "/[ ]*Info:.*/d;/^[ \t]\+/d;/[ ]*Note:/d";\
	if [ ! -f $@ ]; then exit 1; fi

$(FPGA_ASM): $(FPGA_FIT)
	$(Q)cd $(OUTDIR);\
	($(QT_ASM) $(ASM_ARGS) $(FPGA_PROJ) && echo done > $@) | \
	sed "/[ ]\+Info:.*/d"

$(FPGA_STA): $(FPGA_FIT)
	$(Q)cd $(OUTDIR);\
	(quartus_sta $(TAN_ARGS) $(FPGA_PROJ) && echo done > $@) | \
	sed "/[ ]\+Info:.*/d"

$(FPGA_RBF): $(FPGA_ASM)

$(OUTDIR):
	$(ACTION) "Creating: $@"
	$(Q) mkdir -p $@
