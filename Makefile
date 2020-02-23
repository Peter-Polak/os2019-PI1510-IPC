# variables
CC=g++
#CFLAGS=-std=c11 -Wall -Werror

PCP1=proc_p1
PCP2=proc_p2
PCT=proc_t
PCD=proc_d
PCSERV2=proc_serv2

#SRCS=$(PCP1).cpp $(PCP2).cpp $(PCT).cpp $(PCD).cpp $(PCSERV2).cpp
#OBJS=$(SRCS:.cpp=.o)
#PROC=$(basename $(SRCS))

PROC=$(PCP1) $(PCP2) $(PCT) $(PCD) $(PCSERV2)
SRC=$(sufix .cpp, $(PROC))
OBJS=$(sufix .o, $(PROC))

MAIN=zadanie

# targets
all: $(PROC) $(MAIN) 
	

$(MAIN): $(OBJS)
	$(CC) $(OBJS) -o $(MAIN)


#create object file for all processes
%.o: %.cpp
	$(CC) -c $^
	
#create executable for all processes
$(PROC): % : %.o
	$(CC) $^ -o $@

.PHONY: clean

# remove compiled files
clean: 
	rm -rf $(MAIN) *.o





# #old
# #proc_p1
# $(PCP1) : $(PCP1).o
# 	$(CC) $^ -o $@

# #proc_p2
# $(PCP2): $(PCP2).o
# 	$(CC) $^ -o $@
	
# #proc_t
# $(PCT): $(PCT).o
# 	$(CC) $^ -o $@

# #proc_d
# $(PCD): $(PCD).o
# 	$(CC) $^ -o $@

# #proc_serv2
# $(PCSERV2): $(PCSERV2).o
# 	$(CC) $^ -o $@
