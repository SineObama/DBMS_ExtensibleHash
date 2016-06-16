#pragma once

#include "PageManager.h"

#define P 128

// ��ҳ���ʵ��������ļ��Ľӿڣ������ѷ�װ���ڣ�����ͬʱ���´����ļ��ķ�ʽ��
class DataManager
{

public:

    DataManager(const char *);
    ~DataManager();

    // ���ػ����е����ݵ�ַ
    char *read(index_t pid);
    // ��ҳidд���ļ�
    void write(char *, index_t pid, bool check = true);
    // ��ҳidд���ļ�
    //int write(index_t pid);

    std::fstream fs;
private:

    DataManager();

    // �Ӵ��̶�ȡҳ���û��������һҳ�������������ҳ������ҳ������0�Է�����������ҳ�����
    char *readAndReplace(index_t pid);


    struct Cache {
        char data[L];
        bool valid;
        bool bit;  // ʱ��ҳ���㷨�ļ��λ
        index_t pid;  // ��Ӧ�����ϵ�ҳid
        Cache() { valid = bit = false; }
    } *cache;

    int cur;  // ����ʱ��ҳ���㷨����ʾ��һ������ҳ��λ��

};
