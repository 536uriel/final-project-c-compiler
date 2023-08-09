#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "limits.h";
#include "to-binary.c"

#define FALSE 0;
#define TRUE 1;

char *readFileToString(const char *filename)
{

   char *cwd;
   char buff[PATH_MAX + 1];

   cwd = getcwd(buff, PATH_MAX + 1);
   if (cwd != NULL)
   {
      printf("Current working directory: %s\n", cwd);

      // If you have a file name, say "myfile.txt" in the current directory,
      // you can create its full path as follows:
      char full_path[PATH_MAX + 1];
      snprintf(full_path, sizeof(full_path), "%s/%s", cwd, filename);
      printf("Full path: %s\n", full_path);

      FILE *file = fopen(filename, "r"); // Open the file in read mode
      if (!file)
      {
         perror(filename);
         return NULL; // If file does not exist, return NULL
      }

      // Determine the size of the file
      fseek(file, 0, SEEK_END);
      long length = ftell(file);
      fseek(file, 0, SEEK_SET);

      // Allocate memory for the string, including the null terminator
      char *content = (char *)malloc(length + 1);
      if (!content)
      {
         fclose(file);
         return NULL; // Failed to allocate memory
      }

      // Read the file into the allocated memory
      fread(content, 1, length, file);
      content[length] = '\0'; // Null-terminate the string

      fclose(file);
      return content;
   }
   else
   {
      perror("error");
      exit(0);
   }
}

char opcode[16][4] = {
    "mov", "cmp", "add", "sub", "lea",
    "not", "clr", "inc", "dec", "jmp", "bne", "red", "prn", "jsr",
    "rts", "stop"};

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
   for (x = 0; x < (sizeof(opcode) / sizeof(opcode[0])); x++)
   {
      if (isWordMatch(i, opcode[x], input))
      {
         return x;
      }
      printf("%c", ' ');
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

   /* to fix */
   while (input[i] != ' ' & i < strlen(input))
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
   i = jumpToEndOfWord(i, input);
   i = skipBlank(i, input);
   int iname = 0;
   /*add mcro name*/
   while (input[i] != ' ')
   {
      m.name[iname] = input[i];
      iname++;
      i++;
   }

   int mindex = 0;

   while (!isWordMatch(i, "endmcro", input))
   {
      printf("%s", " ");
      m.str[mindex] = input[i];
      i++;
      mindex++;
   }

   return m;
}

int isDirective(int i, char input[])
{
   if (isWordMatch(i, ".data", input) || isWordMatch(i, ".string", input) ||
       isWordMatch(i, ".entry", input) || isWordMatch(i, ".extern", input))
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

int **opcode_case_to_binary(int iopcode, int i, char input[])
{
   /*decode array in decimal*/
   static int d_code[4][4];
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

   /*the last index of d_code is the index of the input itself*/
   d_code[3][0] = itmp;
   int x;
   int y;
   for (x = 0; x < 4; x++)
   {
      for (y = 0; y < 4; y++)
      {
         printf("%d", d_code[x][y]);
      }
   }

   return d_code;
}

int *directive_cases_to_binary(int i, char input[])
{
   /*decode array in decimal*/
   static int d_code[11];

   if (isWordMatch(i, ".data", input))
   {
      i = jumpToEndOfWord(i, input);

      int di = 0;

      /*loop over the numbers until got to letter*/
      while (isLetter(input[i]) == 0)
      {
         i = skipBlank(i, input);
         if (input[i] == ',')
         {
            i++;
            i = skipBlank(i, input);
         }
         int numlen = jumpToEndOfWord(i, input) - i;
         char num[numlen];
         int j;
         for (j = 0; j < numlen; j++)
         {
            num[j] = input[i];
            i++;
         }

         int number = toInt(num);

         d_code[di] = number;
         di++;
      }

      /*the last index of d_code is the index of the input itself*/
      d_code[10] = i;

      return d_code;
   }

   if (isWordMatch(i, ".string", input))
   {
      int strlength = jumpToEndOfWord(i, input) - i;
      int j;
      for (j = 0; j < strlength; j++)
      {
         d_code[j] = input[i];
         i++;
      }
   }
}

int main()
{

   char *inputText = readFileToString("input.txt");
   /*input index*/
   int i = 0;
   /*memory index*/
   int mi = 0;
   printf("%s", "start debug: ");

   while (i < strlen(inputText))
   {

      i = skipBlank(i, inputText);

      if (isSymbol(i, inputText))
      {

         int cnt = i;
         while (inputText[cnt] != ' ')
         {
            cnt++;
         }

         int j;
         /*cnt - i => the length of the symbol word*/
         for (j = 0; j < cnt - i - 1; j++)
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

   /*for testing*/
   int j;
   for (j = 0; j < (sizeof(symbols) / sizeof(symbols[0])); j++)
   {
      printf("%s", (symbols[j].name));
      printf("%s", "/");
   }
   indexSymbols = 0;

   /* until here its first level of the compliler */
   // /*create macros*/
   while (i < strlen(inputText))
   {
      i = skipBlank(i, inputText);

      printf("%c", ' ');

      if (isMcro(i, inputText))
      {
         printf("%s", "is mcro");
         // /*jump after word mcro*/
         i = jumpToEndOfWord(i, inputText);

         mcros[mcroIndex] = createMcro(i, inputText);
         mcroIndex++;
      }

      i = jumpToEndOfWord(i, inputText);
   }

   /*for testing mcros*/
   printf("%s", mcros[0].name);
   printf("%s", mcros[0].str);

   i = 0;
   mcroIndex = 0;

   char newInput[2000];
   int i2 = 0;

   /*skip mcro code in new input text*/
   while (i < strlen(inputText))
   {
      printf("%c", ' ');

      if (isMcro(i, inputText))
      {
         /*skip mcro defenition*/
         printf("%s", "skip mcro");
         i = jumpToEndOfWord(i, inputText);
         i = skipBlank(i, inputText);

         /*skip mcro str code*/
         while (!isWordMatch(i, "endmcro", inputText))
         {
            printf("%c", ' ');
            i++;
         }

         i = jumpToEndOfWord(i, inputText);
      }

      newInput[i2] = inputText[i];

      i++;
      i2++;
   }

   i = 0;
   i2 = 0;
   mcroIndex = 0;
   char newInput2[2000];

   for (mcroIndex = 0; mcroIndex < (sizeof(mcros) / sizeof(mcros[0])); mcroIndex++)
   {
      /*insert insider mcro code into mcro instances*/
      while (i2 < (sizeof(newInput2) / sizeof(newInput2[0])))
      {

         printf("%c", ' ');
         if (isWordMatch(i, mcros[mcroIndex].name, newInput) && strlen(mcros[mcroIndex].name) > 0)
         {
            printf("%s", "mcro found: ");
            printf("%s", mcros[mcroIndex].name);
            int lentmp = strlen(mcros[mcroIndex].str);
            int itmp = 0;

            while (itmp < lentmp)
            {

               newInput2[i2] = mcros[mcroIndex].str[itmp];
               i2++;
               itmp++;
            }

            /*skip mcro name in last input*/
            i = jumpToEndOfWord(i, newInput);
         }
         else
         {
            newInput2[i2] = newInput[i];
            i++;
            i2++;
         }
      }
   }

   i2 = 0;
   mcroIndex = 0;
   i = 0;

   /*for testing*/
   while (i < strlen(newInput2))
   {
      printf("%c", newInput2[i]);
      i++;
   }

   i = 0;

   // /*until here summery:
   // we created array of symbol with there names and addresses
   // and we created macros  instances and replaced the instance
   // with the insider code*/

   // /*test the array returned from function */
   // /* int *arr= test();
   // printf("%d",arr[0]); */

   // /* to do: ignore after .extermn/entry label operands******************** */

   // /*new code here****************************************** */

   // /*convert newinput to binary*/

   /* to fix */

   i = 0;

   char d_code[1000][12];
   int dindex = 0;

   while (i < strlen(newInput2))
   {
      printf("%c", ' ');
      i = skipBlank(i, newInput2);

      int iopcode = isOpCode(i, newInput2);
      if (iopcode != -1)
      {   
         /*skip opcode word*/
         i = jumpToEndOfWord(i,newInput2);
         i = skipBlank(i,newInput2);     

         int opcode_group = get_opcode_group(iopcode);
         if (opcode_group == 1)
         {
            if (isDigit(newInput2[i]))
            {
               char *strnum;
               
            }
         }

      }
      // else
      // {
      //    if (isDirective(i, newInput2))
      //    {
      //       int *d_code = directive_cases_to_binary(i, newInput2);
      //       int x;
      //       for (x = 0; x < sizeof(d_code); x++)
      //       {
      //          printf("%d", d_code[x]);
      //       }
      //    }
      //    else
      //    {
      //       i = jumpToEndOfWord(i, newInput2);
      //    }
      // }
      i = jumpToEndOfWord(i, newInput2);
   }

   /*end new code here*************************************** */

   return 0;
}
