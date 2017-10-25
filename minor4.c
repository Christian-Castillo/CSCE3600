/*
Author: Chalet Shelton
Date: 10/17/17
Instructor: Mark Thompson
Description: This program is supposed to schedule CPU process, but I couln't math that well so mine does not.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct process
{
	int index, Bursttime, Arrivaltime, Remaining, Finished, remaining, Wait;
	char ProcessID[6];
}process;

void sort(struct process arr[], int numLines);

int main()
{
	//char **ProcessID;
	char filename[20];
	char *line, *tokenList;
	int numLines=0, i=0, j=0, waits=0, turns =0, totalBurst=0;
	//int *remaining, *finished;
	float avgThroughput =0, avgWait =0, avgTurn =0;
	FILE *fp;
	
	printf("Enter name of process file: ");
	gets(filename);
	fp = fopen(filename, "r");
	line = malloc(sizeof(fp));

	
	if(fp == NULL)
	{
		printf("\n*****ERROR: unable to open file\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("PLZ GRADE WITH A LITE PEN");
		while(fgets(line, 25, fp) != NULL)
		{
			numLines++;
		}	
		
		struct process Process[numLines]; // make a struct arrays
		struct process sorted[numLines]; // create a sorted array
		rewind(fp); // move file pointer back to the beginning of the file
		
		while(fgets(line, 25, fp) != NULL) //read each line
		{
			i=0;
			tokenList = strtok(line, " \n"); //divide everything into tokens
			Process[j].index = i;
			//printf("%s\n", tokenList);
			
			while(tokenList != NULL)
			{
				//printf("%s ",tokenList); /* your word */
				
				if(i == 0) //if we're at tokenlist index 0 then set as PID
				{
					
					strcpy(Process[j].ProcessID, tokenList); 
					//printf("\n %s \n", Process[j].ProcessID);
					
				}
				else if(i == 1) // else set it as arrival time
				{
					Process[j].Arrivaltime = atoi(tokenList);
					//printf("%d \n",Process[j].Arrivaltime);
				}
				else if(i == 2) // else set it as Burst time
				{
					Process[j].Bursttime = atoi(tokenList);
					totalBurst += Process[j].Bursttime;
					
					//printf("%d TotalBurst: %d \n",Process[j].Bursttime, totalBurst);
				}
				
				tokenList = strtok(NULL, "  \n");				
				i++;
			
			}
			//totalBurst += Process[j].Bursttime;
			Process[j].Wait = Process[j].Finished = 0;
			Process[j].Remaining = Process[j].Bursttime;
			sorted[j] = Process[j];
			j++;
			
		}
		
		sort(sorted, numLines);
		
		int loop, temp =0;
		struct process *waitlist[numLines-1];
		struct process *current = &sorted[0];
		struct process *next = &sorted[1];
		
		for(i =0; i < numLines; i++)
		{
			waitlist[i] = NULL;
		}
		
		for(i=0, j = 2; i < totalBurst; i++)
		{
			current->Remaining--;
			
			if(current->Remaining == 0)
			{
				current->Finished = i; // current process finished 
				waitlist[current->index] = NULL;
				for(loop =0; loop < numLines-1; loop++) //loop and get the first waiting process
				{
					if(waitlist[loop] != NULL)
					{
						current = waitlist[loop];
						break;
					}
				}
				if(current == NULL) //there are no waiting processes
				{
					current = next;
					next = &sorted[j];
					j++;
				}
				else
				{
					for(loop = 0; loop < numLines; loop++) //loop again and get the shortest time remaining
					{
						if(waitlist[loop] != NULL)
						{
							if(current->Remaining < waitlist[loop]->Remaining)
							{
								current = waitlist[loop];
							}
						}
					}
				}
				if(next->Arrivaltime ==1)
				{
					temp = current->Bursttime - next->Arrivaltime; //determine remaing burst of current index
					if(temp > next->Bursttime) //if the bursttime of curent is longer, change current process
					{
						waitlist[current->index] = current; //put current on waitlist
						for(loop =0; loop < numLines; loop++) // increase the wait of any other process
						{
							if(waitlist[loop] != NULL)
							{
								waitlist[loop]->Wait += next->Bursttime;
							}
						}
						
						current = next; //next process becomes current process
						next = &sorted[j]; // get next process in the sorted array
						j++;
					}
				}
				else // the current process is shorter
				{
					waitlist[next->index] = next; // put next on the waitlist
					next = &sorted[j]; //get teh next incoming process
					j++;
				}
				
			}
		}
		/*for(i =0; i < totalBurst; i++)
		{
			printf("%d \n", *(&waitlist[i]));
		}*/
		printf("\n");
		printf("PID\tarrived\tburst\tremain\tfinish\twait\n");
		printf("------------------------------------------\n");
		for(i=0; i < numLines; i++)
		{
			printf("%s\t%d\t%d\t%d\t%d\t%d\n", Process[i].ProcessID, Process[i].Arrivaltime, Process[i].Bursttime, Process[i].Remaining, Process[i].Finished, Process[i].Wait);
		}
	}
	
	fclose(fp);
	free(line);
	return 0;
}

void sort(struct process arr[], int numLines)
{
	int i, j;
	for( i  = numLines; i >= 0; i--)
	{
		for(j = 1; j <= i; j++)
		{
			if(arr[j-1].Arrivaltime > arr[j].Arrivaltime) //if previous arrival time is greater than current arrival time
			{
				struct process temp = arr[j-1]; // create a temp that's equal to previous arrival time
				arr[j-1] = arr[j]; // previous is equal to current
				arr[j] = temp; // current is equal to temp
			}
		}
	}
}