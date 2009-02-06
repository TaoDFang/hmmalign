all: align

clean: 
	rm *~ *.o

align: main.o bitext.o corpus.o ttable.o config.o model1.o model1moore.o hmm.o hmmAlignTypes.o hmmAlign.o utilfuncs.o transitions.o
	g++ -g -o $@ main.o bitext.o corpus.o ttable.o config.o model1.o model1moore.o hmm.o hmmAlignTypes.o hmmAlign.o utilfuncs.o transitions.o

main.o: main.cc bitext.h corpus.h ttable.h config.h model1.h hmm.h
	g++ -g -c -o main.o main.cc

.cc.o:
	g++ -g -c -o $@ $<
