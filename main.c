#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "limits.h";

#define FALSE 0;
#define TRUE 1;

// Base64 character set
char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char getBase64Char(int val)
{
   if (val >= 0 && val < 64)
   {
      return base64_chars[val];
   }
   return ' '; // Error condition, you can handle it as you like.
}

char *convertArrayToBase64(int arr[12])
{
   int first6 = 0, next6 = 0;

   // Convert the first 6 cells into a single integer
   for (int i = 0; i < 6; i++)
   {
      first6 = (first6 << 1) | (arr[i] & 0x01);
   }

   // Convert the next 6 cells into a single integer
   for (int i = 6; i < 12; i++)
   {
      next6 = (next6 << 1) | (arr[i] & 0x01);
   }

   // Convert the two integers into Base64 characters
   static char result[3];
   result[0] = getBase64Char(first6);
   result[1] = getBase64Char(next6);
   result[2] = '\0';

   return result;
}

int *decimalToBinary(int num, int array_size)
{
   int *arr = (int *)malloc(sizeof(int) * array_size);
   if (!arr)
   {
      perror("Failed to allocate memory");
      exit(1);
   }

   // Initialize the array to all zeros.
   for (int i = 0; i < array_size; i++)
   {
      arr[i] = 0;
   }

   // If the number is negative, calculate its 2's complement.
   if (num < 0)
   {
      num = -num; // Take the absolute value.
      num = ~num; // Invert the bits.
      num += 1;   // Add 1.
   }

   // Convert the number to binary and store it in the array.
   int index = 0;
   while (num && index < array_size)
   {
      arr[index] = num & 1;
      num >>= 1;
      index++;
   }

   return arr;
}

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

      // Allocate memory for the string, including the sapce added and null terminator
      char *content = (char *)malloc(length + 2);
      if (!content)
      {
         fclose(file);
         return NULL; // Failed to allocate memory
      }

      // Read the file into the allocated memory
      fread(content, 1, length, file);
      /*add spce to the end of content*/
      content[length] = ' ';
      content[length + 1] = '\0'; // Null-terminate the string

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
   int isExtern;
   int isEntry;
};

struct Symbol symbols[100];
int indexSymbols = 0;

int getSymbolIndex(char symbol[])
{
   int i = 0;
   int i2 = 0;
   int bol = TRUE;
   for (i = 0; i < (sizeof(symbols) / sizeof(symbols[0])); i++)
   {
      if (strlen(symbols[i].name) == strlen(symbol))
      {
         bol = TRUE;
         for (i2 = 0; i2 < strlen(symbols[i].name); i2++)
         {
            if (symbols[i].name[i2] != symbol[i2])
            {
               bol = FALSE;
               break;
            }
         }

         if (bol)
         {
            return i;
         }
      }
   }

   return -1;
}

int skipBlank(int i, char input[])
{
   while (input[i] == ' ')
   {
      i++;
   }

   if (input[i] == '\n')
   {
      i++;
   }

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

int isString(int i, char input[])
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

int isExternOrEntry(int i, char input[])
{
   if (isWordMatch(i, ".entry", input) || isWordMatch(i, ".extern", input))
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

int isDigit(char c)
{
   if (c >= '0' && c <= '9' || c == '-')
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

   int bol = 1;
   int skipIndex = 0;

   if (digits[0] == '-')
   {
      bol = -1;
      skipIndex = 1;
   }

   int num = 0;
   int index;
   for (index = skipIndex; index < strlen(digits); index++)
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

   num *= bol;

   return num;
}

int isLetter(char c)
{
   if ((c > 'a' && c < 'z') || (c > 'A' && c < 'Z'))
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

int get_symbol_index(int itmp, char input[])
{
   /*find the symbol index*/
   int i2;
   char symbole_name[20];
   int symbolen = jumpToEndOfWord(itmp, input) - itmp;
   for (i2 = 0; i2 < symbolen; i2++)
   {
      symbole_name[i2] = input[itmp];
      itmp++;
   }

   symbole_name[symbolen] = '\0';

   printf("%s", symbole_name);
   int j;
   int bool = FALSE;
   int tmpindex = -1;
   for (i2 = 0; i2 < (sizeof(symbols) / sizeof(symbols[0])); i2++)
   {

      for (j = 0; j < strlen(symbole_name); j++)
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

   return tmpindex;
}

int get_symbol_address(int itmp, char input[])
{
   /*find the symbol address*/

   int tmpindex = get_symbol_index(itmp, input);
   int symbol_address = symbols[tmpindex].address;
   return symbol_address;
}

/*helpers for debugging:*/

int isWordFromLettersOnly(char word[])
{
   int i = 0;
   int len = strlen(word);

   while (isLetter(word[i]) && i < len)
   {
      i++;
   }

   if (i == len)
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

int isValidRegister(int i, char input[])
{
   if (input[i] == '@' && input[i + 1] == 'r' && input[i + 2] >= '0' && input[i + 2] < '8')
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

int isCommaBetweenWords(int i, char input[])
{
   if (isLetter(input[i]))
   {
      return FALSE;
   }

   i = skipBlank(i, input);

   if (input[i] == ',')
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

int isValidWord(int i, char input[])
{
   int isR = isValidRegister(i, input);
   int isSymb = isSymbol(i, input);
   int isDt = isData(i, input);
   int isStr = isString(i, input);
   int isEorE = isExternOrEntry(i, input);

   int iopcode = isOpCode(i, input);

   int itmp = i;

   i = jumpToEndOfWord(i, input);

   int wordLen = i - itmp;
   char word[wordLen];
   int itmp2;
   for (itmp2 = 0; itmp2 < wordLen; itmp2++)
   {
      word[itmp2] = input[itmp];
      itmp++;
   }

   int isNum = TRUE;

   for (itmp2 = 0; itmp2 < wordLen; itmp2++)
   {
      if (!isDigit(word[itmp2]))
      {
         isNum = FALSE;
      }
   }

   if (isLetter(word[0]) && word[wordLen] != ':' && iopcode == -1 && !isNum && !isR && !isSymb && !isDt && !isStr && !isEorE)
   {
      return FALSE;
   }
   else
   {
      return TRUE;
   }
}

int isSpaceAndOpcodeOrLabelDefAfterLastOperand(int i, char input[])
{

   i = skipBlank(i, input);

   if (i >= strlen(input) - 1 || input[i] == EOF)
   {
      return TRUE;
   }

   int iopcode = isOpCode(i, input);

   int itmp = i;

   i = jumpToEndOfWord(i, input);

   int wordLen = i - itmp;
   char word[wordLen];
   int itmp2;
   for (itmp2 = 0; itmp2 < wordLen; itmp2++)
   {
      word[itmp2] = input[itmp];
      itmp++;
   }

   if (iopcode != -1)
   {

      /*opcode case*/
      return TRUE;
   }
   else
   {
      if (word[wordLen - 1] == ':')
      {

         /*label case*/
         return TRUE;
      }
      else
      {

         printf("%c", ' ');
         printf("%s", "not opcode after operands");
         return FALSE;
      }
   }
}

/*to do: add special decoding cases for cmp and lea opcodes*/

int main()
{

   printf("%s", "start main: ");

   char *inputText = readFileToString("ps.as");
   /*input index*/
   int i = 0;

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

   /*create symbols*/
   i = 0;

   /*memory index*/
   int mi = 100;

   while (i < strlen(newInput2))
   {
      printf("%c", ' ');
      i = skipBlank(i, newInput2);
      int wasDataCase = FALSE;

      if (isExternOrEntry(i, newInput2))
      {
         i = jumpToEndOfWord(i, newInput2);
         i = skipBlank(i, newInput2);

         i = jumpToEndOfWord(i, newInput2);
         i = skipBlank(i, newInput2);
      }
      else
      {

         if (isSymbol(i, newInput2))
         {

            int cnt = i;
            while (newInput2[cnt] != ' ')
            {
               cnt++;
            }

            int j;
            /*cnt - i => the length of the symbol word*/
            for (j = 0; j < cnt - i - 1; j++)
            {
               symbols[indexSymbols].name[j] = newInput2[i + j];
            }

            symbols[indexSymbols].address = mi;
            indexSymbols++;
         }
         else
         {
            printf("%c", ' ');
            if (isString(i, newInput2))
            {

               /*get to string word*/
               i = jumpToEndOfWord(i, newInput2);
               i = skipBlank(i, newInput2);
               /*skip "*/
               i += 3;

               /*get string length*/
               int len = jumpToEndOfWord(i, newInput2) - 2 - i;

               /*count the memory for the string*/
               mi += len;
               printf("%d", len);
               printf("%s", " - len ");
            }
            else
            {
               printf("%c", ' ');

               if (isData(i, newInput2))
               {

                  /*skip .data word */
                  i = jumpToEndOfWord(i, newInput2);
                  i = skipBlank(i, newInput2);

                  /*get all integers*/
                  int bol = TRUE;

                  while (bol)
                  {
                     /*to fix*/
                     if (isDigit(newInput2[i]))
                     {
                        /*count the memory for the numbers*/
                        mi++;
                     }
                     while (isDigit(newInput2[i]))
                     {
                        i++;
                     }

                     i = skipBlank(i, newInput2);
                     if (inputText[i] == ',')
                     {
                        i++;
                        i = skipBlank(i, newInput2);
                     }
                     else
                     {
                        bol = FALSE;
                        wasDataCase = TRUE;
                     }
                  }
               }
               else
               {
                  if (!is_r_to_r_case(i, newInput2))
                  {
                     mi++;
                  }
               }
            }
         }

         if (!wasDataCase)
         {
            i = jumpToEndOfWord(i, newInput2);
         }
         wasDataCase = FALSE;
      }
   }

   i = 0;
   mi = 0;

   /*for testing*/
   int j;
   for (j = 0; j < (sizeof(symbols) / sizeof(symbols[0])); j++)
   {
      printf("%s", (symbols[j].name));
      printf("%d", symbols[j].address);
      printf("%s", "/");
   }
   indexSymbols = 0;

   /*to fix:*/
   /*define if symbol is external or not*/

   /*first define any symbol to not external by default*/
   for (j = 0; j < (sizeof(symbols) / sizeof(symbols[0])); j++)
   {
      symbols[j].isExtern = FALSE;
   }

   i = 0;

   while (i < strlen(newInput2))
   {
      printf("%c", ' ');
      i = skipBlank(i, newInput2);

      if (isExtern(i, newInput2))
      {
         printf("%s", "extern case");

         /*jump after extern commend*/
         i = jumpToEndOfWord(i, newInput2);
         i = skipBlank(i, newInput2);

         int tmpindex = get_symbol_index(i, newInput2);
         printf("%d", tmpindex);

         if (tmpindex != -1)
         {
            symbols[tmpindex].isExtern = TRUE;
         }
      }

      printf("%c", ' ');

      if (isEntry(i, newInput2))
      {
         printf("%s", "entry case");

         /*jump after entry commend*/
         i = jumpToEndOfWord(i, newInput2);
         i = skipBlank(i, newInput2);

         int tmpindex = get_symbol_index(i, newInput2);
         printf("%d", tmpindex);

         if (tmpindex != -1)
         {
            symbols[tmpindex].isEntry = TRUE;
         }
      }

      i = jumpToEndOfWord(i, newInput2);
   }

   /* until here its first level of the compliler */

   // /*until here summery:
   // we created array of symbol with there names and addresses
   // and we created macros  instances and replaced the instance
   // with the insider code*/

   // /*new code here****************************************** */

   // /* convert newinput to binary and check for errors: */

   i = 0;

   int d_code[1000][12];
   int dindex = 100;
   int isError = FALSE;

   int cntCommends = 0;
   int cntData = 0;

   while (i < strlen(newInput2))
   {
      printf("%c", ' ');
      i = skipBlank(i, newInput2);

      if (isExternOrEntry(i, newInput2))
      {
         /*count the first operand and data of data*/
         cntCommends++;
         cntData++;

         i = jumpToEndOfWord(i, newInput2);
         i = skipBlank(i, newInput2);

         i = jumpToEndOfWord(i, newInput2);
         i = skipBlank(i, newInput2);
      }

      printf("%c", ' ');

      /*debug*/

      if (!isValidWord(i, newInput2))
      {
         isError = TRUE;
      }

      /*convert to binary*/
      int iopcode = isOpCode(i, newInput2);
      if (iopcode != -1)
      {

         d_code[dindex][0] = 0;
         d_code[dindex][1] = 0;

         /*opcode case*/
         int *binaryopcode = decimalToBinary(iopcode, 4);

         d_code[dindex][5] = binaryopcode[0];
         d_code[dindex][6] = binaryopcode[1];
         d_code[dindex][7] = binaryopcode[2];
         d_code[dindex][8] = binaryopcode[3];

         /*skip opcode word*/
         i = jumpToEndOfWord(i, newInput2);
         i = skipBlank(i, newInput2);

         int opcode_group = get_opcode_group(iopcode);
         if (opcode_group == 1)
         {
            /*count for 2 operands*/
            cntCommends += 2;

            /* to do: check here for errors */
            if (isDigit(newInput2[i]))
            {
               /*count for data*/
               cntData++;

               int *binary_opcode = decimalToBinary(iopcode, 4);

               int itmp;
               int itmp2 = 0;

               for (itmp = 5; itmp < 9; itmp++)
               {

                  d_code[dindex][itmp] = binary_opcode[itmp2];

                  itmp2++;
               }

               d_code[dindex][9] = 1;
               d_code[dindex][10] = 0;
               d_code[dindex][11] = 0;

               dindex++;

               char strnum[10];
               itmp = 0;
               while (isDigit(newInput2[i]))
               {
                  strnum[itmp] = newInput2[i];
                  itmp++;
                  i++;
               }

               int n = toInt(strnum);
               int *num = decimalToBinary(n, 10);

               itmp = 0;
               itmp2 = 2;

               for (itmp = 0; itmp < 10; itmp++)
               {
                  d_code[dindex][itmp2] = num[itmp];
               }

               /*skip the comma letter and get to next operand*/
               i = skipBlank(i, newInput2);
               i++;
               i = skipBlank(i, newInput2);

               /* distinguish the decoding for the opcode operand in sibits 2 - 4
               if the third operand is a regiter or a label */
               dindex--;
               if (newInput2[i] == '@')
               {
                  if (!isValidRegister(i, newInput2))
                  {
                     printf("%s", "unvalid register");
                     isError = TRUE;
                  }

                  /*register case*/
                  d_code[dindex][2] = 1;
                  d_code[dindex][3] = 0;
                  d_code[dindex][4] = 1;

                  /*get to the third operand*/
                  dindex += 2;

                  i += 2;
                  int rnum = newInput2[i] - '0';

                  itmp = 0;
                  for (itmp = 0; itmp < 7; itmp++)
                  {
                     d_code[dindex][itmp] = 0;
                  }

                  int *binarynum = decimalToBinary(rnum, 4);

                  itmp2 = 0;
                  for (itmp = 7; itmp < 12; itmp++)
                  {
                     d_code[dindex][itmp] = binarynum[itmp2];
                  }
               }
               else
               {
                  /*label case*/
                  d_code[dindex][2] = 1;
                  d_code[dindex][3] = 1;
                  d_code[dindex][4] = 0;

                  /*get to the third operand*/
                  dindex += 2;
                  int itmp = i;
                  i = jumpToEndOfWord(i, newInput2);
                  int labelen = i - itmp;
                  char label[labelen];

                  for (itmp2 = 0; itmp2 < labelen; itmp2++)
                  {
                     label[itmp2] = newInput2[itmp];
                     itmp++;
                  }

                  int sindex = getSymbolIndex(label);
                  int *num = decimalToBinary(symbols[sindex].address, 10);

                  if (symbols[sindex].isExtern)
                  {
                     d_code[dindex][0] = 1;
                     d_code[dindex][1] = 0;
                  }
                  else
                  {
                     d_code[dindex][0] = 0;
                     d_code[dindex][1] = 1;
                  }

                  itmp = 2;
                  itmp2 = 0;

                  for (itmp = 2; itmp < 12; itmp++)
                  {
                     d_code[dindex][itmp] = num[itmp2];
                     itmp2++;
                  }
               }
            }

            /*if the first operand is not a number
            check if it is a register or label*/

            /*if the first operand is a register*/
            if (newInput2[i] == '@')
            {
               /*debug*/
               if (!isValidRegister(i, newInput2))
               {
                  printf("%s", "unvalid register");
                  isError = TRUE;
               }

               i += 2;
               int rnum1 = newInput2[i] - '0';
               /*get after register*/
               i++;

               /*debug*/
               if (!isCommaBetweenWords(i, newInput2))
               {
                  printf("%s", "unvalid syntax");
               }

               i = skipBlank(i, newInput2);
               /*skip the comma letter*/
               i++;

               i = skipBlank(i, newInput2);

               /*if the scond operand is a register*/
               if (newInput2[i] == '@')
               {
                  printf("%s", "got to register to register case");

                  /*debug*/
                  if (!isValidRegister(i, newInput2))
                  {
                     printf("%s", "unvalid register");
                     isError = TRUE;
                  }

                  i += 2;
                  int rnum2 = newInput2[i] - '0';
                  /*get after register*/
                  i++;

                  /*register to register case*/
                  /*register case*/
                  d_code[dindex][2] = 1;
                  d_code[dindex][3] = 0;
                  d_code[dindex][4] = 1;

                  /*register case*/
                  d_code[dindex][9] = 1;
                  d_code[dindex][10] = 0;
                  d_code[dindex][11] = 1;

                  /*dcoding the registers*/
                  dindex++;
                  d_code[dindex][0] = 0;
                  d_code[dindex][1] = 0;

                  int *binaryRNum1 = decimalToBinary(rnum1, 5);
                  int *binaryRNum2 = decimalToBinary(rnum2, 5);

                  int itmp;
                  int itmp2 = 0;

                  for (itmp = 2; itmp < 12; itmp++)
                  {
                     if (itmp2 == 5)
                     {
                        itmp2 = 0;
                     }

                     if (itmp < 7)
                     {
                        /*decode destination register operand*/
                        d_code[dindex][itmp] = binaryRNum2[itmp2];
                     }
                     else
                     {
                        /*dcode source register o0erand*/
                        d_code[dindex][itmp] = binaryRNum1[itmp2];
                     }

                     itmp2++;
                  }
               }
               else
               {
                  printf("%s", "got to regsiter to label case");

                  /*if the scond operand is a label*/
                  /*get label string*/
                  int itmp = i;
                  i = jumpToEndOfWord(i, newInput2);

                  int labelen = i - itmp;
                  char label[labelen];
                  int itmp2;
                  for (itmp2 = 0; itmp2 < labelen; itmp2++)
                  {
                     label[itmp2] = newInput2[itmp];
                     itmp++;
                  }

                  /*decode register to label case*/
                  d_code[dindex][0] = 0;
                  d_code[dindex][1] = 0;

                  /*register case*/
                  d_code[dindex][9] = 1;
                  d_code[dindex][10] = 0;
                  d_code[dindex][11] = 1;

                  /*label case*/
                  d_code[dindex][2] = 1;
                  d_code[dindex][3] = 1;
                  d_code[dindex][4] = 0;

                  /*decode the register operand*/
                  dindex++;
                  itmp2 = 0;
                  int *binaryRNum1 = decimalToBinary(rnum1, 5);
                  for (itmp = 0; itmp < 12; itmp++)
                  {
                     if (itmp > 6 && itmp < 12)
                     {
                        d_code[dindex][itmp] = binaryRNum1[itmp2];
                        itmp2++;
                     }
                     else
                     {
                        d_code[dindex][itmp] = 0;
                     }
                  }

                  /*decode the label operand*/
                  dindex++;

                  int sindex = getSymbolIndex(label);
                  int *num = decimalToBinary(symbols[sindex].address, 10);

                  if (symbols[sindex].isExtern)
                  {
                     d_code[dindex][0] = 1;
                     d_code[dindex][1] = 0;
                  }
                  else
                  {

                     d_code[dindex][0] = 0;
                     d_code[dindex][1] = 1;
                  }

                  itmp = 2;
                  itmp2 = 0;

                  for (itmp = 2; itmp < 12; itmp++)
                  {
                     d_code[dindex][itmp] = num[itmp2];
                     itmp2++;
                  }
               }
            }
            else
            {
               /*the sccond operand is a label*/
               printf("%s", "got to label to registar case");

               /*label case*/
               d_code[dindex][9] = 1;
               d_code[dindex][10] = 1;
               d_code[dindex][11] = 0;

               /*get label string*/
               int itmp = i;
               i = jumpToEndOfWord(i, newInput2);
               i--;

               /*check for comma*/
               if (newInput2[i] == ',')
               {
                  i--;
               }
               int labelen = i - itmp;
               char label[labelen];
               int itmp2;
               for (itmp2 = 0; itmp2 < labelen; itmp2++)
               {
                  label[itmp2] = newInput2[itmp];
                  itmp++;
               }

               /*skip comma and get to the 3 operand*/
               i = skipBlank(i, newInput2);
               i++;
               i = skipBlank(i, newInput2);

               /*if the 3 opeand is a register*/
               if (newInput2[i] == '@')
               {

                  /*register case*/
                  d_code[dindex][2] = 1;
                  d_code[dindex][3] = 0;
                  d_code[dindex][4] = 1;

                  /*decode the label case*/
                  dindex++;

                  int sindex = getSymbolIndex(label);
                  int *num = decimalToBinary(symbols[sindex].address, 10);

                  if (symbols[sindex].isExtern)
                  {
                     d_code[dindex][0] = 1;
                     d_code[dindex][1] = 0;
                  }
                  else
                  {
                     d_code[dindex][0] = 0;
                     d_code[dindex][1] = 1;
                  }

                  itmp = 2;
                  itmp2 = 0;

                  for (itmp = 2; itmp < 12; itmp++)
                  {
                     d_code[dindex][itmp] = num[itmp2];
                     itmp2++;
                  }

                  /*decode register case*/
                  dindex++;
                  /*get register num*/
                  i += 2;
                  int rnum = newInput2[i] - '0';
                  int *binaryRNum = decimalToBinary(rnum, 4);

                  d_code[dindex][2] = binaryRNum[0];
                  d_code[dindex][3] = binaryRNum[1];
                  d_code[dindex][4] = binaryRNum[2];
                  d_code[dindex][5] = binaryRNum[3];
                  d_code[dindex][6] = binaryRNum[4];

                  d_code[dindex][7] = 0;
                  d_code[dindex][8] = 0;
                  d_code[dindex][9] = 0;
                  d_code[dindex][10] = 0;
                  d_code[dindex][11] = 0;
               }
            }
         }

         if (opcode_group == 2)
         {
            /*count for 1 operand*/
            cntCommends += 1;

            d_code[dindex][0] = 0;
            d_code[dindex][1] = 0;

            d_code[dindex][9] = 0;
            d_code[dindex][10] = 0;
            d_code[dindex][11] = 0;

            /*decode register case*/
            if (newInput2[i] == '@')
            {

               d_code[dindex][2] = 1;
               d_code[dindex][3] = 0;
               d_code[dindex][4] = 1;

               /*decode the register*/
               dindex++;

               i += 2;
               int rnum = newInput2[i] - '0';
               /*get after register*/
               i++;

               int *binaryRnum = decimalToBinary(rnum, 5);
               int itmp = 0;
               int itmp2 = 0;

               for (itmp = 0; itmp < 12; itmp++)
               {
                  if (itmp > 1 && itmp < 7)
                  {
                     d_code[dindex][itmp] = binaryRnum[itmp2];
                     itmp2++;
                  }
                  else
                  {
                     d_code[dindex][itmp] = 0;
                  }
               }
            }
            else
            {

               if (isDigit(newInput2[i]))
               {
                  /*count for data*/
                  cntData++;

                  /* number case*/

                  d_code[dindex][2] = 1;
                  d_code[dindex][3] = 0;
                  d_code[dindex][4] = 0;

                  /*decode the number*/
                  dindex++;

                  d_code[dindex][0] = 0;
                  d_code[dindex][1] = 0;

                  char strnum[10];
                  int itmp = 0;
                  while (isDigit(newInput2[i]))
                  {
                     strnum[itmp] = newInput2[i];
                     itmp++;
                     i++;
                  }

                  int num = toInt(strnum);

                  int *binaryNum = decimalToBinary(num, 10);

                  itmp = 2;
                  int itmp2 = 0;

                  for (itmp = 2; itmp < 12; itmp++)
                  {
                     d_code[dindex][itmp] = binaryNum[itmp2];
                     itmp2++;
                  }
               }
               else
               {
                  /*decode label case*/
                  d_code[dindex][2] = 1;
                  d_code[dindex][3] = 1;
                  d_code[dindex][4] = 0;

                  /*dcode the label*/
                  dindex++;

                  d_code[dindex][0] = 0;
                  d_code[dindex][1] = 1;

                  /*get label string*/
                  int itmp = i;
                  i = jumpToEndOfWord(i, newInput2);
                  int labelen = i - itmp;

                  labelen++;

                  char label[labelen];
                  int itmp2;
                  for (itmp2 = 0; itmp2 < labelen - 1; itmp2++)
                  {
                     label[itmp2] = newInput2[itmp];
                     itmp++;
                  }

                  label[labelen - 1] = '\0';

                  int sindex = getSymbolIndex(label);
                  int *num = decimalToBinary(symbols[sindex].address, 10);

                  if (symbols[sindex].isExtern)
                  {
                     d_code[dindex][0] = 1;
                     d_code[dindex][1] = 0;
                  }
                  else
                  {
                     d_code[dindex][0] = 0;
                     d_code[dindex][1] = 1;
                  }

                  itmp = 2;
                  itmp2 = 0;

                  for (itmp = 2; itmp < 12; itmp++)
                  {
                     d_code[dindex][itmp] = num[itmp2];
                     itmp2++;
                  }
               }
            }
         }

         if (opcode_group == 3)
         {
            d_code[dindex][0] = 0;
            d_code[dindex][1] = 0;

            d_code[dindex][2] = 0;
            d_code[dindex][3] = 0;
            d_code[dindex][4] = 0;

            int *binaryOpcode = decimalToBinary(iopcode, 4);

            d_code[dindex][5] = binaryOpcode[0];
            d_code[dindex][6] = binaryOpcode[1];
            d_code[dindex][7] = binaryOpcode[2];
            d_code[dindex][8] = binaryOpcode[3];

            d_code[dindex][9] = 0;
            d_code[dindex][10] = 0;
            d_code[dindex][11] = 0;

            int i = jumpToEndOfWord(i, newInput2);
         }
         dindex++;

         /*debug*/

         if (!isSpaceAndOpcodeOrLabelDefAfterLastOperand(i, newInput2))
         {
            printf("%s", " syntax error");
            /*return -1;*/
         }
      }

      if (isData(i, newInput2))
      {
         /*count the first operand and data of .data*/
         cntCommends++;
         cntData++;

         printf("%s", "got to .data");

         /*skip .data word */
         i = jumpToEndOfWord(i, newInput2);
         i = skipBlank(i, newInput2);

         /*get all integers*/
         int bol = TRUE;

         while (bol)
         {
            char strnum[10];
            int itmp = 0;

            while (isDigit(newInput2[i]))
            {

               strnum[itmp] = newInput2[i];
               itmp++;
               i++;
            }

            strnum[itmp] = '\0';

            int n = toInt(strnum);

            int *num = decimalToBinary(n, 12);

            itmp = 0;

            for (itmp = 0; itmp < 12; itmp++)
            {
               d_code[dindex][itmp] = num[itmp];
            }

            i = skipBlank(i, newInput2);
            if (newInput2[i] == ',')
            {
               /* count for data and operands of .data */
               cntCommends++;
               cntData++;

               i++;
               i = skipBlank(i, newInput2);
               dindex++;
            }
            else
            {
               bol = FALSE;
            }
            printf("%c", ' ');
         }
         dindex++;

         /*debug*/

         if (!isSpaceAndOpcodeOrLabelDefAfterLastOperand(i, newInput2))
         {
            printf("%s", " syntax error");
            /*return -1;*/
         }
      }

      printf("%c", ' ');

      if (isString(i, newInput2))
      {
         /*count for .string operands*/
         cntCommends++;

         printf("%s", "got to .string");
         /*skip .string*/
         i = jumpToEndOfWord(i, newInput2);
         i = skipBlank(i, newInput2);
         /*skip "*/
         i += 3;

         /*get string length*/
         int len = jumpToEndOfWord(i, newInput2) - 2 - i;

         char str[len];
         int itmp = 0;
         int itmp2 = 0;

         /*put string into str*/
         for (itmp = 0; itmp < len - 1; itmp++)
         {
            /*count for .string data*/
            cntData++;

            str[itmp] = newInput2[i];
            i++;
         }

         str[len - 1] = '\0';

         /* get after string to blank*/
         i += 2;

         /*decode string*/
         for (itmp = 0; itmp < len; itmp++)
         {
            int *ascii = decimalToBinary(str[itmp], 12);

            for (itmp2 = 0; itmp2 < 12; itmp2++)
            {
               d_code[dindex][itmp2] = ascii[itmp2];
            }

            dindex++;
         }

         i = jumpToEndOfWord(i, newInput2);

         if (!isSpaceAndOpcodeOrLabelDefAfterLastOperand(i, newInput2))
         {
            printf("%s", " syntax error");
            /*return -1;*/
         }
      }

      printf("%c", ' ');

      i = jumpToEndOfWord(i, newInput2);
   }

   /*check for errors*/
   if (isError)
   {
      printf("%s", "syntax error in input file.");
      /*return -1;*/
   }

   /* convert d_code binary array ro base64 letters:*/
   int y;
   int x;
   int z = 11;

   /*revers d_code array*/
   for (y = 100; y < dindex; y++)
   {
      int start = 0;
      int end = 11;
      int temp;
      while (start < end)
      {
         temp = d_code[y][start];
         d_code[y][start] = d_code[y][end];
         d_code[y][end] = temp;
         start++;
         end--;
      }
      printf("%c", ' ');
   }

   for (y = 100; y < dindex; y++)
   {
      for (x = 0; x < 12; x++)
      {
         /*for testing*/
         printf("%d", d_code[y][x]);
      }

      printf("%c", ' ');
   }

   char decodingResultStr[dindex][2];

   for (y = 100; y < dindex; y++)
   {
      char *strTmp = convertArrayToBase64(d_code[y]);

      decodingResultStr[y][0] = strTmp[0];
      decodingResultStr[y][1] = strTmp[1];

      /*for testing*/
      printf("%s", decodingResultStr[y]);
      printf("%c", ' ');
   }

   int slen = dindex;
   char outputStr[slen];
   int indexTmp = 0;

   char cnt1[10];
   // /*convert int to string*/
   sprintf(cnt1, "%d", cntCommends);

   char cnt2[10];
   // /*convert int to string*/
   sprintf(cnt2, "%d", cntData);

   for (y = 100; y < dindex; y++)
   {
      outputStr[indexTmp] = decodingResultStr[y][0];
      indexTmp++;
      outputStr[indexTmp] = decodingResultStr[y][1];
      indexTmp++;
      outputStr[indexTmp] = '\n';
      indexTmp++;
   }
   // Pointer to the file
   FILE *file;

   // Open a file in write mode. If the file doesn't exist, it will be created.
   // If it exists, its contents will be overwritten.
   file = fopen("ps.ob", "w");

   // Check if the file was opened successfully
   if (file == NULL)
   {
      printf("Error opening the file!\n");
      return 1; // Return an error code
   }

   // Write some text to the file
   fprintf(file, cnt1);
   fprintf(file, " ");
   fprintf(file, cnt2);
   fprintf(file, "\n");
   fprintf(file, outputStr);

   // Close the file
   fclose(file);

   printf("Data written to ps.ob\n");

   // Pointer to the file
   FILE *entfile;

   // Open a file in write mode. If the file doesn't exist, it will be created.
   // If it exists, its contents will be overwritten.
   entfile = fopen("ps.ent", "w");
   // extfile = fopen("ps.ext", "w");

   // Check if the file was opened successfully
   if (entfile == NULL)
   {
      printf("Error opening the file!\n");
      return 1; // Return an error code
   }

   for (i = 0; i < (sizeof(symbols) / sizeof(symbols[0])); i++)
   {
      if (symbols[i].isEntry)
      {
         printf("%s", "entry case");
         // Write some text to the file
         fprintf(entfile, symbols[i].name);
         fprintf(entfile, " ");
         char address[10];
         /*convert int to string*/
         sprintf(address, "%d", symbols[i].address);
         fprintf(entfile, address);
         fprintf(entfile, "\n");
      }
   }

   // Close the files
   fclose(entfile);

   printf("Data written ent file\n");

   // Pointer to the file
   FILE *extfile;

   // Open a file in write mode. If the file doesn't exist, it will be created.
   // If it exists, its contents will be overwritten.
   extfile = fopen("ps.ext", "w");
   // extfile = fopen("ps.ext", "w");

   // Check if the file was opened successfully
   if (extfile == NULL)
   {
      printf("Error opening the file!\n");
      return 1; // Return an error code
   }

   for (i = 0; i < (sizeof(symbols) / sizeof(symbols[0])); i++)
   {
      if (symbols[i].isExtern)
      {
         printf("%s", "extern case");
         // Write some text to the file
         fprintf(extfile, symbols[i].name);
         fprintf(extfile, " ");
         char address[10];
         // /*convert int to string*/
         sprintf(address, "%d", symbols[i].address);
         fprintf(extfile, address);
         fprintf(extfile, "\n");
      }
   }

   // Close the files
   fclose(extfile);

   printf("Data written ext file\n");

   return 0;
}
