all :
	cd p1 && make

clean :
	find . -name "*~" -exec rm {} \;
	find . -name "*.mpi" -exec rm {} \;
