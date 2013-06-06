all:
	$(MAKE) -C libs

clean:
	$(RM) *.o *.a *.exe

rebuild: clean all
