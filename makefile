CXX = g++
ping: ping.cpp
	$(CXX) ping.cpp -o ping

all: ping

clean:
	rm ping