EXTRA_DIST = syspart.init syspart.sysconf syspart.service

install-exec-local:
	$(INSTALL) -D -m 755 syspart.init $(DESTDIR)/etc/init.d/syspart
	$(INSTALL) -D -m 644 syspart.sysconf $(DESTDIR)/etc/sysconfig/syspart
	$(INSTALL) -D -m 644 syspart.service $(DESTDIR)/usr/lib/systemd/system/syspart.service

uninstall-local:
	rm $(DESTDIR)/etc/init.d/syspart
	rm $(DESTDIR)/etc/sysconfig/syspart
	rm $(DESTDIR)/usr/lib/systemd/system
