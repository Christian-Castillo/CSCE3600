/*
Author: Chalet Shelton
Date: 9.18.17
Professor: Mark Thompson
Description: Minor Assignment 1 – File I/O, Memory Allocation, Parsing
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LENGTH_STRING  81

struct line
{
	char containing[MAX_LENGTH_STRING];
	int lineNum; 
	//int data;
	struct line *next;
};

struct list
{
	struct line *head;
	struct line *tail;
	char line[MAX_LENGTH_STRING];
}list;

void add (struct list *par1, struct line *par2);
void print(struct list *par1);
void freeList(struct list *par1);
int main(int argc, char *argv[])
{
	struct line *longestLine = (struct line*)malloc(sizeof(struct line)); //create a struct longestline of struct line
	struct list temp2;
	temp2.head = temp2.tail = NULL;
	int fileLength = 0;
	int countLine = 0;
	int	occurrences = 0;
	int occurUpdate = 0;
	int longestLineCount =0;
	char arr[MAX_LENGTH_STRING];
	char *tokenlist;
	char token;
	FILE *fp;
	char arr2[MAX_LENGTH_STRING];
	
	if(argc != 3)
	{
		printf("*****ERROR: Not enough parameters, please try again");
	}
	else
	{
		//open file
		fp = fopen(argv[1], "r");
		if(fp == NULL)
		{
			printf("*****ERROR: file not found, please try again.");
			exit(EXIT_FAILURE);
		}
		
		int stringLength;
		
		//read line-by-line
		while(fgets(arr, MAX_LENGTH_STRING, fp) != NULL)
		{
			countLine++;
			occurUpdate = occurrences;
			
			stringLength = strlen(arr); //get the string length of the line
			fileLength = fileLength + stringLength; //get the # of characters in file
			
			if(stringLength > longestLineCount)
			{
				longestLineCount = stringLength;
				strncpy(longestLine->containing, arr, sizeof(arr)); //copy that line into the longest line array
				longestLine->lineNum = countLine; //copy the current line number into new struct line number
			}

			strcpy(arr2, arr);
			
			tokenlist = strtok(arr," \n"); //break the current string into substrings
			
			while(tokenlist != NULL)
			{
				if((strncmp(argv[2],tokenlist, sizeof(argv[2])) == 0) || (strstr(tokenlist,argv[2])!= NULL))
				{
					occurrences++;
				}
				tokenlist = strtok(NULL," \n");
			}
			
			if(occurUpdate != occurrences)
			{
				struct line *temp = (struct line*)malloc(sizeof(struct line)); // create a new object
				strcpy(temp->containing, arr2);
				temp->lineNum = countLine;
				add(&temp2, temp);
			}
			
		}
		printf("Total Number Occurrences of '%s' in File: %d\n", argv[2], occurrences);
		printf("-------------------------------------------------------------\n");
		print(&temp2);
		printf("Total Lines in File		 =	%d\n", countLine);
		printf("Character's Longest Line 	 =	%d\n", strlen(longestLine->containing)); 
		printf("Longest line			 =	%s\n", longestLine->containing);
		printf("-------------------------------------------------------------\n");
	}
	
	
	fclose(fp);
	free(longestLine);
	freeList(&temp2);
	
	return 0;
}
void add (struct list *par1, struct line *par2)
{	
	if(par1->head == NULL)
	{
		par1->head = par1->tail = par2;
		par2->next = NULL;
	}
	else
	{
		par1->tail->next = par2;
		par1->tail = par2;
	}
}
void print(struct list *par1)
{
	struct line *iter = par1->head;
	
	while(iter != NULL)
	{
		printf("Line %d: %s\n", iter->lineNum, iter->containing);
		iter = iter->next;
	}
	return;
}
void freeList(struct list *par1)
{
	struct line *iter = par1->head;
	struct line *iter2;
	
	while(iter != NULL)
	{
		iter2 = iter->next;
		free(iter);
		iter = iter2;
	}
	return;
}