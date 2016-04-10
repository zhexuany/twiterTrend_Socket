all:
	(cd client; make all)
	(cd server; make all)

clean:
	(cd server; make clean)
	(cd client; make clean)
