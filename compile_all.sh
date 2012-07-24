# first compile projs
cd projs
cmake .
if [ $? != 0 ]; then
    exit $?
fi
make $*
if [ $? != 0 ]; then
    exit $?
fi
cd ..

# now, for each app, configure it to work with the local dgx
cd apps
for app in demo_app cockpit_player
do
	cd $app
	cmake . -Ddgx_DIR=../../projs
	if [ $? != 0 ]; then
	    exit $?
	fi
	make $*
	if [ $? != 0 ]; then
		exit $?
	fi
	cd ..
done

