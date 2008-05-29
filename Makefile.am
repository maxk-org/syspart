MAKEFLAGS = -s

SUBDIRS = bitops

topsrc   = @abs_top_srcdir@
rpmbuild = $(topsrc)/rpm-build
rpmdir   = $(top_srcdir)/RPMS
instdir  = $(top_srcdir)/usr/sbin

DISTCLEANFILES = autom4te.cache

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 configure \
                       config.log config.status         \
                       config.sub depcomp install-sh    \
                       ltmain.sh missing config.guess   \
                       mkinstalldirs compile syspart.spec   \
		       Doxyfile

EXTRA_DIST = syspart.spec.in VERSION ChangeLog

real-clean: maintainer-clean
	find . -name Makefile.in | xargs rm
	rm -rf include
	rm -rf $(rpmbuild) $(rpmdir)
