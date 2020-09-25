# word-reverser build & test automation

APP_NAME=testLogger
LIB_NAME=logger
EXE_NAME=main

build:
	gcc -c ${APP_NAME}.c -o ${APP_NAME}.o
	gcc -c ${LIB_NAME}.c -o ${LIB_NAME}.o
	gcc ${APP_NAME}.o ${LIB_NAME}.o -o ${EXE_NAME}.o
test: build
	@echo Test all logger functions
	./${EXE_NAME}.o 
	
clean:
	rm -rf *.o