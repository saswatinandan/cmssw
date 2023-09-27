all: sep19_4_matchAndPlot.o plotClusterShape.o cf_ZSon_ZSoff.o

sep19_4_matchAndPlot.o: sep19_4_matchAndPlot.cc
	g++ `root-config --cflags --glibs` -O3 sep19_4_matchAndPlot.cc -o sep19_4_matchAndPlot.o

plotClusterShape.o: plotClusterShape.cc
	g++ `root-config --cflags --glibs` -O3 plotClusterShape.cc -o plotClusterShape.o

cf_ZSon_ZSoff.o: cf_ZSon_ZSoff.cc
	g++ `root-config --cflags --glibs` -O3 cf_ZSon_ZSoff.cc -o cf_ZSon_ZSoff.o
