

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
#include<bits/stdc++.h> 
using namespace std;





#ifndef MERGE_H
#define MERGE_H

/
// The structure for setting a linked list with a node
struct Node
{
	int data;
	Node* next;
};

// Function to print nodes in a given linked list 
void printList(Node* node)
{
	while (node != NULL)
	{
		printf("%d ", node->data);
		node = node->next;
	}
}

// Comparing two process to see what the order goes betwen the two
Node* sortingMerge(Node* a, Node* b)
{
	Node* result = NULL;

	// using for our base cases
	if (a == NULL)
	{
		return (b);
	}

	else if (b == NULL)
	{
		return (a);
	}
	// comparing two list to help decided where the word is located
	if (a->data <= b->data)
	{
		result = a;
		result->next = sortingMerge(a->next, b);
	}
	else
	{
		result = b;
		result->next = sortingMerge(a, b->next);
	}

	return result;
}


Node* mergeKLists(Node* arr[], int last)
{
	// repeating this process until one list is left
	while (last != 0)
	{
		int i = 0, j = last;

		// (i, j) which is a pair 
		while (i < j)
		{
			// merge List i with List j and storing in the list i
			arr[i] = sortingMerge(arr[i], arr[j]);

			// go to next pair
			i++, j--;

	// Once all done, giving one last update to the list
			if (i >= j)
				last = j;
		}
	}

	return arr[0];
}

// Utility function to create a new node. 
Node *newNode(int data)
{
	struct Node *temp = new Node;
	temp->data = data;
	temp->next = NULL;
	return temp;

	






	// Bubble Sorting the string!       


	// Each process will have it's string of  works be sorted in aplebetical order
	
	
	// for each proccess


	void sortStrings(char arr[][MAX], int n)
	{
		char temp[MAX];

		// Sorting strings using bubble sort 
		for (int j = 0; j<n - 1; j++)
		{
			for (int i = j + 1; i<n; i++)
			{
				if (strcmp(arr[j], arr[i]) > 0)
				{
					strcpy(temp, arr[j]);
					strcpy(arr[j], arr[i]);
					strcpy(arr[i], temp);
				}
			}
		}
	}

	int main()
	{
		
		int n = sizeof(arr) / sizeof(arr[0]);

		sortStrings(arr, n);

		// from here the string should sorted in order
		return 0;
	}
	
}
#endif Merge_H