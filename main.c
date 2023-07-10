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

char inputText[2000];

struct Symbol
{
   int address;
   char name[20];
};

struct Symbol symbols[100];
int indexSymbols = 0;

int skipBlank(int i, char input[])
{
   while (input[i] == ' ')
   {
      i++;
   }

   return i;
}

int jumpToEndOfWord(int i, char input[])
{
   while (input[i] != ' ')
   {
      i++;
   }

   return i;
}

int isWordMatch(int i, char word[], char input[])
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

int isOpCode(int i, char input[])
{
   int x;
   for (x = 0; x < sizeof(opcode); x++)
   {
      if (isWordMatch(i, opcode[x], input) == 1)
      {
         return x;
      }
   }

   return -1;
}

int isRegister(int i, char input[])
{
   int x;
   for (x = 1; x <= 8; x++)
   {
      char buff[4];
      /*connect int x to string buff*/
      snprintf("@r%d", 4, x);
      if (isWordMatch(i, buff, input) == 1)
      {
         return x;
      }
   }

   return -1;
}

int isSymbol(int i, char input[])
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

int isMcro(int i, char input[])
{
   if (isWordMatch(i, "mcro", input))
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

struct Mcro createMcro(int i, char input[])
{
   static struct Mcro m;
   int mindex = 0;
   while (!isWordMatch(i, "endmcro", input))
   {
      /*add mcro name*/
      i = jumpToEndOfWord(i, input);
      i++;
      i = skipBlank(i, input);
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


int isData(int i, char input)
{
   if (isWordMatch(i, ".data", input))
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

int isStringOperand(int i, char input)
{
   if (isWordMatch(i, ".string", input))
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

int isEntry(int i, char input)
{
   if (isWordMatch(i, ".entry", input))
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

int isExtern(int i, char input)
{
   if (isWordMatch(i, ".extern", input))
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

/*new code here**************************** */




/*end new code here**************************** */

int main()
{
   scanf("%s", inputText);

   /*input index*/
   long i = 0;
   /*memory index*/
   long mi = 0;

   while (i < strlen(inputText))
   {

      if (inputText[i] == ' ')
      {
         i = skipBlank(i, inputText);
      }

      if (inputText[i] == ',')
      {
         i++;
      }

      if (isSymbol(i, inputText))
      {

         int cnt = 0;
         while (inputText[i] != ' ')
         {
            cnt++;
         }

         int j;
         for (j = 0; j < cnt; j++)
         {
            symbols[indexSymbols].name[j] = inputText[i + j];
         }

         symbols[indexSymbols].address = mi;
         indexSymbols++;
      }

      i = jumpToEndOfWord(i, inputText);
      i++;

      mi++;
   }

   i = 0;
   mi = 0;
   indexSymbols = 0;

   /* until here its first level of the compliler */

   /*create macros*/
   while (i < strlen(inputText))
   {
      if (inputText[i] == ' ')
      {
         i = skipBlank(i, inputText);
      }

      if (inputText[i] == ',')
      {
         i++;
      }

      if (isMcro(i, inputText))
      {
         /*jump after word mcro*/
         i = jumpToEndOfWord(i, inputText);
         i++;

         mcros[mcroIndex] = createMcro(i, inputText);
         mcroIndex++;
         while (!isWordMatch(i, "endmcro", inputText))
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
   while (i < strlen(inputText))
   {
      newInput[i2] = inputText[i];

      if (isMcro(i, inputText))
      {
         /*skip mcro defenition code*/
         while (!isWordMatch(i, "endmcro", inputText))
         {
            i++;
         }

         i = jumpToEndOfWord(i, inputText);
      }

      i++;
      i2++;
   }

   i = 0;
   i2 = 0;

   /*insert insider mcro code into mcro instances*/
   while (i2 < strlen(newInput))
   {
      if (isWordMatch(i2, mcros[mcroIndex].name, newInput))
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
