CC = g++
CC_FLAGS = -g -Itnt/ -Ijama/ 

MAIN_HEADERS = psr.h psr-sysdyn.h symbol.h environment.h   sampler.h cassandra_grammar.h cassandra_grammar.tab.h   psr-pca.h psr-local.h  hand-pomdp.h
MAIN_SOURCES = main.cpp psr.cpp psr-sysdyn.cpp  symbol.cpp environment.cpp   sampler.cpp cassandra_grammar.tab.c lex.yy.c  psr-pca.cpp psr-local.cpp  hand-pomdp.cpp
MAIN_OBJS = main.o psr.o psr-sysdyn.o symbol.o environment.o   sampler.o  cassandra_grammar.tab.o lex.yy.o psr-pca.o psr-local.o hand-pomdp.o


psr: main 
	$(CC) $(CC_FLAGS) -o psr $(MAIN_OBJS) 

clean:
	rm $(MAIN_OBJS)

main: $(MAIN_HEADERS) $(MAIN_SOURCES)
	$(CC) $(CC_FLAGS) -c $(MAIN_SOURCES)

