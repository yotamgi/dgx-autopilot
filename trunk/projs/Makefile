CXXFLAGS += -Wall
ifneq ($(shell grep ARM /proc/cpuinfo),)
	CXXFLAGS += -march=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=hard -ffast-math -fsingle-precision-constant
endif

export CXXFLAGS

.PHONY:
all:
	make -C  autopilot        all
	make -C  libdata_stream/  all
	make -C  simulator/       all
	make -C  stream_watch/    all
.PHONY:
host:
	make -C  autopilot        host
	make -C  libdata_stream/  host
	make -C  simulator/       host
	make -C  stream_watch/    host
.PHONY:
device:
	make -C  autopilot        device
	make -C  libdata_stream/  device
	make -C  simulator/       device
	make -C  stream_watch/    device
.PHONY:
clean:
	make -C  autopilot        clean
	make -C  libdata_stream/  clean
	make -C  simulator/       clean
	make -C  stream_watch/    clean
