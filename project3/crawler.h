//header
#ifndef CRAWLER_H_
#define CRAWLER_H_


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

//Defining a constant for the Maximum Address Length
#define MAX_ADDR_LENGTH 1000

//Struct listNode
struct listNode
{
  char addr[MAX_ADDR_LENGTH];
  struct listNode *next;
  double tfScore, tfidfScore; //term freq as well as score
};

//Declare Functions
int contains(const struct listNode *pNode, const char *addr);
void insertBack(struct listNode *pNode, const char *addr);
void destroyList(struct listNode *pNode);
int getLink(const char *srcAddr, char *link, const int maxLinkLength);

#endif
//end header