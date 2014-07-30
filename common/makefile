all:
	$(MAKE) -C debugging
	$(MAKE) -C gameplay
	$(MAKE) -C graphics
	$(MAKE) -C map
	$(MAKE) -C network
	$(MAKE) -C ui
	$(MAKE) -C utilities

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all
