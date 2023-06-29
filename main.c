#include <stdio.h>
#include <string.h>
#include "var.c"

char input[1000];
long code[1024];
long wordCounter = 0;


int main() {
   scanf("%s",input);

   long i = 0;
   while(i < strlen(input))
   {
      if(input[i] == 'm' && input[i + 1] == 'o' && input[i + 2] == 'v')
      {
         code[wordCounter] = 0;
         if(input[i + 4] == '@')
         {

         }
      }

      i++;
   }

   printf("%d",i);
   
   return 0;
}


