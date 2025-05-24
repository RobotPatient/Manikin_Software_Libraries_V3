COINES_INSTALL_PATH ?= ../../../..

EXAMPLE_FILE ?= cali_seq.c

BHY_INTF ?= 

COINES_BACKEND ?= COINES_BRIDGE

API_LOCATION ?= ../..

FW_LOCATION ?= ../..

COMMON_LOCATION ?= ..

C_SRCS += \
$(COMMON_LOCATION)/common/common.c \
$(API_LOCATION)/bhy.c \
$(API_LOCATION)/bhy_hif.c \
$(API_LOCATION)/bhy_parse.c \
$(API_LOCATION)/bhy_virtual_sensor_conf_param.c \
$(API_LOCATION)/bhy_phy_sensor_ctrl_param.c \
$(API_LOCATION)/bhy_virtual_sensor_info_param.c \
$(API_LOCATION)/bhy_system_param.c \
$(API_LOCATION)/bhy_event_data.c \

INCLUDEPATHS += . \
$(COMMON_LOCATION)/common \
$(API_LOCATION) \
$(FW_LOCATION)/firmware 

# By default, the sensor is connected over SPI. Define this to change to I2C
ifeq ($(BHY_INTF), I2C)
CFLAGS += -DBHY_USE_I2C
endif

ifeq ($(TARGET),$(filter $(TARGET),MCU_APP30 MCU_APP31))
LOCATION := FLASH
endif

include $(COINES_INSTALL_PATH)/coines.mk