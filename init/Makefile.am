EXTRADIST = syspart.init syspart.sysconf

install-exec-local:
	$(install) -m 755 syspart.init $(DESTDIR)/etc/init.d/syspart
	$(install) -m 644 syspart.sysconf $(DESTDIR)/etc/sysconfig/syspart

uninstall-local:
	rm $(DESTDIR)/etc/init.d/syspart
	rm $(DESTDIR)/etc/sysconfig/syspart
