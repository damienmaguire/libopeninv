# Cross-compile every library source for one STM32 family.
# This does not link or produce firmware. FAMILY is STM32F1 or STM32F4.
FAMILY ?= STM32F1
PREFIX ?= arm-none-eabi-
LIBOPENCM3 ?= ../../libopencm3

CXX = $(PREFIX)g++
CC = $(PREFIX)gcc

ifeq ($(FAMILY),STM32F4)
ARCH = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
else ifeq ($(FAMILY),STM32F1)
ARCH = -mcpu=cortex-m3 -mthumb -mfloat-abi=soft
else
$(error FAMILY must be STM32F1 or STM32F4)
endif

CXXFLAGS = $(ARCH) -std=c++17 -Wall -Wextra -Wno-register -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti
CFLAGS = $(ARCH) -std=c99 -Wall -Wextra -ffunction-sections -fdata-sections
CPPFLAGS = -D$(FAMILY) -Ifamily-include -I../include -I$(LIBOPENCM3)/include

OBJDIR = obj-$(FAMILY)
CXXSRCS = $(wildcard ../src/*.cpp)
CSRCS = $(wildcard ../src/*.c)
OBJS = $(patsubst ../src/%.cpp,$(OBJDIR)/%.o,$(CXXSRCS)) \
       $(patsubst ../src/%.c,$(OBJDIR)/%.o,$(CSRCS)) \
       $(OBJDIR)/family_check.o

all: $(OBJS)

$(OBJDIR)/%.o: ../src/%.cpp | $(OBJDIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: ../src/%.c | $(OBJDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/family_check.o: family_check.cpp | $(OBJDIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR)

.PHONY: all clean
