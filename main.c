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

struct Mcro
{
   char name[30];
   char str[500];
};
struct Mcro mcros[100];
int mcroIndex = 0;

int isMcro(int i)
{
   if (isWordMatch(i, "mcro"))
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

struct Mcro createMcro(int i)
{
   static struct Mcro m;
   int mindex = 0;
   while (!isWordMatch(i, "endmcro"))
   {
      /*add mcro name*/
      i = jumpToEndOfWord(i);
      i++;
      i = skipBlank(i);
      int iname = 0;
      while (input[i] != ' ')
      {
         m.name[iname] = input[i];
         iname++;
         i++;
      }

      i++;

      m.str[mindex] = input[i];
      i++;
      mindex++;
   }

   return m;
}

/*new code here**************************** */

int isData()
{
}

int isStringOperand()
{
}

int isExtern()
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

   /*create macros*/
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

      if (isMcro(i))
      {
         /*jump after word mcro*/
         i = jumpToEndOfWord(i);
         i++;

         mcros[mcroIndex] = createMcro(i);
         mcroIndex++;
         while (!isWordMatch(i, "endmcro"))
         {
            i++;
         }
         /*jump after endmcro*/
         i += 8;
      }
   }

   i = 0;
   mcroIndex = 0;

   char newInput[2000];
   int i2 = 0;

   /*skip mcro code in new input text*/
   while (i < strlen(input))
   {
      newInput[i2] = input[i];

      if (isMcro(i))
      {
         /*skip mcro defenition code*/
         while (!isWordMatch(i, "endmcro"))
         {
            i++;
         }

         i = jumpToEndOfWord(i);
      }

      i++;
      i2++;
   }

   i = 0;
   i2 = 0;

   /*insert insider mcro code into mcro instances*/
   while (i2 < strlen(newInput))
   {
      if (isWordMatch(i2, mcros[mcroIndex].name))
      {
         int lentmp = strlen(mcros[mcroIndex].str);
         int itmp = 0;
         while (itmp < lentmp)
         {
            newInput[i2] = mcros[mcroIndex].str[itmp];
            i2++;
            itmp++;
         }

         mcroIndex++;
      }

      i2++;
   }

   i2 = 0;
   mcroIndex = 0;

   /*until here summery:
   we created array of symbol with there names and addresses
   and we created macros  instances and replaced the instance
   with the insider code*/

   return 1;
}
