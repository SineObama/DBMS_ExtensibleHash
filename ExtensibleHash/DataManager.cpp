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
    cache = new Cache[P];
    replace_count = 0;
}

DataManager::~DataManager()
{
    delete[] cache;
    fs.close();
}

Cache *DataManager::getBlockAndLock() {
    Cache *c = replace();
    if (c == NULL)
        return NULL;
    memset(c->data, 0, L);
    return lock(c);
}

Cache *DataManager::getBlockAndLock(index_t pid) {
    if (pid > MAXPID) {
        return NULL;
    }
    // ���Բ����ڴ��ѻ����ҳ
    int i;
    for (i = 0; i < P; i++) {
        if (cache[i].pin == 0 && cache[i].valid == true && cache[i].pid == pid) {
            cache[i].bit = true;
            return lock(cache + i);
        }
    }
    // ʹ��ʱ��ҳ���滻����
    Cache *c = replace();
    if (c == NULL)
        return NULL;

    c->bit = true;
    memset(c->data, 0, L);
    c->pid = pid;
    c->valid = true;

    fs.seekg(pid * L);
    fs.read(c->data, L);
    fs.clear();
    return lock(c);

}

Cache *DataManager::writeBlockAndUnlock(Cache *c, index_t pid) {
    if (pid > MAXPID) {
        return NULL;
    }
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
    return writeBlockAndUnlock(c);
}

// д�����
Cache *DataManager::writeBlockAndUnlock(Cache *c) {
    if (c->pid > MAXPID) {
        return NULL;
    }
    size_t pos = c->pid * L;
    if (pos > 0x7fffffff) {
        throw std::exception("���λ�ó���2G");
    }
    fs.seekp(pos);
    fs.write(c->data, L);
    fs.clear();
    return unlock(c);
}

Cache *DataManager::lock(Cache *c) {
    if (c->pin > 0)
        return NULL;
    else {
        c->pin++;
        return c;
    }
}

Cache *DataManager::unlock(Cache *c) {
    if (c->pin > 0)
        c->pin--;
    return c;
}

Cache *DataManager::replace() {
    replace_count++;
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
    Cache *rtn = cache + cur;
    cur = (cur + 1) % P;
    return rtn;
}
