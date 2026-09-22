PYTHON ?= python3
CXX ?= c++

.PHONY: verify cpp
verify:
	$(PYTHON) verify/u1.py
	$(PYTHON) verify/u2.py
	$(PYTHON) verify/u3.py
	$(PYTHON) verify/family_f.py
	$(PYTHON) verify/core_overlap.py

cpp:
	mkdir -p build
	$(CXX) -std=c++17 -O3 search/exchange_dfs.cpp -o build/exchange_dfs
