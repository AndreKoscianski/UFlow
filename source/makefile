CC = g++-7
C_FLAGS = -Wall

all: uflow.h uflow.cpp calc-heat.cpp layer.cpp
	clear
	@printf "\nYou agree with license GPLv3.\n (press ENTER)"
	@read xis
	$(CC) -O1 -pthread -o ../uflow calc-heat.cpp layer.cpp lodepng.cpp uflow.cpp logfile.cpp 

img: uflow.h image-calc.cpp calc-heat.cpp layer.cpp
	$(CC) -O1 -pthread -o ../img calc-heat.cpp layer.cpp lodepng.cpp image-calc.cpp logfile.cpp 
