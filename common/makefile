all:
	$(MAKE) -C gameplay
	$(MAKE) -C graphics
	$(MAKE) -C map
	$(MAKE) -C network
	$(MAKE) -C script
	$(MAKE) -C ui
	$(MAKE) -C utilities

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all
