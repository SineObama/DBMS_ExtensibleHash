#pragma once

#include "VLRPUtil.h"

class Page {
    friend class RandomAccessMemory;
public:
    char data[L];
private:
    bool valid;
    bool bit;  // ʱ��ҳ���㷨�ļ��λ
    index_t pid;  // ��Ӧ�����ϵ�ҳid
    size_t pin;  // ��ʹ�õĴ���
    Page() { valid = bit = false; pin = 0; }
};
