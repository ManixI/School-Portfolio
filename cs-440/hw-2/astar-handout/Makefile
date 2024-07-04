PYTHON=python3

stages:
	echo verify clean run

verify:
	for f in *.py; do echo $$f; done

run:
	$(PYTHON)  wsuvpyunitrunner.py 

clean:
	rm -rf *~ 
