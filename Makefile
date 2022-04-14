build:
	make  -C client
	make  -C server

debug:
	make debug -C client
	make debug -C server

clean:
	make clean -C client
	make clean -C server
