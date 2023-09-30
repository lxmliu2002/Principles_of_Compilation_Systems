.PHYON: suffix,test,asm,arm

suffix:
	yacc suffix.y
	gcc y.tab.c -o suffix
	./suffix < input.txt > output.txt

test:
	yacc main2.y
	gcc y.tab.c -o main2
	./main2 < input.txt > output.txt

asm:
	yacc yacc_asm.y
	gcc y.tab.c -o yacc_asm
	./yacc_asm < input.txt > output.s

arm:
	arm-linux-gnueabihf-gcc output.s -o output -static
	./output