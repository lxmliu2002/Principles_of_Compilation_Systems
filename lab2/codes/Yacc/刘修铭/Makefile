.PHONY: debug,gdb,out,port
# qemu-system-arm -machine virt -nographic -kernel ./fib -S -s
ARMYACC=./ARM.y
ARMC=./ARM.c
ARMARM=./ARM.s
ARM=./ARM

EXPRYACC=./expr.y
EXPRC=./expr.c
EXPR=./expr

SUFFIXYACC=./expr_suffix.y
SUFFIXC=./expr_suffix.c
SUFFIX=./expr_suffix

armc:
	yacc -o $(ARMC) $(ARMYACC)

armrun:
	gcc $(ARMC) -o $(ARM)
	$(ARM)

arm:
	arm-linux-gnueabihf-gcc $(ARMARM) -o $(ARM) -static -g
	$(ARM)


exprc:
	yacc -o $(EXPRC) $(EXPR)

exprrun:
	gcc $(EXPRC) -o $(EXPR)
	$(EXPR)

suffixc:
	yacc -o $(SUFFIXC) $(SUFFIXYACC)

suffixrun:
	gcc $(SUFFIXC) -o $(SUFFIX)
	$(SUFFIX)