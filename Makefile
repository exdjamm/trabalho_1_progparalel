gpp = /usr/bin/g++
gpp_flags = -O3

all: sequencial paralelo

sequencial: sequencial.cpp
	$(gpp) $(gpp_flags) $<  -o $@.run

paralelo: paralelo.cpp
	$(gpp) $(gpp_flags) --openmp $<  -o $@.run

clean:
	rm -fr sequencial.run paralelo.run