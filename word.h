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


//

int main()
{

	char *MyStr = words.;
	string word = MyStr;



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



	// We next then have organize all the words in the string in alphabetical order





// Bubble Sorting the string!       


// Each process will have it's string of  works be sorted in aplebetical order


// for each proccess


	void sortingStrings(char ar[][MAX], int n)
	{
		char temp[MAX];

		// Sorting strings using bubble sort 
		for (int j = 0; j < n - 1; j++)
		{
			for (int i = j + 1; i < n; i++)
			{
				if (strcmp(ar[j], ar[i]) > 0)
				{
					strcpy(temp, ar[j]);
					strcpy(ar[j], ar[i]);
					strcpy(ar[i], temp);
				}
			}
		}
	}

	int main()
	{

		int n = sizeof(ar) / sizeof(ar[0]);

		sortingStrings(ar, n);

		// from here the string should sorted in order
		return 0;
	}

}

#endif WORD_H
	