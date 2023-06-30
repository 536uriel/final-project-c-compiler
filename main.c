#include <stdio.h>
#include <string.h>

#define FALSE 0;
#define TRUE 1;

char input[1000];
long code[1024];
char opcode[16][4] = {
   "mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","stop"
};

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

int isOpCode(int i)
{
   int x;
   for(x = 0;x < strlen(opcode);x++)
   {
     if(isWordMatch(i,opcode[x]) == 1)
     {
      return TRUE;
     }
   }

   return FALSE;
}

int isRegister(int i)
{
   int x;
   for(x = 1;x <= 8;x++)
   {
     if(isWordMatch(i,("@r" + x))== 1)
     {
      return TRUE;
     }
   }

   return FALSE;
}


int main() {
   scanf("%s",input);

   long i = 0;
   while(i < strlen(input))
   {



      i++;
   }

   
   return 1;
}


