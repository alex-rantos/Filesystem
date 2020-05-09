#Compiler settings

CC = g++
CFLAGS = -c -Wall -g
SRCS = mydiz.cc helper_funcs.cc archive_structure.cc InodeList.cc Stack.cc
OBJS = $(SRCS:.cc=.o)
EXE = mydiz
LIBS = -pthread

all : $(SRCS) $(EXE)
	@echo $(EXE) "HAS BEEN COMPILED"

$(EXE): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LIBS)

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

clean :
	@echo "Removing files"
	rm $(EXE) *.o

rebuild :
	make clean
	make

send_final :
	tar -cvf AlexandrosRantos-Prj4.tar ./*
	chmod 755 AlexandrosRantos-Prj4.tar
	scp AlexandrosRantos-Prj4.tar sdi1300147@linux01.di.uoa.gr:/home/users/k22/project4/sdi1300147/
	
send_final2 :
	tar -cvf MariosGeladas-Prj4.tar ./*
	chmod 755 MariosGeladas-Prj4.tar
	scp MariosGeladas-Prj4.tar sdi1300027@linux01.di.uoa.gr:/home/users/k22/project4/sdi1300027