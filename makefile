CXX = g++
ping: ping.cpp
	$(CXX) ping.cpp -o ping

nslookup: nslookup.cpp
	$(CXX) nslookup.cpp -o nslookup

all: ping, nslookup

clean:
	rm -f ping nslookup