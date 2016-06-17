#include "stdafx.h"
#include "DataManager.h"

DataManager::DataManager(const char *filename)
{
    // ���ļ�������ʱ����һ���յ�
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        std::ofstream ofs(filename);
        ofs.close();
    }

    // bug�޸���windows�²��ö����ƴ򿪻��\n�ĳ�\r\n����������ֵĺ����ǰ�0x0005�滻��0x0e05�������ƴ򿪾�û�����ˡ�
    fs.open(filename, std::ios::binary | std::ios::in | std::ios::out);
    cache = new Page[P];
    I = O = replaceCount = 0;
    Itime = Otime = 0;
}

DataManager::~DataManager()
{
    delete[] cache;
    fs.close();
}

Page *DataManager::getAndLock() {
    Page *c = replace();
    if (c == NULL)
        return NULL;
    memset(c->data, 0, L);
    return lock(c);
}

Page *DataManager::getAndLock(index_t pid) {
    // ���Բ����ڴ��ѻ����ҳ
    int i;
    for (i = 0; i < P; i++) {
        if (cache[i].pin == 0 && cache[i].valid == true && cache[i].pid == pid) {
            cache[i].bit = true;
            return lock(cache + i);
        }
    }
    // ʹ��ʱ��ҳ���滻����
    Page *c = replace();
    if (c == NULL)
        return NULL;

    c->bit = true;
    memset(c->data, 0, L);
    c->pid = pid;
    c->valid = true;

    clock_t begin = clock();
    fs.seekg(pid * L);
    fs.read(c->data, L);
    fs.clear();
    clock_t end = clock();
    I++;
    Itime += end - begin;
    return lock(c);

}

Page *DataManager::writeAndUnlock(Page *c, index_t pid) {
    if (c->pid != pid) {
        // ���ҵ�����д���ҳid�Ķ�Ӧ�ڴ棬��Ϊ��Ч
        for (size_t i = 0; i < P; i++) {
            if (cache[i].valid && cache[i].pid == pid) {
                cache[i].valid = false;
                break;
            }
        }
        c->pid = pid;
        c->valid = true;
    }
    return writeAndUnlock(c);
}

// д�����
Page *DataManager::writeAndUnlock(Page *c) {
    size_t pos = c->pid * L;
    if (pos > 0x7fffffff) {
        throw std::exception("���λ�ó���2G");
    }
    clock_t begin = clock();
    fs.seekp(pos);
    fs.write(c->data, L);
    fs.clear();
    clock_t end = clock();
    O++;
    Otime += end - begin;
    return unlock(c);
}

Page *DataManager::lock(Page *c) {
    if (c->pin > 0)
        return NULL;
    else {
        c->pin++;
        return c;
    }
}

Page *DataManager::unlock(Page *c) {
    if (c->pin > 0)
        c->pin--;
    return c;
}

Page *DataManager::replace() {
    replaceCount++;
    static size_t cur = 0;
    size_t last = cur;
    bool one = true, fail = false;
    while (1)
    {
        if (cache[cur].pin == 0) {
            if (!cache[cur].valid || cache[cur].bit == false)
                break;
            cache[cur].bit = false;
        }
        cur = (cur + 1) % P;
        if (cur == last) {
            if (one)
                one = false;
            else {
                fail = true;
                break;
            }
        }
    }
    if (fail)
        return NULL;
    Page *rtn = cache + cur;
    cur = (cur + 1) % P;
    return rtn;
}

size_t DataManager::getIOs() {
    return I + O;
}

clock_t DataManager::getIOtime() {
    return Itime + Otime;
}

size_t DataManager::getReplacements() {
    return replaceCount;
}
