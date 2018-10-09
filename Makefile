all: mapred sortnums sortwords reducewords
mapred: A1.cpp
	g++ A1.cpp -o mapred -lrt -pthread -std=c++11
sortnums: numSort.cpp
	g++ numSort.cpp -o sortnums -lrt -std=c++11
sortwords: wordSort.cpp
	g++ wordSort.cpp -o sortwords -lrt -std=c++11
reducewords: wordRed.cpp
	g++ wordRed.cpp -o reducewords -lrt -std=c++11
