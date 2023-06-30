#include <stdio.h>
#include <string.h>

#define FALSE 0;
#define TRUE 1;

char input[1000];
long code[1024];
long wordCounter = 0;

int isWordMatch(int i,char word[])
{
   int index;
   for(index = 0;index < strlen(word);index++)
   {
      if(input[i] != word[index])
      {
         return FALSE;
      }

      if(index == strlen(word))
      {
         return TRUE;
      }

      i++;
   }
}


int main() {
   scanf("%s",input);

   long i = 0;
   while(i < strlen(input))
   {
      

      i++;
   }

   // printf("%d",i);
   
   return 1;
}


