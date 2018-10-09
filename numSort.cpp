#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <vector>
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
	int begin = stoi(argv[1]);
	int end = stoi(argv[2]);
	int size = stoi(argv[3]);

	// accesses shared memory
	int fd = shm_open("shm", O_RDWR, 0666);

	// data will be an array of wordhash structs
	long *data = (long*)mmap(0, size * sizeof(long), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	// convert c string to c++ string
	vector<long> s;
	for (int i = begin; i < end; i++) {
		s.push_back(data[i]);
	}
	sort(s.begin(), s.end());

	// get c string from c++ string
	for (int i = 0; i < s.size(); i++) {
		data[i + begin] = s[i];
	}
	return 0;
}