// reducee words

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <fstream> 


#ifndef WORD_H
#define WORD_H


using namespace std;


struct vecWord {
		string word;
		int freq;
		
	vecWord(char w, int f) 
	{
	string = w;
	freq = f;
	}
	
		}




int main()
{

	
// We next then have organize all the words in the string in alphabetical order

int low;
int high;
	
sort(num.begin()+low,num.end()+high);
	
	


	// We first begin by counting the specific amount of each word


	map<string, unsigned int> wordsCount;

	// if we encounter the word for the first time
	if (wordsCount.find(word) == wordsCount.end()) {

		// need to assign the word in the string to 1
		wordsCount[word] = 1;

	}


	// if we have already seen this word before
	else {
		// adding 1 to the amount it had before
		wordsCount[word]++;
	}



	






}

#endif WORD_H
	
