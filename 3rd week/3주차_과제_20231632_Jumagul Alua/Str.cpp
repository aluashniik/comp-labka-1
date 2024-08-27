#include "Str.h"
#include <cstring>
#include <cstdlib>

Str::Str(int length) {
    len = length; // 길이 저장
    str = new char[len + 1];//len에 1더한만큼 str 동적할당
    str[0] = '\0';//문자열이 null로 끝나도록 할당
}

Str::Str(char* contents) {
    len = strlen(contents); // 문자열의 길이 구하기
    str = new char[len + 1]; //len 1더한만큼 str 동적할당
    strcpy(str, contents);//contents에 문자열을 복사
    str[len] = '\0';//문자열이 nulll로 끝나도록 할당
}

Str::~Str() {
    delete []str;// 메모리 해제
}

int Str::length() {
    return len;//길이 리턴
}

char *Str::contents() {
    return str;//문자열 리턴
}

int Str::compare(char *a) {
    return strcmp(str, a);//문자열과 문자열 포인터를 비교
}

int Str::compare(Str &a) {
    return (*this).compare(a);//객체와 비교;
}

void Str::operator=(char *a) {
    if (a == 0) { // 0인 경우 처리
        len = 0;
        delete []str;
                str = new char[1];
        str[0] = '\0';
        return;
    }
    int length_a = strlen(a);// 문자열의 길이 저장

    if (len < length_a) { // 현재 문자열의 길이보다 입젹받은 문자열의 길이가 작은 경우
                delete []str;//메모리 해제
                str = new char[length_a + 1];//세러운 메모리 할당
        }

    len = length_a;//길이 업데이트

    strcpy(str, a); // 문자열을 메모리에 복사
    str[len] = '\0';//문자열이 null로 끝나도록 설정
}

void Str::operator=(Str &a) {
    (*this) = a.contents();//다른 객체의 내요을 대입
}
