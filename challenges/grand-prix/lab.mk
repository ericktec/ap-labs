APP_NAME=grand-prix

build:
	go build -o ${APP_NAME} ${APP_NAME}.go

run: build
	@echo Test 1
	./${APP_NAME} -players=20 -laps=3 

clean: 
	rm -rf ${APP_NAME}
