CXXFLAGS += -Wall
ifneq ($(shell grep ARM /proc/cpuinfo),)
	CXXFLAGS += -march=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=hard -ffast-math -fsingle-precision-constant
endif

export CXXFLAGS

.PHONY:
all:
	make -C  libdata_stream/  all
	make -C  simulator/       all
	make -C  ground_station/  all
	make -C  autopilot        all
.PHONY:
host:
	make -C  libdata_stream/  host
	make -C  simulator/       host
	make -C  ground_station/  host
	make -C  autopilot        host
.PHONY:
device:
	make -C  libdata_stream/  device
	make -C  simulator/       device
	make -C  ground_station/  device
	make -C  autopilot        device
.PHONY:
clean:
	make -C  libdata_stream/  clean
	make -C  simulator/       clean
	make -C  ground_station/  clean
	make -C  autopilot        clean
