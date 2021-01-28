#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(int argc, const char** argv){

    int lowerBound, upperBound;

    if(argc != 3){
        fprintf(stderr, "USAGE: %s lowerBound upperBound\n", argv[0]);
        return -1;
    }
  
    lowerBound = atoi(argv[1]);
    upperBound = atoi(argv[2]);
  
    if(lowerBound < 1 || upperBound < 1){
        fprintf(stderr, "ERROR: both the lowerBound (%d) and the upperBound (%d)"
            " must be positive.\n", lowerBound, upperBound);
        return -2;
    }
  //declare variables for loop
  int i, j, flag_var;
  

 
  // nested for loop to print/and find prime numbers        
    for (i=lowerBound; i<=upperBound; ++i) {

        if (i == 1){
            i=i+1;
        }

        if (upperBound==1 && lowerBound==1){
            return 0;
        }

      flag_var=0;
      for(j=2; j<=i/2; ++j)
      {
         if(i%j==0)
         {
            flag_var=1;
            break;
         }
      }
      if(flag_var==0)
         printf("%d\n",i);
  
}
return 0;
}