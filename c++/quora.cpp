#include <string>  
#include <stdlib.h>
#include <stdio.h>
#include "time.h"

void GetLen(FILE * fp, int &len);
void ChangeExtension(char *fname, char *newExt);
int StrReplace(char *target, const char *needle, const char *replacement, int len);
void CPPReplace(std::string &data);

int main(int c, char**v)
{
    FILE *fp = NULL;
    int len = 0;
    char *buf = NULL;
    char fname[256];
    time_t totalC = 0, totalCPP = 0;

    if (c < 2)
    {
        puts("I need filename(s), like this...");
        puts("fixcsv.exe file.txt");
        return 0;
    }

    for (int i = 1; i < c; i++)
    {
        fopen_s(&fp, v[i], "r");
        GetLen(fp, len);
        buf = (char*)calloc(1, len * 2);
        if (!buf)
        {
            puts("Wow! That file's too big for me. Out of memory!");
            return -1;
        }

        fread(buf, len, 1, fp);
        fclose(fp);
        std::string data = buf;
        time_t start, end;
        puts("Starting with std::string");
        start = clock();//predefined  function in c
        CPPReplace(data);
        end = clock();
        totalCPP += end - start;
        printf("CPP Runtime in mSecs: %lld\n", end - start);
        puts("Starting with C stdlib functions");
        start = clock();//predefined  function in c
        int result = StrReplace(buf, v[1], ",\n", len * 2);
        if (result)
        {
            puts("Something went wrong. I have to quit!");
            return -1;
        }
        end = clock();
        totalC += end - start;
        printf("C Runtime in mSecs: %lld\n", end - start);
        strcpy(fname, v[i]);
        ChangeExtension(fname, "csv");
        fopen_s(&fp, fname, "w");
        fwrite(buf, strlen(buf), 1, fp);
        fclose(fp);
    }
    printf("Total Runtimes\nCPP with std::string: %f\nC with stdlib: %f\n", (float)totalCPP / CLOCKS_PER_SEC, (float)totalC / CLOCKS_PER_SEC);
    printf("In this example, C was %f times faster than C++\n", (float)(totalCPP / totalC));
    puts("Press return to continue");
    getchar();
    return 0;
}

void CPPReplace(std::string &data)
{
    const std::string s = ",";
    const std::string t = ",\n";

    std::string::size_type n = 0;
    while ((n = data.find(s, n)) != std::string::npos)
    {
        data.replace(n, s.size(), t);
        n += t.size();
    }
}

void GetLen(FILE * fp, int &len)
{
    fseek(fp, 0L, SEEK_END);
    len = ftell(fp);
    rewind(fp);
}

void ChangeExtension(char *fname, char *newExt)
{
#define EVAL_MAX_LEN (300)

    size_t nLen;
    char szOut[EVAL_MAX_LEN] = { 0 };
    char szPath[EVAL_MAX_LEN];
    strcpy(szPath, fname);
    nLen = strlen(szPath);
    if ((nLen > 0) && (nLen < EVAL_MAX_LEN))
    {

        while (nLen)
        {
            if (szPath[nLen] == '.')
            {
                szPath[nLen] = '\0';
                break;
            }
            nLen--;
        }
        sprintf(szOut, "%s.%s", szPath, newExt);
        strcpy(fname, szOut);
    }
    return;
}

int StrReplace(char *target, const char *needle, const char *replacement, int len)
{
    char *buffer = (char*)calloc(1, len);
    if (!buffer)
    {
        puts("Wow! That file's too big for me. Out of memory!");
        return -1;
    }

    char *insert_point = &buffer[0];
    const char *tmp = target;
    size_t needle_len = strlen(needle);
    size_t repl_len = strlen(replacement);

    while (1)
    {
        const char *p = strstr(tmp, needle);
        if (p == NULL)
        {
            strcpy(insert_point, tmp);
            break;
        }
        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;
        memcpy(insert_point, replacement, repl_len);
        insert_point += repl_len;
        tmp = p + needle_len;
    }
    strcpy(target, buffer);
    delete(buffer);
    return 0;
}
