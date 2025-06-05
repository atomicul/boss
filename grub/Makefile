.PHONY: install
install:
ifndef DESTDIR
	$(error you must specify $$DESTDIR)
endif
	mkdir -p $(DESTDIR)/boot/grub
	echo 'menuentry "boss" { multiboot /boot/boss.bin }' > $(DESTDIR)/boot/grub/grub.cfg
