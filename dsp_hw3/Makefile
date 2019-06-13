SRIPATH ?= /home/student/05/b05902037/DSP/dsp_hw3/srilm-1.5.10
MACHINE_TYPE ?= i686-m64
LM ?= bigram.lm
LM3 ?= trigram.lm
CXX = g++
CXXFLAGS = -O3 -I$(SRIPATH)/include -w --std=c++11
vpath lib%.a $(SRIPATH)/lib/$(MACHINE_TYPE)

TARGET = mydisambig
SRC = mydisambig.cpp
OBJ = $(SRC:.cpp=.o)
TO = ZhuYin-Big5.map
FROM = Big5-ZhuYin.map
.PHONY: all clean map rum

all: $(TARGET)

$(TARGET): $(OBJ) -loolm -ldstruct -lmisc
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<
run:
	@for i in $(shell seq 1 10) ; do \
		echo "Running $$i.txt" ;\
		./mydisambig $(TO) $(LM) 2 testdata/$$i.txt > result2/$$i.txt;\
		done;
	@for i in $(shell seq 1 10) ; do\
		echo "Running trigrem $$i.txt" ;\
		./mydisambig $(TO) $(LM3) 3 testdata/$$i.txt > result33/$$i.txt\
		done;

map: 
	@echo "Mapping!"
	@python mapping.py $(FROM) $(TO)

clean:
	$(RM) $(OBJ) $(TARGET)
