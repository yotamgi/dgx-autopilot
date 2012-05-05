CXXFLAGS += -Wall
ifneq ($(shell grep ARM /proc/cpuinfo),)
	#CXXFLAGS += -march=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=hard -ffast-math -fsingle-precision-constant
	CXXFLAGS += -ftree-vectorize -ffast-math -fsingle-precision-constant
endif

export CXXFLAGS

.PHONY:
all:
	make -C  libdata_stream/  all
	make -C  autopilot        all
	make -C  simulator/       all
	make -C  ground_station/  all
.PHONY:
host:
	make -C  libdata_stream/  host
	make -C  autopilot        host
	make -C  simulator/       host
	make -C  ground_station/  host
.PHONY:
device:
	make -C  libdata_stream/  device
	make -C  autopilot        device
	make -C  simulator/       device
	make -C  ground_station/  device
.PHONY:
clean:
	make -C  libdata_stream/  clean
	make -C  autopilot        clean
	make -C  simulator/       clean
	make -C  ground_station/  clean
