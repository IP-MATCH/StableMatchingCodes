SYSTEM = x86-64_sles10_4.1
LIBFORMAT = static_pic

GUROBI_DIR = /users/grad/wpette/opt/gurobi752/linux64
CLIB     = -L$(GUROBI_DIR)/lib/ -lgurobi75
INC      = $(GUROBI_DIR)/include/
CPPLIB   = -L$(GUROBI_DIR)/lib/ -lgurobi_c++ $(CLIB)
GUROBI_OPTS = -I$(INC) $(CPPLIB) -lpthread -lm -m64 

CC = g++ 
CFLAGS = -g -Wall -ansi -pedantic -DIL_STD -std=c++11
DEBUG = -pg -g -Wall -ansi -pedantic -DIL_STD 
OBJECTS = main.o Allocation.o time.o AgentIterator.o Graph.o
 
exec : $(OBJECTS) 
	$(CC) $(CFLAGS) -o 3_NOBIN_1STA_YESMERGED_MAXWT $(OBJECTS) $(GUROBI_OPTS)
 
.cpp.o : $<
	$(CC) $(CFLAGS) $(GUROBI_OPTS) -c $< -o $@ 
 
clean : 
	rm -f $(OBJECTS) 3_NOBIN_1STA_YESMERGED_MAXWT
