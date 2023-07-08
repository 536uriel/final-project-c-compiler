#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "to-binary.c"

#define FALSE 0;
#define TRUE 1;

char opcode[16][4] = {
    "mov", "cmp", "add", "sub",
    "not", "clr", "lea", "inc", "dec",
    "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};

char input[2000];
long code[1024];

struct Symbol
{
   int address;
   char name[];
};

struct Symbol symbols[100];
int indexSymbols = 0;


int isWordMatch(int i, char word[])
{
   int index;
   for (index = 0; index < strlen(word); index++)
   {
      if (input[i] != word[index])
      {
         return FALSE;
      }

      if (index == strlen(word))
      {
         return TRUE;
      }

      i++;
   }
}

int isOpCode(int i)
{
   int x;
   for (x = 0; x < sizeof(opcode); x++)
   {
      if (isWordMatch(i, opcode[x]) == 1)
      {
         return x;
      }
   }

   return -1;
}

int isRegister(int i)
{
   int x;
   for (x = 1; x <= 8; x++)
   {
      char buff[4];
      /*connect int x to string buff*/
      snprintf("@r%d", 4, x);
      if (isWordMatch(i, buff) == 1)
      {
         return TRUE;
      }
   }

   return FALSE;
}

int isSymbol(i)
{
   while (input[i] != ' ')
   {
      i++;
   }
   if (input[i - 1] == ':')
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

int jumpToNextWord(int i)
{
   while (input[i] == ' ')
   {
      i++;
   }

   return i;
}

int jumpToEndOfWord(int i)
{
   while (input[i] != ' ')
   {
      i++;
   }

   return i;
}

int main()
{
   scanf("%s", input);

   long i = 0;
   long ci = 0;

   while (i < strlen(input))
   {

      if (input[i] == ' ')
      {
         i = jumpToNextWord(i);
      }

      int opCodeIndex = isOpCode(i);
      if (opCodeIndex != -1)
      {
         code[ci] = opCodeIndex;
      }
      else
      {
         if (isRegister(i))
         {
         }
         else
         {
            if (isSymbol(i))
            {

               int cnt = 0;
               while (input[i] != ' ')
               {
                  cnt++;
               }
               
               int j;
               for (j = 0; j < cnt; j++)
               {
                  symbols[indexSymbols].name[j] = input[i + j];
               }

               symbols[indexSymbols].address = ci;
               indexSymbols++;

            
            }
            else
            {
            }
         }
      }

      i = jumpToEndOfWord(i);
      i++;

      ci++;
   }

   return 1;
}
