#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <thread>
#include <stdlib.h>
#include <cstring>

using namespace std;

vector<int> nums;
vector<string> words;
int fd;
vector<vecWord> w;

typedef struct wordhash{
    char word[80];
    int freq;
} wordhash;

struct vecWord{
    string word;
    int freq;
    vecWord(char* w, int f){
        word = string(w);
        freq = f;
    }
};

void fill_words(string filename){
    ifstream file(filename);
    string line;
    
    while(getline(file,line)){
        size_t prev = 0, pos;
        while((pos = line.find_first_of(" .,;:!-\t\n\v\f\r", prev)) != string::npos){
            if(pos > prev){
                if(line.substr(prev, pos-prev) != "—" && line.substr(prev, pos-prev) != "’"){
                    string s = line.substr(prev,pos-prev);
                    transform(s.begin(), s.end(), s.begin(), ::tolower);
                    words.push_back(s);
                }
            }
            prev = pos + 1;
        }
        if(prev < line.length()){
            if(line.substr(prev, string::npos) != "—" && line.substr(prev, string::npos) != "’"){
                string s = line.substr(prev, string::npos);
                transform(s.begin(), s.end(), s.begin(), ::tolower);
                words.push_back(s);
            }
        }
    }
    file.close();
}

void fill_nums(string filename){
    ifstream file(filename);
    string line;

    while(getline(file,line)){
        size_t prev = 0, pos;
        while((pos = line.find_first_of(" .,;:!-\t\n\v\f\r", prev)) != string::npos){
            if(pos > prev){
                nums.push_back(stoi(line.substr(prev,pos-prev)));
            }
            prev = pos + 1;
        }
        if(prev < line.length())
            nums.push_back(stoi(line.substr(prev, string::npos)));
    }
    file.close();
}

void process_func(char* func, int procs, int size){
    int status = 0;
    pid_t childID;
    char total[10];
    sprintf(total, "%d", size);

    int split = size/procs;

    // fork processes and send to function
    for(int i  = 0; i < procs - 1; i++){
        if(fork() == 0){
            // turn ints into null terminated strings
            char begin[10];
            sprintf(begin, "%d", (i*split));
            char end[10];
            sprintf(end, "%d", (i+1)*split - 1);

            char* args[5] = {func, begin, end, total};
            execvp(func, args);
        }
    }
    // wait for child processes to finish
    while ((childID = wait(&status)) > 0);

    // last process goes to the end of the vector
    if(fork() == 0){
        char begin[10];
        sprintf(begin, "%d", ((procs-1)*split));
        char end[10];
        sprintf(end, "%d", size - 1);
            
        char* args[5] = {(char*) func, begin, end, total};
        execvp(func, args);
    }
    else wait(&status);
}

void map(string infile, bool wordcount, int maps){
    // fill corresponding vectors
    if(wordcount){
        fill_words(infile);
    }
    else{
        fill_nums(infile);
    }

    fd = shm_open("shm", O_CREAT | O_RDWR, 0666);
    if(wordcount){
        // size shared memory
        ftruncate(fd, words.size()*sizeof(wordhash));
        wordhash *data = (wordhash*)mmap(0, words.size()*sizeof(wordhash), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        
        // copy to shared memory
        for(int i = 0; i < words.size(); i++){
            strcpy(data[i].word, words[i].c_str());
            data[i].freq = 1;
        }
        // map
        process_func((char*)"./sortwords", maps, words.size());
    }
    else{
        if(ftruncate(fd, nums.size()*sizeof(int)) == -1){
            perror("ftruncate");
            return;
        }
        int *data = (int*)mmap(0, nums.size()*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        for(int i = 0; i < nums.size(); i++){
            data[i] = nums.at(i);
        }
        process_func((char*)"./sortnums", maps, nums.size());
    }
}

void reduce(string outfile, bool wordcount, int reds){
    if(wordcount){
       process_func((char*)"./reducewords", reds, words.size());
       
       //combine
       wordhash *data = (wordhash*)mmap(0, words.size()*sizeof(wordhash), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

       vector<vecWord> wordfreqs;
       vecWord v = vecWord(data[0].word, data[0].freq);
       wordfreqs.push_back(v);
       ofstream file(outfile);
       int j;
       for(int i = 1; i < words.size(); i++){
           v = vecWord(data[i].word, data[i].freq);
           j = wordfreqs.size()-1;
           if(v.word == wordfreqs[j].word){
               wordfreqs[j].freq += v.freq;
           }
           else{
               file << wordfreqs[j].word << " " << wordfreqs[j].freq << endl;
               wordfreqs.push_back(v);
           }
       }
       if(wordfreqs[j].word != v.word){
           file << v.word << " " << v.freq << endl;
       }
       file.close();
    }
    else{
        int *data = (int*)mmap(0, nums.size()*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        ofstream file(outfile);
        for(int i = 0; i < nums.size(); i++){
            file << data[i] << endl;
        }
        file.close();
    }
}
void ECparse(string filename, bool wc){
    if(wc){
        ifstream file(filename);
        string line;
    
        while(getline(file,line)){
            size_t prev = 0, pos;
            while((pos = line.find_first_of(" .,;:!-\t\n\v\f\r", prev)) != string::npos){
                if(pos > prev){
                    if(line.substr(prev, pos-prev) != "—" && line.substr(prev, pos-prev) != "’"){
                        string s = line.substr(prev,pos-prev);
                        transform(s.begin(), s.end(), s.begin(), ::tolower);
                        
                        if(find(v.begin(), v.end(), x) != v.end()
                    }
                }
                prev = pos + 1;
            }
            if(prev < line.length()){
                if(line.substr(prev, string::npos) != "—" && line.substr(prev, string::npos) != "’"){
                    string s = line.substr(prev, string::npos);
                    transform(s.begin(), s.end(), s.begin(), ::tolower);
                    
                    if(w.count(s) == 1){
                        w[s]++;
                    }
                    else w[s] = 1;
                }
            }
        }
        file.close();
    }
    else fill_nums(filename);
}

int main(int argc, char **argv){
    // 2 - [wordcount, sort]
    // 4 - [procs, threads, extra]
    string task = argv[2];
    string type = argv[4];
    bool wordcount = task == "wordcount";
    bool proc = type == "procs";
    if(type != "extra"){
        // 6 - maps
        // 8 - reduces
        // 10 - input filename
        // 12 - output filename
        int maps = stoi(argv[6]);
        int reds = stoi(argv[8]);
        string infile = argv[10];
        string outfile = argv[12];
        map(infile, wordcount, maps);
        reduce(outfile, wordcount, reds);
        shm_unlink("/shm");
    }
    else{ // extra
        // 6 - threads
        // 8 - input
        // 10 - output
        int threads = stoi(argv[6]);
        string infile = argv[8];
        string outfile = argv[10];
        ECparse(infile, wordcount);
    }
}