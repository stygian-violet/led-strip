ARDMK_DIR := lib/Arduino-Makefile
SRC_DIR := src
USER_LIB_PATH := lib

# ATtiny / Arduino / OpenCM / OpenCR / Sam / Teensy / chipKIT
BOARD_TYPE := ATtiny
# From boards.txt
BOARD_TAG := attinyx5micr
# BOARD_TAG.menu.cpu.* from boards.txt
BOARD_SUB :=
# BOARD_TAG.menu.clock.* from boards.txt
BOARD_CLOCK := 16pll
#
ALTERNATE_CORE_PATH := lib/ATTinyCore/avr

ARDUINO_LIBS := FAB_LED

CFLAGS_STD := -std=gnu++20
CXXFLAGS_STD := -std=gnu++20

include $(ARDMK_DIR)/$(BOARD_TYPE).mk
