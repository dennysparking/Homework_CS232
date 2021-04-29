#include "indexPage.h"

//create struct
struct trieNode *indexPage(const char *url)
{
	long sumOfWordCount = 0;

	//declare variables
	char textBuffer[MAXIMUM_INDEXED_CHARACTERS], tempString[MAXIMUM_INDEXED_CHARACTERS]; 
	long inputBufferSize;																 
	int wordStatus;																		

	printf("%s\n", url); //Printing the the WEBPAGE URL

	// struct within heap mem
	struct trieNode *trieRoot = malloc(sizeof(struct trieNode));

	// null validation
	if (trieRoot == NULL)
	{
		//error validation
		fprintf(stderr, "Error Allocation Space In Heap for Trie Root Node\n"); 
		return NULL;
	}

	//Initializing the Trie Root with a Word Count of 0 and making all of its children point to NULL.
	trieRoot->wordCount = 0;
	for (int initTrieNode = 0; initTrieNode < TOTAL_ALPHABETS; initTrieNode++)
	{
		trieRoot->children[initTrieNode] = NULL; //Making Child point to NULL
	}

	//Calling getText to get all the text from the WebPage with specified URL.
	inputBufferSize = getText(url, textBuffer, MAXIMUM_INDEXED_CHARACTERS); //All the text is stored within textBuffer

	if (inputBufferSize < 1)
	{
		fprintf(stderr, "The Webpage is Empty, Unable to Parse Characters\n");
		int destroyTrieCheck = freeTrieMemory(trieRoot);
		if (destroyTrieCheck)
		{	
			//error validation															
			fprintf(stderr, "Error Occured While Destroying the Trie\n"); 
			return NULL;
		}
		return NULL;
	}

	//Making all the Non-Alphabetic Characters a Space and Converting all the Alphabetic Characters to lower-case.
	for (int formatText = 0; formatText < inputBufferSize; formatText++)
	{
		if (!(isalpha(textBuffer[formatText])) && textBuffer[formatText] != '\0')
		{ // Converting a non-alphabetic to a space.
			textBuffer[formatText] = ' ';
		}
		else
		{
			// Coverting to lowercase
			textBuffer[formatText] = tolower(textBuffer[formatText]); 
		}
	}

	//Setting the wordstatus variable value to false
	wordStatus = 0;

	
	for (int bufferCounter = 0, stringCounter = 0; bufferCounter < inputBufferSize; bufferCounter++)
	{
		//Checking if the textbuffer character is in the Alphabet
		if (isalpha(textBuffer[bufferCounter]))
		{
			
			tempString[stringCounter] = textBuffer[bufferCounter];
			stringCounter++; 
			wordStatus = 1;	

			//Special Case: Checking if the word added was the Last Character of the Text Buffer.
			if (bufferCounter == (inputBufferSize - 1))
			{

				tempString[stringCounter] = '\0'; //Forming the word as a String
				printf("\t%s\n", tempString);	  //Printing the word in the required format

				//Sending the word for Trie Addition
				int state = addWordOccurrence(tempString, stringCounter, trieRoot);
				if (state != 0)
				{ // error validation
					fprintf(stderr, "An Error Occurred in the Addition of a Node");
					int destroyTrieCheck = freeTrieMemory(trieRoot); //Calling freeTrieMemory to Traverse through each Node and Free the Allocated Heap Space.
					if (destroyTrieCheck)
					{																  
						fprintf(stderr, "Error Occured While Destroying the Trie\n"); 
						return NULL;
					}
					return NULL;
				}

				//set to 0 and add sum of words plus 1
				stringCounter = 0;
				wordStatus = 0;
				sumOfWordCount += 1;
			}
		}
		else
		{ // space character validation

			//Checking if a Word Formation was taking place
			if (wordStatus == 1)
			{
				tempString[stringCounter] = '\0'; //Forming the word as a string

				printf("\t%s\n", tempString); //Printing the word in the required format

				//Sending the word for Trie Addition
				int state = addWordOccurrence(tempString, stringCounter, trieRoot);
				if (state != 0)
				{ // Checking if an error occured during Trie Addition.
					fprintf(stderr, "An Error Occurred in the Addition of a Node");
					int destroyTrieCheck = freeTrieMemory(trieRoot); //Calling freeTrieMemory to Traverse through each Node and Free the Allocated Heap Space.
					if (destroyTrieCheck)
					{																 
						fprintf(stderr, "Error Occured While Destroying the Trie\n"); 
						return NULL;
					}
					return NULL;
				}

				//set 0 and add to wordcount sum
				stringCounter = 0;
				wordStatus = 0;
				sumOfWordCount += 1;
			}
		}
	}

	trieRoot->totalWords = sumOfWordCount;

	return trieRoot; 
}


int addWordOccurrence(const char *word, const int wordLength, struct trieNode *trieRoot)
{

	if (wordLength == 0)
	{			  //validation
		return 1; 
	}
	else
	{ 

		//Finding the character's mappedIndex 
		int mappedIndex = word[0] - FIRST_LOWERCASE_ALPHABET_ASCII_VALUE;

		//if child exists
		if (trieRoot->children[mappedIndex] == NULL)
		{ //Does not Exist
			//mem allocation
			trieRoot->children[mappedIndex] = malloc(sizeof(struct trieNode));

			if (trieRoot->children[mappedIndex] == NULL)
			{
				fprintf(stderr, "Error allocating memory in heap for Trie Node\n");
				return -2;
			}

			trieRoot->children[mappedIndex]->wordCount = 0;
			for (int initTrieNode = 0; initTrieNode < TOTAL_ALPHABETS; initTrieNode++)
			{
				trieRoot->children[mappedIndex]->children[initTrieNode] = NULL;
			}
		}

		//Incremeting the word count of the child via a recursive call to add the next character of the word.
		trieRoot->children[mappedIndex]->wordCount += addWordOccurrence((word + 1), (wordLength - 1), trieRoot->children[mappedIndex]);
	}

	return 0; 
}


//freeTrieMemory function to allocate/deallocate mem
int freeTrieMemory(struct trieNode *trieRoot)
{
	if (trieRoot == NULL)
	{ // if trieRoot is null
		return 1;
	}
	else
	{
		
		for (int iterateChildren = 0; iterateChildren < TOTAL_ALPHABETS; iterateChildren++)
		{
			if (trieRoot->children[iterateChildren] != NULL)
			{														 // if child exists
				freeTrieMemory(trieRoot->children[iterateChildren]); // Recusively traversing to the Child's children so as to finally reach the leaf node.
			}
		}

		free(trieRoot); //free mem allocated
		return 0;		
	}
}

int findWordOccurence(struct trieNode *trieRoot, const char *word)
{
	if (strlen(word) == 0)
	{
		return trieRoot->wordCount;
	}
	else
	{	
		int mappedIndex = (int)word[0] - FIRST_LOWERCASE_ALPHABET_ASCII_VALUE;
		if (trieRoot->children[mappedIndex] != NULL)
		{
			int termFrequency = findWordOccurence(trieRoot->children[mappedIndex], word + 1);
			return termFrequency;
		}
		else
		{
			return 0;
		}
	}
}

/* You should not need to modify this function */
int getText(const char *srcAddr, char *buffer, const int bufSize)
{
	FILE *pipe;
	int bytesRead;

	snprintf(buffer, bufSize, "curl -s \"%s\" | python getText.py", srcAddr);

	pipe = popen(buffer, "r");
	if (pipe == NULL)
	{
		fprintf(stderr, "ERROR: could not open the pipe for command %s\n",
				buffer);
		return 0;
	}

	bytesRead = fread(buffer, sizeof(char), bufSize - 1, pipe);
	buffer[bytesRead] = '\0';

	pclose(pipe);

	return bytesRead;
}
