# Makefile FOR CONSTANT72
# Making for host
# > make ARCH=host
SOURCES:=Checksum.cpp file_client.cpp file_server.cpp lib.cpp Link.cpp Transport.cpp
CXXFLAGS=-I. -lssl -lcrypto -std=c++0x
EXENAME=threadsLAB

# Making for host
CXX=g++
BUILD_DIR=build/host/
BIN_DIR=bin/host/

# > make ARCH= target
ifeq (${ARCH}, target)
CXX=arm-devkit-g++
BUILD_DIR=build/target/
BIN_DIR=bin/target/
endif

ROOTDIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
OBJECTS:=${addprefix ${BUILD_DIR},${SOURCES:.cpp=.o}}
EXE=${BIN_DIR}${EXENAME}
DEPS:=${OBJECTS:.o=.d}

#Fjern .SILENT hvis udskrifter ønskes m fejlfinding evt tilføje
#-pedantic i CXXFLAGS, eller -Wall -Wextra

$(EXE): $(OBJECTS) $(DEPS)
	mkdir -p ${BIN_DIR}
	$(CXX) $(CXXFLAGS) -o $@  $(OBJECTS)

# Rule that describes how a .d ( dependency ) file is created from a .cpp file
# Similar to the assigment that you just completed %.cpp -> %.o
${BUILD_DIR}%.o: %.cpp
	mkdir -p ${BUILD_DIR}
	$(CXX) -c $(CXXFLAGS) -o $@ $<

${BUILD_DIR}%.d: %.cpp
	mkdir -p ${BUILD_DIR}
	$(CXX) -MT $@ -MM $(CXXFLAGS) $^ > $@

.PHONY: clean
clean:
	rm -rf $(EXE) $(DEPS) $(OBJECTS)

.PHONY: deepClean
deepClean:
	rm -rf $(EXE) $(DEPS) $(OBJECTS)
	rm -R ${ROOTDIR}/bin ${ROOTDIR}/build
#find . -type f -not -name '*.c*' -not -name '*.h*' -not -name 'Makefile' -delete

help:
	@echo ARCH=[host; target], clean, $@.

run: $(EXE)
	./${EXE}

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
