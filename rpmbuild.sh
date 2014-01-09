#!/bin/bash

pkg=syspart

if [ ! -e $pkg.spec ] ; then
	echo "Running ./bootstrap first"
	./bootstrap || { echo "Error: ${FD}/bootstrap errored. Quiting."; exit 1; }
	echo "Now running ./configure"
	./configure || { echo "Error: ${FD}/configure failed. See config.log for details."; exit 2; }
fi

make rpm
