#include "TXLib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct TextLine{
    char *start, *end;
};

const size_t MAX_STRINGS_COUNT = 20;
const size_t MAX_SYMBOLS_COUNT = 50;
const char* INPUT_TEXT_FILE = "beautiful_text.txt";
const char* OUTPUT_TEXT_FILE = "mytexts.txt";

int InputTextFromFile(char text[MAX_STRINGS_COUNT][MAX_SYMBOLS_COUNT], const char* name_file);
int StrBackCmp(const void* ptr_str1, const void* ptr_str2);
int StrReverseCmp(const void* ptr_str1, const void* ptr_str2);
int StrForwardCmp(const void* str1, const void* str2);
void* GetPtr(const void* data, size_t size_el, size_t ind);
void Swap(void* a, void* b, size_t size_el);
size_t Partition(void* data, size_t size_el, size_t left, size_t right, int (*CompFunc)(const void*, const void*));
void QuickSort(void* data, size_t size_el, size_t left, size_t right, int (*CompFunc)(const void*, const void*));
void BubbleSort(void* data, size_t size_arr, size_t size_el, int (*CompFunc)(const void* a, const void* b));
void PrintStringArray(const TextLine* text_lines, size_t size);

int main(){ //после ебаной стирки у мен€ заработал код, очень многи варнингов и ошибок было 3:00 (реально 3:00)
    char text[MAX_STRINGS_COUNT][MAX_SYMBOLS_COUNT] = {};

    int nstrings = InputTextFromFile(text, INPUT_TEXT_FILE);

    if (nstrings < 0){
        perror("Ќу чота с файлом");
        return 0;
    }

    TextLine text_lines[MAX_STRINGS_COUNT] = {};

    for (size_t i = 0; i < (size_t)nstrings; ++i){
        text_lines[i].end = text_lines[i].start = text[i];
        while (*text_lines[i].end != '\0')
            ++text_lines[i].end;
    }

    QuickSort(text_lines, sizeof(text_lines[0]), 0, nstrings - 1, &StrForwardCmp);

    // PrintTextToFile(text_lines, nstrings, OUTPUT_TEXT_FILE);
    PrintStringArray(text_lines, nstrings);

    printf("----------------------------------\n");

    qsort(text_lines, nstrings, sizeof(text_lines[0]), &StrReverseCmp);

    // PrintTextToFile(text_lines, nstrings, OUTPUT_TEXT_FILE);
    PrintStringArray(text_lines, nstrings);

    printf("----------------------------------\n");

    BubbleSort(text_lines, nstrings, sizeof(text_lines[0]), &StrBackCmp);

    // PrintTextToFile(text_lines, nstrings, OUTPUT_TEXT_FILE);
    PrintStringArray(text_lines, nstrings);
}

int InputTextFromFile(char text[MAX_STRINGS_COUNT][MAX_SYMBOLS_COUNT], const char* name_file){
    assert(text && name_file);

    FILE* fp = fopen(name_file, "r");

    if (fp == NULL)
        return -1;

    size_t nstrings = 0;

    while (fgets(text[nstrings], MAX_SYMBOLS_COUNT, fp) != NULL)
        ++nstrings;

    if (fclose(fp) == EOF)
        return -2;

    return (int)nstrings;
}

int StrBackCmp(const void* ptr_str1, const void* ptr_str2){ //0:26 гуд парти€ довольна (0:57 x2)
    assert(ptr_str1 && ptr_str2);

    if (size_t(((const TextLine*)ptr_str1)->start) < size_t(((const TextLine*)ptr_str2)->start))
        return -1;

    if (size_t(((const TextLine*)ptr_str1)->start) > size_t(((const TextLine*)ptr_str2)->start))
        return 1;

    return 0;
}

int StrReverseCmp(const void* ptr_text_line1, const void* ptr_text_line2){ //0:26 тут полнейший пиздец | 1:03 переделал на структуру, вроде норм
    assert(ptr_text_line1 && ptr_text_line2);

    const char* const ptr_str1 = ((const TextLine*)ptr_text_line1)->start;
    const char* const ptr_str2 = ((const TextLine*)ptr_text_line2)->start;

    const char* str1 = ((const TextLine*)ptr_text_line1)->end;
    const char* str2 = ((const TextLine*)ptr_text_line2)->end;

    while (str1 >= ptr_str1 && str2 >= ptr_str2){
        while (str1 >= ptr_str1 && !isalpha(*str1))
            --str1;

        while (str2 >= ptr_str2 && !isalpha(*str2))
            --str2;

        if (str1 >= ptr_str1 && str2 >= ptr_str2 && *str1 != *str2)
            return *str1 - *str2;

        if (str1 >= ptr_str1 && str2 >= ptr_str2){
            --str1;
            --str2;
        }
    }

    if (str2 >= ptr_str2)
        return *str2;

    if (str1 >= ptr_str1)
        return -*str1;

    return 0;
}

int StrForwardCmp(const void* ptr_text_line1, const void* ptr_text_line2){ //1:05 переделал на структуры
    assert(ptr_text_line1 && ptr_text_line2);

    const char* str1 = ((const TextLine*)ptr_text_line1)->start;
    const char* str2 = ((const TextLine*)ptr_text_line2)->start;

    while (*str1 != '\0' && *str2 != '\0'){
        while (!isalpha(*str1) && *str1 != '\0')
            ++str1;

        while (!isalpha(*str2) && *str2 != '\0')
            ++str2;

        if (tolower(*str1) - tolower(*str2) != 0)
            return tolower(*str1) - tolower(*str2);

        if (*str1 != '\0'){
            ++str1;
            ++str2;
        }
    }

    return 0;
}

void* GetPtr(const void* data, size_t size_el, size_t ind){
    assert(data);

    return (void*)((size_t)data + ind * size_el);
}

void Swap(void* a, void* b, size_t size_el){ //TODO opt
    assert(a && b);

    char* ptr_a = (char*)a;
    char* ptr_b = (char*)b;

    char temp = '\0';

    for (size_t i = 0; i < size_el; ++i) {
        temp = *(ptr_a + i);
        *(ptr_a + i) = *(ptr_b + i);
        *(ptr_b + i) = temp;
    }
}

size_t Partition(void* data, size_t size_el, size_t left, size_t right, int (*CompFunc)(const void*, const void*)){
    assert(data && CompFunc && left <= right);
    size_t i_pivot = left + (right - left) / 2;

    size_t i = left;
    size_t j = right;

    while (true) {
        while (CompFunc(GetPtr(data, size_el, i), GetPtr(data, size_el, i_pivot)) < 0) ++i;
        while (CompFunc(GetPtr(data, size_el, j), GetPtr(data, size_el, i_pivot)) > 0) --j;

        assert(left <= i && i <= right && left <= j && j <= right);

        if (i >= j)
            return j;

        Swap(GetPtr(data, size_el, i), GetPtr(data, size_el, j), size_el);

        if (i_pivot == i)
            i_pivot = j;

        else if (i_pivot == j)
            i_pivot = i;

        ++i;
        --j;
    }
}

void QuickSort(void* data, size_t size_el, size_t left, size_t right, int (*CompFunc)(const void*, const void*)){
    assert(data && CompFunc);

    if (right == left)
        return;

    size_t p = Partition(data, size_el, left, right, CompFunc);

    QuickSort(data, size_el, left, p, CompFunc);
    QuickSort(data, size_el, p + 1, right, CompFunc);
}

void BubbleSort(void* data, size_t size_arr, size_t size_el, int (*CompFunc)(const void* a, const void* b)){
    assert(data && CompFunc);

    bool sorted = false;

    do {
        sorted = true;

        for (size_t i = 0; i < size_arr - 1; i++){
            void* first = (void*)((size_t)data + i * size_el);
            void* second = (void*)((size_t)first + size_el);

            if ((*CompFunc)(first, second) > 0){
                sorted = false;
                Swap(first, second, size_el);
            }
        }
    } while (!sorted);
}

void PrintStringArray(const TextLine* text_lines, size_t size){
    assert(text_lines);

    for (size_t i = 0; i < size; i++)
        printf("[%zu]=%s", i, text_lines[i].start);
}

/*

*/
