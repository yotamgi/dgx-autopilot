CXXFLAGS += -Wall
CXXFLAGS += -I/home/yotam/stuff/armadillo-2.2.4/include/
ifneq ($(shell grep ARM /proc/cpuinfo),)
	CXXFLAGS += -march=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=hard -ffast-math -fsingle-precision-constant
endif

export CXXFLAGS

.PHONY:
all:
	make -C  libdata_stream/  all
	make -C  simulator/       all
	make -C  stream_watch/    all
	make -C  autopilot        all
.PHONY:
host:
	make -C  libdata_stream/  host
	make -C  simulator/       host
	make -C  stream_watch/    host
	make -C  autopilot        host
.PHONY:
device:
	make -C  libdata_stream/  device
	make -C  simulator/       device
	make -C  stream_watch/    device
	make -C  autopilot        device
.PHONY:
clean:
	make -C  libdata_stream/  clean
	make -C  simulator/       clean
	make -C  stream_watch/    clean
	make -C  autopilot        clean
