INCLUDE := include
SOURCE  := src
BIN     := bin
HEADERS := $(wildcard ${INCLUDE}/*.h)

${BIN}/arpoison: ${SOURCE}/main.cpp ${HEADERS}
	g++ --std=c++17 -pthread -I ${INCLUDE} -o $@ ${SOURCE}/main.cpp

clean: 
	@rm ${BIN}/*