#/bin/bash

rm -rf RPM
mkdir RPM
mkdir RPM/build
mkdir RPM/pkg
mkdir RPM/root

rpm=`pwd`/RPM
src=`pwd`

rpmbuild --buildroot="$rpm/root"         \
	 --define="_rpmdir $rpm/pkg"     \
	 --define="_builddir $rpm/build" \
	 --define="_sourcedir $src"   \
	 --define='_unpackaged_files_terminate_build 0' \
	 -bb syspart.spec
