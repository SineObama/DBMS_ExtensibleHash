#pragma once

#include "DataManager.h"

class Page {
    friend class DataManager;
public:
    char data[L];
private:
    bool valid;
    bool bit;  // ʱ��ҳ���㷨�ļ��λ
    index_t pid;  // ��Ӧ�����ϵ�ҳid
    size_t pin;  // ��ʹ�õĴ���
    Page() { valid = bit = false; pin = 0; }
};
