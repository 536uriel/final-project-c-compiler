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

struct Symbol
{
   int address;
   char name[20];
};

struct Symbol symbols[100];
int indexSymbols = 0;

int skipBlank(int i)
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
         return x;
      }
   }

   return -1;
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

/*new code here**************************** */

struct Mcro
{
   char str[500];
};

int isData()
{
}

int isStringOperand()
{
}

int isExtern()
{
}

struct Mcro isMcro(int i)
{
}

/*end new code here**************************** */

int main()
{
   scanf("%s", input);

   /*input index*/
   long i = 0;
   /*memory index*/
   long mi = 0;

   while (i < strlen(input))
   {

      if (input[i] == ' ')
      {
         i = skipBlank(i);
      }

      if (input[i] == ',')
      {
         i++;
      }

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

         symbols[indexSymbols].address = mi;
         indexSymbols++;
      }

      i = jumpToEndOfWord(i);
      i++;

      mi++;
   }

   i = 0;
   mi = 0;
   indexSymbols = 0;

   /* until here its first level of the compliler */

   return 1;
}
