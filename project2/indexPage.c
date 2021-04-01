/* File: indexPage.c */
/* Author: Dennis Ratliff + Britton Wolfe */
/* Date: September 3rd, 2010 */

/* This program indexes a web page, printing out the counts of words on that page */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define TOTAL_ALPHABETS (26)
#define CONVERT(c) ((int)c - (int)'a')
 

/* TODO: structure definitions */
struct trieNode{
  struct trieNode *children[26];
  int isWord;
  int counter;
};

struct trieNode *indexPage(const char* url);

int addWordOccurrence(const char* word, const int wordLength , struct trieNode *pNode);
		       /* TODO: other parameters you need */
struct trieNode *makeNode();
int freeTrieMemory(struct trieNode *pNode);
int getText(const char* srcAddr, char* buffer, const int bufSize);
struct trieNode *makeNode();
int printTrieContents(struct trieNode* trieRoot, char* characterBuffer, int level);

int main(int argc, char** argv){
    
    int level = 0;
    char newBuffer[30000];
    
    struct trieNode *root = indexPage(argv[1]);
   
   printTrieContents(root , newBuffer , level);

   freeTrieMemory(root);
    return 0;
}
struct trieNode *makeNode(){
  struct trieNode *pNode = NULL;
  
    pNode = (struct trieNode *)malloc(sizeof(struct trieNode));
  
    if (pNode)
    {
        int i;
  
        pNode->isWord = 0;
  
        for (i = 0; i < TOTAL_ALPHABETS; i++)
            pNode->children[i] = NULL;
    }
  
    return pNode;
}


int addWordOccurrence(const char* word, int wordLength , struct trieNode *pNode)
{
    int level;
    int length = strlen(word);
  
    struct trieNode *temp = pNode;
  
    for (level = 0; level < length; level++)
    {
        int index = CONVERT(word[level]);
        if (!temp->children[index])
            temp->children[index] = makeNode();
  
        temp = temp->children[index];
    }
  
    //mark last node as leaf
    temp->isWord = 1;
    temp->counter += 1;

    return 0;
}

int getText(const char* srcAddr, char* buffer, const int bufSize){
  FILE *pipe;
  int bytesRead;

  snprintf(buffer, bufSize, "curl -s \"%s\" | python getText.py", srcAddr);

  pipe = popen(buffer, "r");
  if(pipe == NULL){
    fprintf(stderr, "ERROR: could not open the pipe for command %s\n",
	    buffer);
    return 0;
  }

  bytesRead = fread(buffer, sizeof(char), bufSize-1, pipe);
  buffer[bytesRead] = '\0';

  pclose(pipe);

  return bytesRead;
}

int freeTrieMemory(struct trieNode *pNode) {
if(pNode == NULL) { // Checking for errors i.e. if the Trie Root is Null.
    return 1;
  }
  else {
    //Traversing through the children of every Trie Node.
    for(int iterateChildren = 0; iterateChildren < TOTAL_ALPHABETS; iterateChildren++) {
      if(pNode->children[iterateChildren] != NULL) { // Checking the child exists
        freeTrieMemory(pNode->children[iterateChildren]); // Recusively traversing to the Child's children so as to finally reach the leaf node.
      }
    }

    free(pNode); //Using free to remove the respective Trie Root from the allocated space within the Heap.
    return 0; // Successful Case.

  }
}


struct trieNode *indexPage(const char* url)
{
const int bufSize = 30000;
char buffer[bufSize];
getText(url , buffer , bufSize);
int endOfBuffer = strlen(buffer);
char wordBuffer[bufSize];
struct trieNode *root = NULL;
root = malloc(sizeof(struct trieNode));
root->isWord = 0;
root->counter = 0;
    for(int i = 0; i < 26; i++){
      root->children[i] = NULL;
    }
printf("%s\n" , url);
int j = 0;
  for(int i = 0; i < endOfBuffer; i++){
    
   if(buffer[i] == '\0'){
     break;
   }
   //if current element is not a letter, but last element is or element -2 is, then decrements
   //the buffer and adds word occurrence
   if(isalpha(buffer[i]) == 0 && (isalpha(buffer[i - 1]) != 0 || isalpha(buffer[i - 2]) != 0)){
     buffer[i] = buffer[i - 1];
     int wordLength = strlen(wordBuffer+1);
     addWordOccurrence(wordBuffer, wordLength , root);
     printf("\t%s\n" , wordBuffer);
     memset(wordBuffer, 0 , bufSize); //clears buffer
     j = 0;
   }
   //if current element is a letter than it adds it to the wordBuffer then increments
   if(isalpha(buffer[i]) != 0){
   wordBuffer[j] = tolower(buffer[i]);
   buffer[i] = buffer[i +1];
   ++j;
   }
   }

return root;
}

int printTrieContents(struct trieNode* trieRoot, char* characterBuffer, int level) {

  //Checking the Trie Root Provided is pointing to NULL. (Error Checking)
  if(trieRoot == NULL) {
    return 1; // If a failure occurs.
  }
  else {

    //Looping through the children of the Trie Root to check if a Child Exists
    for(int iterateChildren = 0; iterateChildren < TOTAL_ALPHABETS; iterateChildren++) {
      //If Child Exists (Traversal takes place in an Alphabetical Manner)
      if(trieRoot->children[iterateChildren] != NULL) {
          //Add Character to Print Buffer and make it a word.
          characterBuffer[level] = iterateChildren + 'a';
          characterBuffer[level+1] = '\0';

          //Display the word stored in the buffer's word count if it is non-zero.
          if(trieRoot->children[iterateChildren]->counter != 0) {
            //Printing the word in the required format.
            printf("%s: %d\n", characterBuffer, trieRoot->children[iterateChildren]->counter);
          }
          //Recursively Traverse through the subtries of the respective root.
          printTrieContents(trieRoot->children[iterateChildren], characterBuffer, level + 1);
      }

    }

    return 0; // If the printing is successful

  }

}