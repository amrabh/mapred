#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <cstring>
#include <vector>

using namespace std;

typedef struct wordhash {
	char word[80];
	int freq;
} wordhash;

struct vecWord {
	string word;
	int freq;
	vecWord(char* w, int f) {
		word = w;
		freq = f;
	}
};

int main(int argc, char **argv) {
	int begin = stoi(argv[1]);
	int end = stoi(argv[2]);
	int size = stoi(argv[3]);

	// accesses shared memory
	int fd = shm_open("shm", O_RDWR, 0666);

	// data will be an array of wordhash structs
	wordhash *data = (wordhash*)mmap(0, size * sizeof(wordhash), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	// convert c string to c++ string
	vector<vecWord> w;
	for (int i = begin; i < end; i++) {
		vecWord a = vecWord(data[i].word, 0);
		w.push_back(a);
	}
	
    for(int i = 0; i < w.size(); i++){
        string s = w[i].word;
        int freq = 0;
        int j = i;
        while(i < w.size() && w[i].word == s){
            freq++;
            i++;
        }
        w[j].freq = freq;
        i--;
    }

	// get c string from c++ string
	for (int i = 0; i < w.size(); i++) {
		strcpy(data[i + begin].word, w.at(i).word.c_str());
        data[i+begin].freq = w[i].freq;
	}

	return 0;
}