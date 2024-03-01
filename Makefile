lavash: lavash.cpp
	$(CC) $^ -o $@

test: lavash test.py
	python3 test.py
