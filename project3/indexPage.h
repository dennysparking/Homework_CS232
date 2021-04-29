//header
#ifndef INDEXPAGE_H_
#define INDEXPAGE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define TOTAL_ALPHABETS 26                      
#define FIRST_LOWERCASE_ALPHABET_ASCII_VALUE 97 // ASCII Value of 'a'
#define MAXIMUM_INDEXED_CHARACTERS 300000       

// Trie Node Data Structure
struct trieNode
{
  int wordCount;                              
  struct trieNode *children[TOTAL_ALPHABETS]; 
  long totalWords;                            //The total number of words indexed by the Trie
 };


//Function declaration
struct trieNode *indexPage(const char *url);

int addWordOccurrence(const char *word, const int wordLength, struct trieNode *);
int freeTrieMemory(struct trieNode *trieRoot);
int findWordOccurence(struct trieNode *trieRoot, const char *word);
int getText(const char *srcAddr, char *buffer, const int bufSize);

#endif