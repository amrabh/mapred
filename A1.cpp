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
#include <map>

using namespace std;

vector<long> nums;
vector<string> words;
vector<map<string,int>> wo;
vector<map<long,int>> nu;
int fd;

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
    vecWord(string w, int f){
        word = w;
        freq = f;
    }
};

vector<vecWord> w;

void fill_words(string filename){
    ifstream file(filename);
    string line;
    
    while(getline(file,line)){
        size_t prev = 0, pos;
        while((pos = line.find_first_of(" .,;:!-)\t\n\v\f\r", prev)) != string::npos){
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
        while((pos = line.find_first_of(" .,;:)!-\t\n\v\f\r", prev)) != string::npos){
            if(pos > prev){
                nums.push_back(stol(line.substr(prev,pos-prev)));
            }
            prev = pos + 1;
        }
        if(prev < line.length())
            nums.push_back(stol(line.substr(prev, string::npos)));
    }
    file.close();
}

void numsort(int low, int high){
    sort(nums.begin()+low, nums.begin()+high);
}

bool vecComp(vecWord a, vecWord b){
    return a.word < b.word;
}
void vecSort(int low, int high){
    sort(w.begin()+low, w.begin()+high, vecComp);
}


void parse(string filename, bool wc){
    if(wc){
        ifstream file(filename);
        string line;
    
        while(getline(file,line)){
            size_t prev = 0, pos;
            while((pos = line.find_first_of(" .,;:!)-\t\n\v\f\r", prev)) != string::npos){
                if(pos > prev){
                    if(line.substr(prev, pos-prev) != "—" && line.substr(prev, pos-prev) != "’"){
                        string s = line.substr(prev,pos-prev);
                        transform(s.begin(), s.end(), s.begin(), ::tolower);
                        w.push_back(vecWord(s,0));
                    }
                }
                prev = pos + 1;
            }
            if(prev < line.length()){
                if(line.substr(prev, string::npos) != "—" && line.substr(prev, string::npos) != "’"){
                    string s = line.substr(prev, string::npos);
                    transform(s.begin(), s.end(), s.begin(), ::tolower);
                    w.push_back(vecWord(s,0));
                }
            }
        }
        file.close();
    }
    else fill_nums(filename);
}

// word merge two vecword arrays
void Wmerge(int low, int mid, int high){
    int size1 = mid - low;
    int size2 = high - mid;

    vector<vecWord> a1;
    vector<vecWord> a2;

    for(int i = 0; i < size1; i++){
        a1.push_back(w[i + low]);
    }
    for(int i = 0; i < size2; i++){
        a2.push_back(w[i + mid]);
    }

    int k = low; 
    int i = 0, j = 0;
  
    // merge 
    while (i < size1 && j < size2) { 
        if (a1[i].word <= a2[j].word) 
            w[k++] = a1[i++]; 
        else
            w[k++] = a2[j++]; 
    } 
  
    // values left on either side 
    while (i < size1) { 
        w[k++] = a1[i++]; 
    } 

    while (j < size2) { 
        w[k++] = a2[j++]; 
    } 
}

// numbers merge two sorted arrays
void merge(int low, int mid, int high){
    int size1 = mid - low;
    int size2 = high - mid;

    long a1[size1];
    long a2[size2];

    for(int i = 0; i < size1; i++){
        a1[i] = nums[i + low];
    }
    for(int i = 0; i < size2; i++){
        a2[i] = nums[i + mid];
    }

    int k = low; 
    int i = 0, j = 0;
  
    // merge 
    while (i < size1 && j < size2) { 
        if (a1[i] <= a2[j]) 
            nums[k++] = a1[i++]; 
        else
            nums[k++] = a2[j++]; 
    } 
  
    // values left on either side 
    while (i < size1) { 
        nums[k++] = a1[i++]; 
    } 

    while (j < size2) { 
        nums[k++] = a2[j++]; 
    } 
}

void threadmapp(int t, bool wc){
    int split;
    if(wc){
        
        split = w.size()/t;
        vector<thread> ts;

        // split threads for mergesort
        for(int i = 0; i < t - 1; i++){
            int low = i*split;
            int high = (i+1)*split;
            
            ts.push_back(thread(vecSort, low, high));
        }
        ts.push_back(thread(vecSort, (t-1)*split, w.size()));

        // wait for threads to finish
        for(auto &t: ts){
            t.join();
        }

        for(int i = 1; i < t - 1; i++){
            int high = (i + 1)*split;
            Wmerge(0, i*split, high);
        }
        Wmerge(0, (t-1)*split, w.size());
    }
    else{
        split = nums.size()/t;
        vector<thread> ts;
        for(int i = 0; i < t - 1; i++){
            int low = i*split;
            int high = (i+1)*split;

            ts.push_back(thread(numsort, low, high));
        }
        ts.push_back(thread(numsort, (t-1)*split, nums.size()));
        
        for(int i = 0; i < ts.size(); i++){
            ts[i].join();
        }
        
        for(int i = 1; i < t - 1; i++){
            int high = (i + 1)*split;
            merge(0, i*split, high);
        }
        merge(0, (t-1)*split, nums.size());
    }
}
void tReduce(int low, int high){
    // merges common words
    for(int i = low; i < high; i++){
        string s = w[i].word;
        int freq = 0;
        int j = i;
        while(i < high && w[i].word == s){
            freq++;
            i++;
        }
        w[j].freq = freq;
        i--;
    }
}

void threadreduce(string outfile, int t, bool wc){
    int split;
    if(wc){
        split = w.size()/t;
        vector<thread> ts;

        // split threads for mergesort
        for(int i = 0; i < t - 1; i++){
            int low = i*split;
            int high = (i+1)*split;
            
            ts.push_back(thread(tReduce, low, high));
        }
        ts.push_back(thread(tReduce, (t-1)*split, w.size()));

        // wait for threads to finish
        for(auto &t: ts){
            t.join();
        }

        ofstream file(outfile);
        string s = w[0].word;
        int f = w[0].freq;
        
        for(int i = 1; i < w.size(); i++){
            if(w[i].word == s){
                f += w[i].freq;
            }
            else{
                file << s << " " << f << endl;
                s = w[i].word;
                f = w[i].freq;
            }
        }
        file << s << " " << f << endl;
    }
    else{
        ofstream file(outfile);
        for(long k: nums){
            file << k << endl;
        }
    }
}

void process_func(char* func, int procs, int size){
    int status = 0;
    int split;
    pid_t childID;
    char total[10];
    sprintf(total, "%d", size);

    if(procs > size){
        split = 1;
        procs = size;
    }
    else split = size/procs;

    // fork processes and send to function
    for(int i  = 0; i < procs - 1; i++){
        if(fork() == 0){
            // turn ints into null terminated strings
            char begin[10];
            sprintf(begin, "%d", (i*split));
            char end[10];
            sprintf(end, "%d", (i+1)*split);

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
        sprintf(end, "%d", size);
            
        char* args[5] = {(char*) func, begin, end, total};
        execvp(func, args);
    }
    else wait(&status);
}

void mapp(string infile, bool wordcount, int maps){
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
        long *data = (long*)mmap(0, nums.size()*sizeof(long), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

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
        long *data = (long*)mmap(0, nums.size()*sizeof(long), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        ofstream file(outfile);
        for(int i = 0; i < nums.size(); i++){
            file << data[i] << endl;
        }
        file.close();
    }
}

void wfill(int begin, int end, int index){
    string line;
    for(int i = begin; i < end; i++){
        line = words[i];
        size_t prev = 0, pos;
        while((pos = line.find_first_of(" .,;:!-)\t\n\v\f\r", prev)) != string::npos){
            if(pos > prev){
                if(line.substr(prev, pos-prev) != "—" && line.substr(prev, pos-prev) != "’"){
                    string s = line.substr(prev,pos-prev);
                    transform(s.begin(), s.end(), s.begin(), ::tolower);
                    
                    // check if map already contains string
                    if(wo[index].count(s) > 0)
                         wo[index].at(s)++;
                    else wo[index].emplace(s, 1);
                }
            }
            prev = pos + 1;
        }
        if(prev < line.length()){
            if(line.substr(prev, string::npos) != "—" && line.substr(prev, string::npos) != "’"){
                string s = line.substr(prev, string::npos);
                transform(s.begin(), s.end(), s.begin(), ::tolower);
                
                if(wo[index].count(s) > 0)
                    wo[index].at(s)++;
                else wo[index].emplace(s, 1);
            }
        }
    }
}
void nfill(int begin, int end, int index){
    string line;
    for(int i = begin; i < end; i++){
        line = words[i];
        size_t prev = 0, pos;
        while((pos = line.find_first_of(" .,;:!-)\t\n\v\f\r", prev)) != string::npos){
            if(pos > prev){
                long k = stol(line.substr(prev,pos-prev));

                // check is map already contains number
                if(nu[index].count(k) > 0)
                    nu[index].at(k)++;
                else nu[index].emplace(k, 1);
            }
            prev = pos + 1;
        }
        
        if(prev < line.length()){
            long k = stol(line.substr(prev, string::npos));
            if(nu[index].count(k) > 0)
                nu[index].at(k)++;
            else nu[index].emplace(k, 1);
        }
    }
}

void ECparse(string filename, bool wc, int t){
    int split;
    ifstream file(filename);
    string line;
    while(getline(file,line)){
        words.push_back(line);
    }
    file.close();

    if(t > words.size()){
        t = words.size();
        split = 1;
    }
    else split = words.size()/t;
    
    vector<thread> ts;
    
    if(wc){
        for(int i = 0; i < t; i++){
            map<string,int> m;
            wo.push_back(m);
        }
        for(int i = 0; i < t - 1; i++){
            ts.push_back(thread(wfill, i*split, (i+1)*split, i));
        }
        ts.push_back(thread(wfill, (t-1)*split, words.size(), t-1));
        for(auto &t: ts){
            t.join();
        }
    }
    else{
        for(int i = 0; i < t; i++){
            map<long,int> m;
            nu.push_back(m);
        }
        
        for(int i = 0; i < t - 1; i++){
            ts.push_back(thread(nfill, i*split, (i+1)*split, i));
        }
        ts.push_back(thread(nfill, (t-1)*split, words.size(), t-1));

        for(auto &t: ts){
            t.join();
        }
    }
}

// merge all maps into first map
void merge(string outfile, bool wc){
    ofstream file(outfile);
    if(wc){
        for(int i = 1; i < wo.size(); i++){
            for(const auto &p: wo[i]){
                if(wo[0].count(p.first) > 0)
                    wo[0].at(p.first) += p.second;
                else wo[0].emplace(p.first, p.second);
            }
        }
        for(const auto &p: wo[0]){
            file << p.first << " " << p.second << endl;
        }
    }

    else{
        for(int i = 1; i < nu.size(); i++){
            for(const auto &p: nu[i]){
                if(nu[0].count(p.first) > 0)
                    nu[0].at(p.first) += p.second;
                else nu[0].emplace(p.first, p.second);
            }
        }
        for(const auto &p: nu[0]){
            for(int i = 0; i < p.second; i++){
                file << p.first << endl;
            }
        }
    }
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
        
        if(proc){
            mapp(infile, wordcount, maps);
            reduce(outfile, wordcount, reds);
            shm_unlink("/shm");
        }
        else{ // threads
            parse(infile, wordcount);
            threadmapp(maps, wordcount);
            threadreduce(outfile, reds, wordcount);
        }
    }
    else{ // extra
        // 6 - threads
        // 8 - input
        // 10 - output
        int threads = stoi(argv[6]);
        string infile = argv[8];
        string outfile = argv[10];
        ECparse(infile, wordcount, threads);
        merge(outfile, wordcount);
    }
}