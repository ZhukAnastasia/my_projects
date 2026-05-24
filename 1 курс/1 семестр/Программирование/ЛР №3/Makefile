.PHONY: all clean

APP=lab3avzN3148
CFLAGS=-Wall -Wextra -Werror -g
CCON1=infileall
CCON2=infileonly
CF1=inconsolall
CF2=inconsolonly

all: $(APP)

$(APP): 
	gcc -c $(CCON1).c $(CCON2).c $(CF1).c $(CF2).c
	gcc -o $(APP) $(CFLAGS) $(APP).c $(CCON1).o $(CCON2).o $(CF1).o $(CF2).o

clean:
	rm $(APP) $(CCON1).o $(CCON2).o $(CF1).o $(CF2).o

