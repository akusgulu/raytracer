make:
	g++ -o rtrace main.cpp xml.cpp pugixml-1.12/src/pugixml.cpp -std=c++11 -pthread -O3 -g -Wall