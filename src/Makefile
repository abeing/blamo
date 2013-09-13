CXX = clang++
SDL = -framework SDL2 -framework SDL2_image

CXXFLAGS = -g -O0 -Wall -c -std=c++11
LDFLAGS = $(SDL)
EXE = blamo

all: $(EXE)

$(EXE): blamo.o
	$(CXX) $(LDFLAGS) $< -o $@

blamo.o: blamo.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm *.o && rm $(EXE)
