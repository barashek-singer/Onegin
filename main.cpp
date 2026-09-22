#include "TXLib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct TextLine{
    char *start, *end;
};

struct Text{
    char* buf;
    size_t size;
    size_t lines_count;
    TextLine* lines;
};

const char* INPUT_TEXT_FILE = "beautiful_text.txt";
const char* OUTPUT_TEXT_FILE = "mytexts.txt";

void FreeText(Text* text);
int SplitText(Text* text);
int PrintTextToFile(const Text* text, const char* name_file, const char* access_mode);
int DuplicateTextFromFile(Text* text, const char* name_file);
int StrBackCmp(const void* ptr_str1, const void* ptr_str2);
int StrReverseCmp(const void* ptr_str1, const void* ptr_str2);
int StrForwardCmp(const void* str1, const void* str2);
void* GetPtr(const void* data, size_t size_el, size_t ind);
void Swap(void* a, void* b, size_t size_el);
size_t Partition(void* data, size_t size_el, size_t left, size_t right, int (*CompFunc)(const void*, const void*));
void QuickSort(void* data, size_t size_el, size_t left, size_t right, int (*CompFunc)(const void*, const void*));
void BubbleSort(void* data, size_t size_arr, size_t size_el, int (*CompFunc)(const void* a, const void* b));
void PrintStringArray(const TextLine* text_lines, size_t size);

int main(){
    Text text = {0};

    if (DuplicateTextFromFile(&text, INPUT_TEXT_FILE))
        return 0;

    if (SplitText(&text))
        return 0;

    qsort(text.lines, text.lines_count, sizeof(TextLine), &StrForwardCmp);

    if (PrintTextToFile(&text, OUTPUT_TEXT_FILE, "w"))
        return 0;

    QuickSort(text.lines, sizeof(TextLine), 0, text.lines_count - 1, &StrReverseCmp);

    if (PrintTextToFile(&text, OUTPUT_TEXT_FILE, "a"))
        return 0;

    BubbleSort(text.lines, text.lines_count, sizeof(TextLine), &StrBackCmp);

    if (PrintTextToFile(&text, OUTPUT_TEXT_FILE, "a"))
        return 0;

    FreeText(&text);

    return 0;
}

void FreeText(Text* text){ //
    assert(text);

    free(text->lines);
    free(text->buf);
}

int SplitText(Text* text){
    assert(text);

    for (size_t cur_ch = 0; cur_ch < text->size; cur_ch++){
        if (*(text->buf + cur_ch) == '\n'){
            ++text->lines_count;
            *(text->buf + cur_ch) = '\0';
        }

        if (*(text->buf + cur_ch) == '\r')
            *(text->buf + cur_ch) = '\0';
    }

    text->lines = (TextLine*)calloc(text->lines_count, sizeof(TextLine));
    if (text->lines == NULL){
        printf("Проблемы с динамической памятью");
        return -3;
    }

    size_t cur_ch = 0;

    for (size_t i = 0; i < text->lines_count; i++){
        text->lines[i].start = text->buf + cur_ch;
        while (*(text->buf + cur_ch) != '\0')
            ++cur_ch;

        text->lines[i].end = text->buf + cur_ch;
        cur_ch += 2; //\r\n - 2 символа
    }

    return 0;
}

int PrintTextToFile(const Text* text, const char* name_file, const char* access_mode){
    assert(text && name_file && access_mode);

    FILE* fp = fopen(name_file, access_mode);

    if (fp == NULL){
        perror("Проблема с открытием файла");
        return -1;
    }

    fprintf(fp, "-----------------------------\n");

    // printf()

    for (size_t i = 0; i < text->lines_count; i++)
        fprintf(fp, "%s\n", text->lines[i].start);

    fprintf(fp, "-----------------------------\n");

    if (fclose(fp) == EOF){
        perror("Проблема с закрытием файла");
        return -2;
    }

    return 0;
}

int DuplicateTextFromFile(Text* text, const char* name_file){ //
    assert(text && name_file);

    FILE* fp = fopen(name_file, "rb");
    if (fp == NULL){
        perror("Проблема при открытии файла");
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    text->size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    text->buf = (char*)calloc(text->size + 1, sizeof(char));

    if (text->buf == NULL){
        if (fclose(fp) == EOF){
            perror("Проблема с закрытием файла");
            return -2;
        }

        printf("Проблемы с динамической памятью");
        return -3;
    }

    fread(text->buf, 1, text->size, fp);

    return 0;
}

int StrBackCmp(const void* ptr_str1, const void* ptr_str2){
    assert(ptr_str1 && ptr_str2);

    if (size_t(((const TextLine*)ptr_str1)->start) < size_t(((const TextLine*)ptr_str2)->start))
        return -1;

    if (size_t(((const TextLine*)ptr_str1)->start) > size_t(((const TextLine*)ptr_str2)->start))
        return 1;

    return 0;
}

int StrReverseCmp(const void* ptr_text_line1, const void* ptr_text_line2){
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

        if (str1 >= ptr_str1 && str2 >= ptr_str2 && tolower(*str1) != tolower(*str2))
            return tolower(*str1) - tolower(*str2);

        if (str1 >= ptr_str1 && str2 >= ptr_str2){
            --str1;
            --str2;
        }
    }

    if (str2 >= ptr_str2)
        return -*str2;

    if (str1 >= ptr_str1)
        return *str1;

    return 0;
}

int StrForwardCmp(const void* ptr_text_line1, const void* ptr_text_line2){
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

void Swap(void* a, void* b, size_t size_el){
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
