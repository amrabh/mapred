//
//  main.cpp
//  mapred
//
//  Created by Amruta Abhyankar on 9/30/18.
//  Copyright © 2018 Amruta Abhyankar. All rights reserved.
//
#include <math.h>
#include <iostream>
#include <thread>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <cstdlib>
#include <pthread.h>
#include <thread>
#include <condition_variable> // used as signal mechanism between threads
#include <mutex>
#include <map>
#include <regex>

std::mutex mu;
using namespace std;
vector<string> sc;
vector<string> result;
vector<string> counts;
vector<string> finals;
vector<string> vins;
vector<string> vtemp;
vector<string> temp;
vector<string> temp1;
vector<int> intsort;
vector<string> intresult;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
//function to print string vector
void print(std::vector<string> const &input){
    for(int i = 0 ; i < input.size(); i++){
        if(input.at(i) != " "){
            std::cout<< input.at(i)<< ' '<<'\n';
        }
    }
}


// worker function to count the frequency of words in the splitted file
void *exec(void *dummyPtr){
    //std::cout << "thread " << n << "\n";
    string words;
    int word_Count = 1;
    words = temp[0];
    for(int i=0; i < temp.size(); i++){
        //mtx.lock();
        //cout<<"Word 2 is "<< v1[i+1] <<'\n';
        if(words != temp[i] || words == temp[temp.size()]){
            //cout<<words<<" "<< word_Count<<endl;
            std::string s = std::to_string(word_Count);
            string ss = words +" "+ s;
            sc.push_back(ss);
            pthread_cond_signal( &cond );
            words = temp[i];
            word_Count = 0;
        }
        word_Count++;
        
    }
    return 0;
}
// Map function for word counting
void wordcountMapThreads(string filename, int nthreads){
    string counts;
    string line;
    //vector<string> temp;
    vector<string> tokens;
    //cout<<"I am in the word count threads function \n" << endl;
    //cout<<"The text file name is : "<< filename <<endl;
    ifstream myfile(filename);
    if (myfile.is_open()){
        while(getline (myfile,line)){
            bool my_predicate(char c);
            string s;
            //s.erase(remove_if(s.begin(), s.end(), [](char c) {return ispunct(c); } ), s.end());
            std::remove_copy_if(line.begin(), line.end(),
                                std::back_inserter(s), //Store output
                                std::ptr_fun<int, int>(&std::ispunct)
                                );
            
            /**char chars[] = "—";
             
             for (unsigned int i = 0; i < strlen(chars); ++i)
             {
             // you need include <algorithm> to use general algorithms like std::remove()
             s.erase (std::remove(s.begin(), s.end(), chars[i]), s.end());
             }
             //cout << s << endl;**/
            istringstream iss(s);
            
            copy(istream_iterator<string>(iss),
                 istream_iterator<string>(),
                 back_inserter(tokens));
        }
        std::vector<std::string> v1;
        v1.reserve(tokens.size());
        std::transform(
                       tokens.begin(),
                       tokens.end(),
                       std::back_inserter(v1),
                       [](const std::string& in) {
                           std::string out;
                           out.reserve(in.size());
                           std::transform(in.begin(), in.end(), std::back_inserter(out),::tolower);
                           return out;
                       }
                       );
        float v_size = v1.size();
        int parts = int(v_size/nthreads + 1);
        
        for ( int j = 0; j<nthreads; j++){
            for (int i=0; i<parts+1; i++){
                temp.push_back(v1[i]);
            }
            if(v1.size()>parts){
                v1.erase(v1.begin(),v1.begin()+parts+1);
            }
            else{
                //cout<<"Size of v1 is :"<<v1.size();
                //cout<<"I am in the else loop"<<endl;
                //cout<<"------------------------------------"<<endl;
                temp.clear();
                for(unsigned i = 0;i<v1.size()+1;i++){
                    temp.push_back(v1[i]);
                }
            }
            //cout<<"v1 size"<<v1.size()<<endl;
            //cout<<"temp size"<<temp.size()<<endl;
            //cout<<"**********************************************************"<<endl;
            /**std::thread myThreads[nthreads];
            myThreads[j] = std::thread(exec, j, temp);
            myThreads[j].join()**/
            pthread_t thread_id[nthreads];
            pthread_create( &thread_id[j], NULL, exec, (void *)&j);
            pthread_join( thread_id[j], NULL);
            temp.clear();
            //print(temp);
        }
    }
    sort(sc.begin(),sc.end());
    //print(sc);
}

void *addtokens(void *dummyPtr){
    string words1;
    //sort(s1.begin(), s1.end());
    int word_Count = 1;
    words1 = temp1[0];
    //cout<<"Word is "<< words1 <<'\n';
    for(int i=1; i < temp1.size()+1; ++i){
        //cout<<"Word 2 is "<< v1[1] <<'\n';
        if(words1 != temp1[i]){
            //cout<<" "<<'\n';
            string st = words1.substr(0, words1.size()-1);
            //cout<<st<<" "<< word_Count<<endl;
            std::string wc = std::to_string(word_Count);
            result.push_back(st+"\t "+wc);
            word_Count = 0;
            words1 = temp1[i];
        }
        word_Count++;
    }
    return 0;
}



void wordcountReduceThreads(vector<string> s, int threads){
    float v_size = s.size();
    int parts = int((v_size/threads) + 1);
    //cout<<parts<<endl;
    for ( int j = 0; j<threads; j++){
        for (int i=0; i<parts; i++){
            temp1.push_back(s[i]);
        }
        if(s.size()>parts){
            s.erase(s.begin(),s.begin()+(parts));
        }
        else{
            //print(s);
            //cout<<"Size of v1 is :"<<s.size();
            //cout<<"I am in the else loop"<<endl;
            temp1.clear();
            for( unsigned i = 0;i<s.size()+1;i++){
                temp1.push_back(s[i]);
            }
        }
        
        //cout<<"v1 size"<<s.size()<<endl;
        //cout<<"temp size"<<temp.size()<<endl;
        /**std::thread myThreads[threads];
        myThreads[j] = std::thread(addtokens,temp);
        myThreads[j].join();**/
        pthread_t thread_id[threads];
        pthread_create( &thread_id[j], NULL, addtokens, (void *)&j);
        pthread_join( thread_id[j], NULL);
        temp1.clear();
        //print(temp);
        
    }
    
    
    finals.push_back(result[0]);
    for(int i=1; i<result.size()+1; ++i){
        if(result[i-1].substr(0,result[i-1].size()-3) == result[i].substr(0,result[i].size()-3)){
            string words = result[i-1].substr(0,result[i-1].size()-3);
            string s1 = (result[i-1].substr(result[i-1].size()-3,result[i-1].size()));
            string s2 = (result[i].substr(result[i].size()-3,result[i].size()));
            stringstream geek(s1);
            stringstream geek2(s2);
            int x1 = 0;
            int x2 = 0;
            int sum = 0;
            geek >> x1;
            geek2 >> x2;
            sum = x1+x2;
            //cout<<words<<" "<<sum<<endl;
            std::string wcc = std::to_string(sum);
            finals.pop_back();
            finals.push_back(words+"\t"+ wcc);
        }
        else{
            finals.push_back(result[i]);
        }
    }
    for(int i=0; i<finals.size(); i++){
        if(finals[i].substr(0,finals[i].size()-1) == finals[i+1].substr(0,finals[i+1].size()-1)){
            //cout<<"Doubles"<<endl;
            finals.erase(finals.begin()+i,finals.begin()+i+1);
        }
    }
    cout<<"**************************************"<<endl;
    sort(finals.begin(),finals.end());
    print(finals);
    std::ofstream output_file("./word_output.txt");
    std::ostream_iterator<std::string> output_iterator(output_file, "\n");
    std::copy(finals.begin(), finals.end(), output_iterator);
    
}


//********************Sorting*************************

void print2(std::vector<string> const &input){
    for(int i = 0 ; i < input.size(); i++){
        if(input.at(i) != " "){
            std::cout<< input.at(i)<<endl;
        }
    }
}
void printInt(std::vector<int> const &input){
    for(int i = 0 ; i < input.size(); i++){
            std::cout<< input.at(i)<<endl;
        }
}

void *sorts(void *dummyPtr){
    //int i;
    //printf("Thread number %ld\n", long(pthread_self()));
   
    std::ofstream output_file("./sort_output.txt");
    for (const auto &e : intsort) output_file << e << "\n";
    return 0;
}
void *isorts(void *dummyPtr){
    long n;
    n = (long)dummyPtr;
    //std::cout << "thread " << n << "\n";
    //cout<<"vins size"<<vins.size()<<endl;
    //cout<<"vtemp size"<<vtemp.size()<<endl;
    for(int i1=0; i1<vtemp.size()-1; ++i1){
        intsort.push_back(stoi(vtemp[i1]));
    }
    sort(intsort.begin(),intsort.end());
    return 0;
}

void integerSortMapThreads(string filename, int nthreads){
    
    //cout<<"I am inside the integer Sort"<<endl;
    string ins;
    ifstream ip("sort_input.csv");
    if (!ip.is_open()) std::cout<<"ERROR: File Open"<<'\n';
        while(ip.good()){
            getline(ip,ins);
            vins.push_back(ins);
        }
    //cout<<"Map vins"<<endl;
    //print2(vins);
    float v_size = vins.size();
    int parts = int(v_size/nthreads + 1);
    for ( int j = 0; j<nthreads; j++){
        for (int i=0; i<parts; i++){
            vtemp.push_back(vins[i]);
        }
        if(vins.size()>parts){
            vins.erase(vins.begin(),vins.begin()+(parts - 1));
        }
        else{
            //print(vins);
            //cout<<"Size of vins is :"<<vins.size();
            //cout<<"I am in the else loop"<<endl;
            vtemp.clear();
            for(unsigned i = 0;i<vins.size();i++){
                vtemp.push_back(vins[i]);
            }

        }
        
        cout<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"<<endl;
        //print(v1);
        //cout<<"vins size"<<vins.size()<<endl;
        //cout<<"vtemp size"<<vtemp.size()<<endl;
        pthread_t thread_id[nthreads];
        pthread_create( &thread_id[j], NULL, isorts, (void *)&j );
        pthread_join( thread_id[j], NULL);
        vtemp.clear();
        //print(vtemp);
}


}
void integerSortReduceThreads (int nthreads){
    cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
    printInt(intsort);
    cout<<"Size of the sorted list is :"<<intsort.size()<<endl;
    cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
    int i;
    pthread_t thread_id[nthreads];
    
    for(i=0; i < nthreads; i++)
    {
        pthread_create( &thread_id[i], NULL, sorts, NULL );
        pthread_join( thread_id[i], NULL);
    }
}



int main(int argc, const char * argv[]) {
    string textfilename = "word_input.txt";
    wordcountMapThreads(textfilename, 10);
    wordcountReduceThreads(sc, 10);
    integerSortMapThreads("sort_input.csv", 10);
    integerSortReduceThreads(10);
    return 0;
}
