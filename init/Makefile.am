EXTRA_DIST = syspart.init syspart.sysconf

install-exec-local:
	$(INSTALL) -D -m 755 syspart.init $(DESTDIR)/etc/init.d/syspart
	$(INSTALL) -D -m 644 syspart.sysconf $(DESTDIR)/etc/sysconfig/syspart

uninstall-local:
	rm $(DESTDIR)/etc/init.d/syspart
	rm $(DESTDIR)/etc/sysconfig/syspart
