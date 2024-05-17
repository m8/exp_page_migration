CXX_FLAGS=-O2 -lnuma

all:
	$(CXX) main.cc -o bench $(CXX_FLAGS)

clean:
	$(RM) bench