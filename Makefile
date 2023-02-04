#	Makefile - for weather station base station
#
#	Copyright David Haworth
#
#	This file is part of weather-station.
#
#	weather-station is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	weather-station is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with weather-station.  If not, see <http://www.gnu.org/licenses/>.

#	Targets:
#		clean: removes all object and binary files
#		default: compiles and links
#		srec: objcopy the ELF to an S-record file in the bin directory

# Find out where we are :-)
PRJ_ROOT		?= $(shell pwd)
DAVROS_ROOT		?= $(shell (cd $(PRJ_ROOT)/../../davros ; pwd))
DAVROSKA_ROOT	= $(DAVROS_ROOT)/davroska

PROJECT			?= weather-station

# ARM gcc  ToDo: use clang?
GNU_D		?=	/data1/gnu/gcc-arm-none-eabi-9-2019-q4-major

WS_CC		?=	$(GNU_D)/bin/arm-none-eabi-gcc
WS_LD		?=	$(GNU_D)/bin/arm-none-eabi-ld
WS_OBJCOPY	?=	$(GNU_D)/bin/arm-none-eabi-objcopy
WS_LDLIB_D	?=	$(GNU_D)/arm-none-eabi/lib/thumb/v7-m/nofp

LDSCRIPT	?=	script/dv-blue-pill.ldscript

ENTRY		?=	-e dv_reset

IMAGE_FILE	?=	build/$(PROJECT).bin
SREC_FILE	?=	build/$(PROJECT).srec

BIN_D		= build
OBJ_D		= build/obj

CC_OPT		+= -mcpu=cortex-m3
CC_OPT		+=	-D DV_DAVROSKA=1
CC_OPT		+= -I h
CC_OPT		+= -I $(DAVROSKA_ROOT)/h
CC_OPT		+= -I $(DAVROSKA_ROOT)/hardware
CC_OPT		+= -I $(DAVROS_ROOT)/devices/h
CC_OPT		+= -I $(DAVROS_ROOT)/lib/h
CC_OPT		+= -Wall
CC_OPT		+= -fno-common
CC_OPT		+= -ffunction-sections -fdata-sections
CC_OPT		+= -g

CC_OPT		+= -O2

#CC_OPT		+= -mfloat-abi=hard
#CC_OPT		+= -mfpu=vfp

LD_OPT		+= $(ENTRY)
LD_OPT		+= -T $(LDSCRIPT)
LD_OPT		+=	-L $(WS_LDLIB_D)
LD_OPT		+=	-lc
LD_OPT		+=	--gc-sections

# The project code
LD_OBJS		+= $(OBJ_D)/weather-station.o
LD_OBJS		+= $(OBJ_D)/led.o
LD_OBJS		+= $(OBJ_D)/gather.o
LD_OBJS		+= $(OBJ_D)/record.o
LD_OBJS		+= $(OBJ_D)/convert.o
LD_OBJS		+= $(OBJ_D)/tty1.o
LD_OBJS		+= $(OBJ_D)/tty2.o
LD_OBJS		+= $(OBJ_D)/ws-blue-pill.o

# davroska and associated library files
LD_OBJS		+= $(OBJ_D)/davroska.o
LD_OBJS		+= $(OBJ_D)/davroska-time.o
LD_OBJS		+= $(OBJ_D)/davroska-extended.o
LD_OBJS		+= $(OBJ_D)/dv-printf.o
LD_OBJS		+= $(OBJ_D)/dv-xprintf.o
LD_OBJS		+= $(OBJ_D)/dv-memset32.o

VPATH 		+=	c
VPATH 		+=	s
VPATH		+=	$(DAVROSKA_ROOT)/c

# Hardware-specific files and paths
LD_OBJS		+= $(OBJ_D)/davroska-cortexm.o
LD_OBJS		+= $(OBJ_D)/dv-cortexm-vectors.o
LD_OBJS		+= $(OBJ_D)/dv-switchcall.o
LD_OBJS		+= $(OBJ_D)/dv-cortexm-ctxsw.o
LD_OBJS		+= $(OBJ_D)/dv-nvic.o

LD_OBJS		+= $(OBJ_D)/dv-stm32-rcc.o
LD_OBJS		+= $(OBJ_D)/dv-stm32-uart.o
LD_OBJS		+= $(OBJ_D)/dv-stm32-timx.o

VPATH		+=	$(DAVROSKA_ROOT)/hardware/arm/c
VPATH		+=	$(DAVROSKA_ROOT)/hardware/arm/s
VPATH		+=	$(DAVROSKA_ROOT)/hardware/common/c
VPATH		+=	$(DAVROS_ROOT)/lib/c
VPATH		+=	$(DAVROS_ROOT)/devices/c
VPATH		+=	$(DAVROS_ROOT)/devices/s

.PHONY:		default all help clean install srec image

default:	image

clean:
	-rm -rf $(BIN_D)

elf:		$(OBJ_D) $(BIN_D)/$(PROJECT).elf

image:		$(OBJ_D) $(BIN_D) $(IMAGE_FILE)

srec:		$(OBJ_D) $(BIN_D) $(SREC_FILE)

$(BIN_D)/$(PROJECT).elf:	$(BIN_D) $(LD_OBJS)
	$(WS_LD) -o $@ $(LD_OBJS) $(LD_LIB) $(LD_OPT)

$(OBJ_D)/%.o:  %.c
	$(WS_CC) $(CC_OPT) -o $@ -c $<

$(OBJ_D)/%.o:  %.S
	$(WS_CC) $(CC_OPT) -o $@ -c $<

$(BIN_D):
	mkdir -p $(BIN_D)

$(OBJ_D):
	mkdir -p $(OBJ_D)

install:		$(OBJ_D) $(BIN_D) $(IMAGE_FILE)


$(IMAGE_FILE):	$(BIN_D)/$(PROJECT).elf
	$(WS_OBJCOPY) $(BIN_D)/$(PROJECT).elf -O binary $(IMAGE_FILE)
	echo "flash write_image erase $(IMAGE_FILE) 0x08000000"

$(SREC_FILE):	$(BIN_D)/$(PROJECT).elf
	$(WS_OBJCOPY) $(BIN_D)/$(PROJECT).elf -O srec --srec-forceS3 /dev/stdout | dos2unix | egrep -v '^S3..........00*..$$' > $(SREC_FILE)
