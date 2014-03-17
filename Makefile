#******************************************************************************
#
# Makefile - Rules for building a TivaWare project
#
# Copyright (c) 2012-2013 Texas Instruments Incorporated.  All rights reserved.
# Software License Agreement
# 
# Texas Instruments (TI) is supplying this software for use solely and
# exclusively on TI's microcontroller products. The software is owned by
# TI and/or its suppliers, and is protected under applicable copyright
# laws. You may not combine this software with "viral" open-source
# software in order to form a larger program.
# 
# THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
# NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
# NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
# CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
# DAMAGES, FOR ANY REASON WHATSOEVER.
# 
# This is part of revision 1.1 of the EK-TM4C123GXL Firmware Package.
#
#******************************************************************************

#
# Defines the part type that this project uses.
#
PART=TM4C123GH6PM
#PART=TM4C1294NCPDT

#
# The base directory for TivaWare.
#
ifdef TIVAWARE
ROOT=${TIVAWARE}
else
ROOT=/home/ebrundic/Documents/Tiva/TivaWare
endif

# Program name
PROGNAME=nokia1202

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

#
# Where to find header files that do not live in the source directory.
#
IPATH=${ROOT}


#
# The default rule, which causes the project to be built.
#
all: ${COMPILER}
all: ${COMPILER}/${PROGNAME}.axf

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir -p ${COMPILER}

#
# Rules for building the project.  Add additional ${COMPILER}/srcfile.o type of entries for additional files.
#
${COMPILER}/${PROGNAME}.axf: ${COMPILER}/nokia1202_drv.o
${COMPILER}/${PROGNAME}.axf: ${COMPILER}/ste2007.o
${COMPILER}/${PROGNAME}.axf: ${COMPILER}/font_5x7.o
${COMPILER}/${PROGNAME}.axf: ${COMPILER}/devoptab.o
${COMPILER}/${PROGNAME}.axf: ${COMPILER}/${PROGNAME}.o
#${COMPILER}/${PROGNAME}.axf: ${COMPILER}/example_tm4c1294ncpdt.o
${COMPILER}/${PROGNAME}.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/${PROGNAME}.axf: ${PART}.ld
SCATTERgcc_${PROGNAME}=${PART}.ld
ENTRY_${PROGNAME}=ResetISR

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
