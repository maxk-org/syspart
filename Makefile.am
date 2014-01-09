include $(top_srcdir)/acversion.mk
include $(top_srcdir)/acsubst.mk

ACLOCAL_AMFLAGS = -I.

topsrc   = @abs_top_srcdir@
debbuild = $(topsrc)/deb-build
rpmbuild = $(topsrc)/rpm-build
rpmdir   = $(top_srcdir)/pkg

rpm: syspart.spec
	rm -rf $(rpmbuild)
	mkdir -p $(rpmdir)
	make DESTDIR=$(rpmbuild) install
	rpmbuild --buildroot=$(rpmbuild)     \
		--define='_rpmdir $(rpmdir)' \
		--define='_unpackaged_files_terminate_build 0' \
		-bb syspart.spec
	rm -rf $(rpmbuild)

builddeb: builddeb.in Makefile
	@echo Generating $@
	@$(acsubst) $< > $@

deb: builddeb
	sh builddeb $(debbuild)

EXTRA_DIST = syspart.spec.in VERSION ChangeLog

#
# Init related scripts
noinst_SCRIPTS = init/syspart.init init/syspart.sysconf init/syspart.service

install-exec-local:
	$(INSTALL) -D -m 755 init/syspart.init $(DESTDIR)/etc/init.d/syspart
	$(INSTALL) -D -m 644 init/syspart.sysconf $(DESTDIR)/etc/sysconfig/syspart
	$(INSTALL) -D -m 644 init/syspart.service $(DESTDIR)/usr/lib/systemd/system/syspart.service

uninstall-local:
	rm $(DESTDIR)/etc/init.d/syspart
	rm $(DESTDIR)/etc/sysconfig/syspart
	rm $(DESTDIR)/usr/lib/systemd/system
	rm $(DESTDIR)/etc/profile.d/syspart.sh

#
# Helper progs
bin_PROGRAMS   = bitops/bitops
bitops_bitops_SOURCES = bitops/bitmap.h bitops/bitmap.c bitops/bitops.c

#
# Main scripts
sbin_SCRIPTS   = tools/syspart
