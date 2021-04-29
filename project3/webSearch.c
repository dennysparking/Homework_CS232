
// HEADER
#include "webSearch.h"

int main(int argc, char *argv[])
{
    // Characters parsed by scanf
    int parsedCharacters;   
    // Declaring Max. Hops and the indexed Pages as defined within the Algorithm.                            
    int maximumHops, indexedPages = 0, currentHops = 0; 
    int bytesParsed = 0;  
	
	FILE *urlFile;                                      
    
     // a single read line from the URL File.
	char *parseLine;      
     // Stores the URL from the parsed line.                             
	char startURL[MAX_ADDR_LENGTH];     
    // Stores the indexedURL modfified via getLink function.              
    char indexedURL[MAX_ADDR_LENGTH];                   

    // Declaring a Linked List Head Node.
    struct listNode *crawlerListHead; 					
    struct trieNode **indexedTries;   				
    
	int MAX_NO_PAGES;                 					
    int queryState = 0;                   			
	
    // Stores the query to be searched for.
    char *queryInput;    
     // Stores the top ranked URLs for a given Query.                  				
    char *topUrl[MAX_TOP_URLS];            			   
	
    long seedValue;                        				
	
    // temp to store words for query
    char tempString[500];                		 		
    char *queryWord;                       				
	
    // score var for top scores
    double scoreOne, scoreTwo, scoreThree; 				

    // input validation
    if (argc < 3 || argc > 4)
    {
        fprintf(stderr, "Please enter the correct number of arguments.\n");
        return -1;
    }

    // string to long
    seedValue = atol(argv[3]);
    srand(seedValue);

    urlFile = fopen(argv[1], "r");

    if (urlFile == NULL)
    {
        fprintf(stderr, "The url file %s could not be opened\n", argv[1]);
        return -1;
    }

    // string to int for MAX
    MAX_NO_PAGES = atoi(argv[2]);

    // Error validation
    if (MAX_NO_PAGES < 1)
    {
        fprintf(stderr, "The number of url's mentioned is less than 1\n");
        fclose(urlFile);
        return -1;
    }

    // mem alloc for heap
    parseLine = malloc(sizeof(char) * MAX_LINE_LEN);

    // allocation error validation
    if (parseLine == NULL)
    {
        fprintf(stderr, "Error allocating space in the heap for URL File String\n");
        fclose(urlFile);
        return -1;
    }

    // space for linked list
    crawlerListHead = malloc(sizeof(struct listNode));
    crawlerListHead->next = NULL;
    crawlerListHead->tfScore = 0;
    crawlerListHead->tfidfScore = 0;

    // Copying the Word Begin to the head node.
    strncpy(crawlerListHead->addr, "Begin", MAX_ADDR_LENGTH);

    // allocation error validation
    if (crawlerListHead == NULL)
    {
        fprintf(stderr, "Error allocating space in the heap for the Crawler List\n");
        free(parseLine);
        fclose(urlFile);
        return -1;
    }

    // Allocating space for the Max No. of Trie Node roots as specified by the Command Line Arguments.
    indexedTries = malloc(sizeof(struct trieNode *) * MAX_NO_PAGES);

    // allocation error validation
    if (indexedTries == NULL)
    {
        fprintf(stderr, "Error allocating space in the heap for the Trie Root Pointer\n");
        destroyList(crawlerListHead);
        free(parseLine);
        fclose(urlFile);
        return -1;
    }

    //index begins

    printf("Indexing...\n");

    // reading URL line by line
    while (fgets(parseLine, MAX_LINE_LEN, urlFile) != 0 && indexedPages < MAX_NO_PAGES)
    {
        // looks for string and int
        parsedCharacters = sscanf(parseLine, "%s %d", startURL, &maximumHops);

        // validation
        if (parsedCharacters != 2)
        {
            // Freeing the heap allocated memory

            fprintf(stderr, "The file given is not formatted correctly.\n");
            fclose(urlFile);

            for (int ctr = 0; ctr < indexedPages; ctr++)
            {
                freeTrieMemory(indexedTries[ctr]);
            }

            free(indexedTries);
            destroyList(crawlerListHead);
            free(parseLine);

            return -1;
        }

        //counter
        currentHops = 0;

        // Crawl through the line's URL and Index it.
        while (TRUE)
        {
            //URL validation check
            if (contains(crawlerListHead, startURL) == 0)
            {
                //crawl addition and index
                insertBack(crawlerListHead, startURL);
                // NULL if page is empty
                indexedTries[indexedPages] = indexPage(startURL); 
                indexedPages += 1;                               
            }
			
            currentHops += 1;
			
            //validation for hops
            if (currentHops <= maximumHops && indexedPages < MAX_NO_PAGES)
            {
                //Crawling Process
                int validLink = getLink(startURL, indexedURL, MAX_ADDR_LENGTH);

                if (!validLink)
                {
                    break;
                }
                else
                {
                    // Copying the indexed URL to the startUrl and iterating the loop.
                    strncpy(startURL, indexedURL, MAX_ADDR_LENGTH);
                    startURL[MAX_ADDR_LENGTH - 1] = '\0';
                }
            }
            else
            {
                break;
            }
        }
    }

    // Query

    // mem allocation
    queryInput = malloc(sizeof(char) * MAX_QUERY_LEN);

    // Allocating space for the top url strings
    for (int ctr = 0; ctr < MAX_TOP_URLS; ctr += 1)
    {
        topUrl[ctr] = malloc(sizeof(char) * MAX_ADDR_LENGTH);
    }

    // intial values for comparision
    strncpy(topUrl[0], "httpsv", MAX_ADDR_LENGTH);
    strncpy(topUrl[1], "http", MAX_ADDR_LENGTH);
    strncpy(topUrl[2], "https", MAX_ADDR_LENGTH);

    // memory allocation
    if (queryInput == NULL)
    {
        fprintf(stderr, "Error allocating space in the heap for query input\n");
        fclose(urlFile);

        for (int ctr = 0; ctr < indexedPages; ctr++)
        {
            freeTrieMemory(indexedTries[ctr]);
        }

        for (int ctr = 0; ctr < MAX_TOP_URLS; ctr += 1)
        {
            free(topUrl[ctr]);
        }

        destroyList(crawlerListHead);
        free(indexedTries);
        free(parseLine);
        return -1;
    }

    while (TRUE)
    {
        // user input
        printf("\nEnter a web query: ");

        // gets line
        if (fgets(queryInput, MAX_QUERY_LEN, stdin) != NULL)
        {
            //Checking for Errors in Input
            if (strlen(queryInput) == 1 && queryInput[0] == '\n')
            {
                queryState = -2;
            }
            else
            {
                //replace newline characters
                if (queryInput[strlen(queryInput) - 1] == '\n')
                {
                    queryInput[strlen(queryInput) - 1] = '\0';
                }

                for (int i = 0; i < strlen(queryInput); i++)
                {
                    //Checking if the correctly formatted String has been entered.

                    if (!islower(queryInput[i]) && !isspace(queryInput[i]))
                    {
                        queryState = -1;
                        break;
                    }
                    else if (i == strlen(queryInput) - 1 && isspace(queryInput[i]))
                    {
                        queryState = -1;
                        break;
                    }
                    else if (isspace(queryInput[i]) && !isalpha(queryInput[i - 1]) && !isalpha(queryInput[i + 1]))
                    {
                        queryState = -1;
                        break;
                    }
                }
            }

            /*
            Query State of -1 represents an incorrectly formatted String
            Query State of -2 represents no character being entered
            */

            if (queryState == -1)
            {
                printf("Please enter a query containing only lower-case letters.\n");
                queryState = 0;
                continue;
            }
            else if (queryState == -2)
            {
                printf("Exiting the program\n");
                queryState = 0;
                break;
            }
            else
            {          
				// output
                printf("Query is \"%s\".\nIDF scores are\n", queryInput);

    
                bytesParsed = 0;
                queryWord = queryInput;

                // calculate score

                while (sscanf(queryWord, "%s%n", tempString, &bytesParsed) == 1)
                {
                    int loopCtr = 0;
                    double idfScore = 0;
                    int indexedWithTerm = 0;

                    // going through linkedlist to generate score
                    for (struct listNode *iterNode = crawlerListHead->next; iterNode != NULL; iterNode = iterNode->next, loopCtr += 1)
                    {
                        if (indexedTries[loopCtr] != NULL) //validation
                        {
                            iterNode->tfScore = calcTermFrequency(tempString, indexedTries[loopCtr]);

                            // contains the words at least more than 0
                            if (iterNode->tfScore != 0)
                            {
                                indexedWithTerm += 1;
                            }
                        }
                        else
                        {
                            iterNode->tfScore = 0;
                        }
                    }

                    // Calculating the Inverse Document Frequency of the Document / Page
                    idfScore = calcInverseDocFrequency(indexedPages, indexedWithTerm);

                    // Printing the Inverse Document Frequency of the Web Page
                    printf("IDF(%s): %.7f\n", tempString, idfScore);

                    // Calculating the tf-idf score for all the Pages.
                    calcTfidfScore(crawlerListHead->next, idfScore);

                    // Moving the queryWord ptr by the amount of bytes already processed.
                    queryWord += bytesParsed;
                }

                // Output

                printf("Web pages:\n");

                // Setting the Top Scores to 0
                scoreOne = scoreTwo = scoreThree = 0;

                // looping to assign top scores

                for (struct listNode *loopNode = crawlerListHead->next; loopNode != NULL; loopNode = loopNode->next)
                {
                    if (loopNode->tfidfScore > scoreThree)
                    {
                        if (loopNode->tfidfScore > scoreTwo)
                        {
                            if (loopNode->tfidfScore > scoreOne)
                            {
                                scoreThree = scoreTwo;
                                strncpy(topUrl[2], topUrl[1], MAX_ADDR_LENGTH);
                                scoreTwo = scoreOne;
                                strncpy(topUrl[1], topUrl[0], MAX_ADDR_LENGTH);
                                scoreOne = loopNode->tfidfScore;
                                strncpy(topUrl[0], loopNode->addr, MAX_ADDR_LENGTH);
                            }
                            else
                            {
                                scoreThree = scoreTwo;
                                strncpy(topUrl[2], topUrl[1], MAX_ADDR_LENGTH);

                                scoreTwo = loopNode->tfidfScore;
                                strncpy(topUrl[1], loopNode->addr, MAX_ADDR_LENGTH);
                            }
                        }
                        else
                        {
                            scoreThree = loopNode->tfidfScore;
                            strncpy(topUrl[2], loopNode->addr, MAX_ADDR_LENGTH);
                        }
                    }
                }

                //validation

                // if 0 print nothing
                if (scoreOne == 0)
                {
                }
                else if (scoreTwo == 0) 
                {
                    printf("1. %s (score: %.4f)\n", topUrl[0], scoreOne);
                }
                else if (scoreThree == 0) 
                {
                    printf("1. %s (score: %.4f)\n", topUrl[0], scoreOne);
                    printf("2. %s (score: %.4f)\n", topUrl[1], scoreTwo);
                }
                else // If all are non zero values.
                {
                    printf("1. %s (score: %.4f)\n", topUrl[0], scoreOne);
                    printf("2. %s (score: %.4f)\n", topUrl[1], scoreTwo);
                    printf("3. %s (score: %.4f)\n", topUrl[2], scoreThree);
                }

                // assign to 0
                for (struct listNode *iterNode = crawlerListHead->next; iterNode != NULL; iterNode = iterNode->next)
                {
                    iterNode->tfidfScore = 0;
                    iterNode->tfScore = 0;
                }
            }
        }
        else
        {
            // if lines are 0 break
            printf("Exiting the program\n");
            break;
        }
    }

    // close url file
    fclose(urlFile);
	
    // remove mem allocation
    free(parseLine);
    destroyList(crawlerListHead);
    free(queryInput);

    for (int ctr = 0; ctr < indexedPages; ctr++)
    {
        freeTrieMemory(indexedTries[ctr]);
    }

    for (int ctr = 0; ctr < 3; ctr += 1)
    {
        free(topUrl[ctr]);
    }

    free(indexedTries);

    return 0;
}
