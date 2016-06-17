#pragma once

#include "DataManager.h"

class Cache {
    friend class DataManager;
public:
    char data[L];
private:
    bool valid;
    bool bit;  // ʱ��ҳ���㷨�ļ��λ
    index_t pid;  // ��Ӧ�����ϵ�ҳid
    size_t pin;  // ��ʹ�õĴ���
    Cache() { valid = bit = false; pin = 0; }
};
