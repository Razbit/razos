all:
	$(MAKE) -C src

clean:
	$(MAKE) clean -C src

run:
	sudo ./update_image.sh
	sudo ./run_bochs.sh
