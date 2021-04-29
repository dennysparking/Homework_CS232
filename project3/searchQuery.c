// header 
#include "searchQuery.h"

// calc term frequency
double calcTermFrequency(char *term, struct trieNode *indexedPages)
{
   
    int termFrequency = 0; 
    double tfScore = 0;

    // termFrequency equals findWordOccurence
    termFrequency = findWordOccurence(indexedPages, term);

    if (termFrequency == 0)
    {
        tfScore = 0; 
    }
    else
    {
        // Using the formula given in the project description to compute the tf score.
        tfScore = (double)((double)termFrequency / (double)indexedPages->totalWords);
    }

    return tfScore;
}

// used to calculate InverseDocFrequency
double calcInverseDocFrequency(int totalIndexed, int indexedWithTerm)
{
    //Using the formula provided within the project description to compute the Inverse Document frequency.
    double logTerm = (double)(((double)(1 + totalIndexed)) / ((double)(1 + indexedWithTerm)));
    double idfScore = log(logTerm);

    return idfScore;
}

//calculate the tf-idf score
void calcTfidfScore(struct listNode *documentList, double idfScore)
{
    // while not null, go through list
    while (documentList != NULL)
    {
        //Adding the tf-idf score
        documentList->tfidfScore += documentList->tfScore * idfScore;
        documentList = documentList->next;
    }

    return;
}
