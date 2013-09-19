#!/bin/bash

# Everything is relative to the working directory
cd $(dirname $0)
FD=$(pwd)

rm -rf RPM
mkdir RPM
mkdir RPM/build
mkdir RPM/pkg
mkdir RPM/root

rpm=`pwd`/RPM
src=`pwd`

pkg=syspart

if [ ! -e $pkg.spec ] ; then
	echo "Running ./bootstrap first"
	./bootstrap || { echo "Error: ${FD}/bootstrap errored. Quiting."; exit 1; }
	echo "Now running ./configure"
	./configure || { echo "Error: ${FD}/configure failed. See config.log for details."; exit 2; }
fi

ver=$(rpm -q --qf '%{version} ' --specfile $pkg.spec | cut -d' ' -f1)

git archive --format=tar --prefix=$pkg-$ver/ HEAD | \
        gzip -9 > $src/$pkg-$ver.tar.gz


rpmbuild --buildroot="$rpm/root"         \
	 --define="_rpmdir $rpm/pkg"     \
	 --define="_builddir $rpm/build" \
	 --define="_sourcedir $src"   \
	 --define='_unpackaged_files_terminate_build 0' \
	 -bb $pkg.spec
