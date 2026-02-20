CXX = gcc

INCS = -I.  -I"." 
OBJS = $(SRC:.c=.o)

SRC = ratp.c main.c

LDFLAGS = -lcmocka --coverage -lgcov
OPTIONS = -Wall -fexceptions -fprofile-arcs -ftest-coverage 

all: $(OBJS)
	$(CXX) $(OPTIONS) $(INCS) -o testAll.exe $(OBJS) $(LDFLAGS)
	clear
	
exec : all
	testAll.exe
%.o: %.c
	$(CXX) $(OPTIONS) -c  $< -o $@ $(INCS)
	
clean:
	rm -r $(OBJS) 


mrproper: clean
	rm -f testAll.exe
	rm -f result.xml