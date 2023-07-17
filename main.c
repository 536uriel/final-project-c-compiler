#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "to-binary.c"

#define FALSE 0;
#define TRUE 1;

char opcode[16][4] = {
    "mov", "cmp", "add", "sub", "lea",
    "not", "clr", "inc", "dec", "jmp", "bne", "red", "prn", "jsr",
    "rts", "stop"};

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

int isData(int i, char input[])
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

int isStringOperand(int i, char input[])
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

int isEntry(int i, char input[])
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

int isExtern(int i, char input[])
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

/*to do: translate to binary code*/

int isDigit(char c)
{
   if (c > '0' && c < '9')
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

int toInt(char digits[])
{

   int num = 0;
   int index;
   for (index = 0; index < sizeof(digits); index++)
   {
      int digit = digits[index] - '0';
      if (num == 0)
      {
         num = digit;
      }
      else
      {
         num *= 10;
         num += digit;
      }
   }

   return num;
}

int isLetter(char c)
{
   if (c > 'a' && c < 'z')
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

int is_r_to_r_case(int i, char input[])
{
   if (input[i] == '@')
   {
      i += 3;
      i = skipBlank(i, input);
      i++;
      i = skipBlank(i, input);
      if (input[i] == '@')
      {
         return TRUE;
      }
      else
      {
         return FALSE;
      }
   }
   else
   {
      return FALSE;
   }
}

int get_opcode_group(int iopcode)
{

   /*opcode group one*/
   if (iopcode < 5)
   {
      return 1;
   }
   /*opcode group two*/
   if (5 <= iopcode && iopcode < 14)
   {
      return 2;
   }
   /*opcode group three*/
   if (14 <= iopcode && iopcode < 16)
   {
      return 3;
   }
}

int get_symbol_address(int itmp, char input[])
{
   /*find the symbol address*/
   int i2;
   char symbole_name[20];
   int symbolen = jumpToEndOfWord(itmp, input) - itmp;
   for (i2 = 0; i2 < symbolen; i2++)
   {
      symbole_name[i2] = input[itmp];
      itmp++;
   }

   int j;
   int bool = FALSE;
   int tmpindex;
   for (i2 = 0; i2 < sizeof(symbols); i2++)
   {
      for (j = 0; j < sizeof(symbols[i2].name); j++)
      {
         if (symbols[i2].name[j] == symbole_name[j])
         {
            bool = TRUE;
         }
         else
         {
            bool = FALSE;
            break;
         }
      }

      if (bool)
      {
         tmpindex = i2;
      }
   }

   int symbol_address = symbols[tmpindex].address;
   return symbol_address;
}

int *opcode_case_to_binary(int iopcode, int i, char input[])
{
   /*decode array in decimal*/
   static int d_code[3][4];
   d_code[0][0] = 0;
   int itmp = i;

   itmp = jumpToEndOfWord(itmp, input);
   itmp = skipBlank(itmp, input);

   int opcode_group = get_opcode_group(iopcode);

   /*start index input from sccond operand*/

   if (opcode_group == 1)
   {

      /*to do: check sccond operand  & third operand if
       label case or register case or number case!!!!!!!!!!!!!!!!!!!!!!!!*/

      if (isDigit(input[itmp]))
      {
         int numlen = jumpToEndOfWord(i, input) - i;
         int i2;
         char cdigits[numlen];
         for (i2 = 0; i2 < numlen; i2++)
         {
            cdigits[i2] = input[itmp];
            itmp++;
         }

         itmp++;

         int numoperand = toInt(cdigits);

         /*to compleate.......*/
         itmp = skipBlank(itmp, input);
         itmp++;
         itmp = skipBlank(itmp, input);

         if (input[itmp] == '@')
         {
            int registernum = input[itmp + 2] - '0';

            /*case of opcode number,register*/

            d_code[0][1] = 1;
            d_code[0][2] = iopcode;
            d_code[0][3] = 5;

            d_code[1][0] = numoperand;
            d_code[1][1] = 0;
            d_code[1][2] = 0;

            d_code[2][0] = registernum;
            d_code[2][1] = 0;
            d_code[2][2] = 0;
         }
         else
         {

            /*case of opcode number,label*/

            d_code[0][1] = 1;
            d_code[0][2] = iopcode;
            d_code[0][3] = 3;

            d_code[1][0] = numoperand;
            d_code[1][1] = 0;
            d_code[1][2] = 0;

            d_code[2][0] = get_symbol_address(itmp, input);
            d_code[2][1] = 0;
            d_code[2][2] = 0;
         }
      }
      else
      {
         if (is_r_to_r_case(itmp, input))
         {

            d_code[0][1] = 5;
            d_code[0][2] = iopcode;
            d_code[0][3] = 5;

            d_code[1][0] = 0;
            d_code[1][1] = 5;
            d_code[1][2] = 5;
            /*case -1 means ignore the last decoding*/
            d_code[1][3] = -1;

            int i2;
            int j;
            for (i2 = 2; i2 < 4; i2++)
            {
               for (j = 0; j < 4; j++)
               {
                  d_code[i2][j] = -1;
               }
            }

            i2 = 0;
            j = 0;
         }
      }
   }

   if (opcode_group == 2)
   {

      if (input[itmp] == '@')
      {
         d_code[0][1] = input[itmp + 2] - '0';
         d_code[0][2] = iopcode;
         d_code[0][3] = 0;
      }
      else
      {
         d_code[0][1] = get_symbol_address(itmp, input);
         d_code[0][2] = iopcode;
         d_code[0][3] = 0;
      }

      /*ignore the last operand*/
      int j;

      for (j = 0; j < 4; j++)
      {
         d_code[2][j] = -1;
      }
   }

   if (opcode_group == 3)
   {
      d_code[0][1] = 0;
      d_code[0][2] = iopcode;
      d_code[0][3] = 0;

      int j;

      for (j = 0; j < 4; j++)
      {
         d_code[1][j] = -1;
      }

      for (j = 0; j < 4; j++)
      {
         d_code[2][j] = -1;
      }
   }

   return d_code;
}

/*new code here****************************************** */

/*to do: */

int *directive_cases_to_binary(int i, char input[])
{
}

/*end new code here*************************************** */

int main()
{
   /*to fix: scanf("%s", inputText);*/

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
