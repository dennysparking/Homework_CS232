#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ADDR_LENGTH 1000

/*
 * a node in our linked-list of web addresses
 */
struct listNode {
  char addr[MAX_ADDR_LENGTH];
  struct listNode *next;
};

/*
 * returns 1 if the list starting at pNode contains the address addr,
 *    and returns 0 otherwise
 */
int contains(const struct listNode *pNode, const char *addr);

/*
 * inserts the address addr as a new listNode at the end of
 *    the list
 */
void insertBack(struct listNode *pNode, const char *addr);

/*
 * prints the addresses from pNode to the end of the list,
 *   one on each line
 */
void printAddresses(const struct listNode *pNode);

/*
 * frees the memory associated with this node and all subsequent nodes
 */
void destroyList(struct listNode *pNode);
  
/*
 * srcAddr should be a web address (e.g., http://www.yahoo.com).
 * link should point to an array of maxLinkLength characters.
 * getLink returns 1 if a link was found and 0 otherwise.
 * If a link was found, "link" will be filled in with the web address.
 */
int getLink(const char* srcAddr, char* link, const int maxLinkLength);

int main(int argc, char** argv){
  
  long seed;

  char startAddr[MAX_ADDR_LENGTH];
  char destAddr[MAX_ADDR_LENGTH];
  
  int hopNum, numHops;
  
  struct listNode *pListStart;

  if(argc < 3 || argc > 4){
    fprintf(stderr, "USAGE: %s startAddr numHops [rand seed]", argv[0]);
    return -1;
  }
  
  /* initialization */
  if(argc >= 4){
    seed = atol(argv[3]);
  }
  else{
    seed = time(NULL);
  }

  printf("seed = %ld\n", seed);
  srand(seed);

  strncpy(startAddr, argv[1], MAX_ADDR_LENGTH);
  startAddr[MAX_ADDR_LENGTH - 1] = '\0';

  numHops = atoi(argv[2]);

  pListStart = malloc(sizeof(struct listNode));
  if(pListStart == NULL){
    fprintf(stderr, "ERROR: could not allocate memory\n");
    return -2;
  }
  strncpy(pListStart->addr, startAddr, MAX_ADDR_LENGTH);
  pListStart->next = NULL;

  /* start the crawling */
  for(hopNum=1; hopNum <= numHops; hopNum++){
    int res = getLink(startAddr, destAddr, MAX_ADDR_LENGTH);

    if(!res){
      printf("Dead end on hop %d: no outgoing links\n", hopNum);
      break;
    }

    if(contains(pListStart, destAddr)){
      printf("Cycle detected on hop %d: address %s\n", hopNum, destAddr);
      hopNum--; // try again for this hop in the next iteration
    }
    else{
      insertBack(pListStart, destAddr);
      strncpy(startAddr, destAddr, MAX_ADDR_LENGTH);
    }
  }

  printAddresses(pListStart);

  destroyList(pListStart);

  return 0;
}


/*
 * returns 1 if the list starting at pNode contains the address addr,
 *    and returns 0 otherwise
 */
int contains(const struct listNode *pNode, const char *addr) {


  //Checking if the current pNode is null
  if(pNode == NULL) {
    return 0; //If no node has been found to contain the address, we return 0.
  }
  else {
    //Checking if pNode contains the given address parameter using strncmp.
    if(strncmp(pNode->addr, addr, MAX_ADDR_LENGTH) == 0) { //If the contained address and given address are equal
      return 1; 
    }
    else {
      return contains(pNode->next, addr); // Traverse through linked list using recursion.
    }

  }

}


/*
 * inserts the address addr as a new listNode at the end of
 *    the list
 */
void insertBack(struct listNode *pNode, const char *addr) {

  //Checking if we have reached the tail of the linked list.
  if(pNode->next == NULL) {

    //Creating a newNode and allocating space for it in the heap.
    struct listNode *newNode = malloc(sizeof(struct listNode));

    if(newNode == NULL) { // In case there was no free space in the heap.
      printf("ERROR : Memory Could Not Be Allocated for the New Node.\n");
      return;
    }

    //Copying the given address to the address member of the newNode using strncpy function.
    strncpy(newNode->addr, addr, MAX_ADDR_LENGTH);

    //Setting the newNode's next pointer to NULL as it is being added to the end of the linked list.
    newNode->next = NULL;

    //Connecting the last node of the linked list to the newNode.
    pNode->next = newNode;

    return;

  }
  else {
      insertBack(pNode->next, addr); //Traversing until the last node of the linked list is reached.
  }

}

/*
 * prints the addresses from pNode to the end of the list,
 *   one on each line
 */
void printAddresses(const struct listNode *pNode) {
 

  //Checking if the end of the linked last has been reached.
  if(pNode == NULL) {
    return;
  }

  //Printing the Address of the Node and then traversing to the next node.
  printf("%s\n" , pNode->addr);
  printAddresses(pNode->next); //Recursive Call.

}

/*
 * frees the memory associated with this node and all subsequent nodes
 */
void destroyList(struct listNode *pNode){
 
  //Checking if the end of the linked list has been reached.
  if(pNode == NULL) {
    return;
  }

  //Traversing to the end of the linked list first so as to begin deleting from the end.
  
  destroyList(pNode->next); // Recursive Call.
  free(pNode); // To free chunks of heap memory.

}
  
int getLink(const char* srcAddr, char* link, const int maxLinkLength){
  const int bufSize = 1000;
  char buffer[bufSize];

  int numLinks = 0;

  FILE *pipe;

  snprintf(buffer, bufSize, "curl -s \"%s\" | python getLinks.py", srcAddr);

  pipe = popen(buffer, "r");
  if(pipe == NULL){
    fprintf(stderr, "ERROR: could not open the pipe for command %s\n", buffer);
    return 0;
  }

  fscanf(pipe, "%d\n", &numLinks);

  if(numLinks > 0){
    int linkNum;
    double r = (double)rand() / ((double)RAND_MAX + 1.0);

    for(linkNum=0; linkNum<numLinks; linkNum++){
      fgets(buffer, bufSize, pipe);
      
      if(r < (linkNum + 1.0) / numLinks){
		    break;
      }
    }

    /* copy the address from buffer to link */
    strncpy(link, buffer, maxLinkLength);
    link[maxLinkLength-1] = '\0';
    
    /* get rid of the newline */
    {
      char* pNewline = strchr(link, '\n');
      if(pNewline != NULL){
		    *pNewline = '\0';
      }
    }
  }

  pclose(pipe);

  if(numLinks > 0){
    return 1;
  }
  else{
    return 0;
  }
}
