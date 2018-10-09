#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
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
using namespace std;







// The structure for setting a linked list with a node
struct Node
{
	int item;
	Node* next;
};

// Function to print nodes in a given linked list 
void printList(Node* node)
{
	while (node != NULL)
	{
		printf("%d ", node->item);
		node = node->next;
	}
}

// Comparing two process to see what the order goes betwen the two
Node* sortingMerge(Node* a, Node* b)
{
	Node* listResult = NULL;

	// using for our base cases
	if (b == NULL)
	{
		return (a);
	}

	else if (a == NULL)
	{
		return (b);
	}
	// comparing two list to help decided where the word is located


	if (a->item <= b->item)
	{
		listResult = a;
		listResult->next = sortingMerge(a->next, b);
	}


	else
	{
		listResult = b;
		listResult->next = sortingMerge(a, b->next);
	}

	return listResult;
}


Node* mergeTheLists(Node* arr[], int lastone)
{
	// repeating this process until one list is left
	while (lastone != 0)
	{
		int j = lastone;
		int i = 0;

		// Comparing the pair 
		while (i < j)
		{
			// merge List i with List j and storing in the list i
			arr[i] = sortingMerge(arr[i], arr[j]);

			// go to next pair
			j--;
			i++;

			// Once all done, giving one last update to the list
			if (i >= j)
				lastone = j;
		}
	}

	return arr[0];
}

// Utility function to create a new node. 
Node *newNode(int item)
{
	struct Node *temp = new Node;

	temp->item = item;

	temp->next = NULL;

	return temp;
}


