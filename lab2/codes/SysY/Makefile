.PHONY: debug,gdb,out,port
# qemu-system-arm -machine virt -nographic -kernel ./fib -S -s
SOURCENAME=./fib.s
EXENAME=./fib

debug:
	arm-linux-gnueabihf-gcc $(SOURCENAME) -o $(EXENAME) -static -g
	qemu-arm -g 1234 $(EXENAME)

# -ex 'set architecture arm'
gdb:
	arm-linux-gnueabihf-gdb -ex 'target remote localhost:1234' $(EXENAME)

out:
	arm-linux-gnueabihf-gcc $(SOURCENAME) -o $(EXENAME) -static -g

port:
	lsof -i:1234

run:
	$(EXENAME)

# 结束进程 : kill -9 ID
