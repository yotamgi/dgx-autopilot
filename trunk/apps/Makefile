.PHONY:
all:
	make -C  demo_app/  all
	make -C  waypoint_pilot_test/  all
	make -C  cockpit_test 	 all
.PHONY:
host:
	make -C  demo_app/  all
	make -C  waypoint_pilot_test/  host
	make -C  cockpit_test/   host
.PHONY:
device:
	make -C  demo_app/  all
	make -C  waypoint_pilot_test/  device
	make -C  cockpit_test/   device
.PHONY:
clean:
	make -C  demo_app/  all
	make -C  waypoint_pilot_test/  clean
	make -C  cockpit_test/   clean
 
