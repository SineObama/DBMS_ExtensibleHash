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
    //cur = 0;
}

DataManager::~DataManager()
{
    delete[] cache;
    fs.close();
}

//char *DataManager::getAndLock() {
//	return NULL;
//}

//char *DataManager::readAndLock(index_t pid) {
//	bool found = false;
//	int i;
//	for (i = 0; i < P; i++) {
//		if (cache[i].valid == true && cache[i].pid == pid) {
//			cache[i].bit = true;
//			cache[i].pin++;
//			found = true;
//			break;
//		}
//	}
//	// �����б��������ڵ��Ե�
//	int watch[L / sizeof(int)] = {};
//	fs.seekg(15 * L);
//	// ִ����1�оͲ��ᷢ���ļ���ȡ���죬ֱ�ӵ��µĴ�����VLRPUtil::insert����-3
//	//fs.read((char *)watch, L);
//	return found ? cache[i].data : readAndReplace(pid);
//}

//void DataManager::writeAndUnlock(char *data, index_t pid, bool check) {
//	int watch[L / sizeof(int)] = {};
//	//fs.seekg(15 * L);
//	//fs.read((char *)watch, L);
//		// ���Ҷ�Ӧ���ڴ�ҳ��Ϊ�µ�ҳid��ҳidԭ����Ӧ���ڴ�ҳ��Ϊ��Ч
//	for (int i = 0; i < P; i++) {
//		if (cache[i].pid == pid) {
//			cache[i].valid = false;
//		}
//		else if (cache[i].data == data) {
//			cache[i].pid = pid;
//			cache[i].pin--;
//		}
//	}
//	//memcpy(watch, data, L);
//	// д�����
//	fs.seekp(pid * L);
//	fs.write(data, L);
//	fs.clear();
//
//	//fs.seekg(15*L);
//	//fs.read((char *)watch, L);
//
//}

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

    //int watch[L / sizeof(int)] = {};
    //memcpy(watch, c->data, L);

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
    fs.seekp(c->pid * L);
    //int watch[L / sizeof(int)] = {};
    //memcpy(watch, c->data, L);
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

char *DataManager::readAndReplace(index_t pid) {
    static size_t cur = 0;
    int last = cur;
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
    memset(cache[cur].data, 0, L);
    fs.seekg(pid * L);
    fs.read(cache[cur].data, L);
    fs.clear();
    //int watch[L / sizeof(int)] = {};
    //memcpy(watch, cache[cur].data, L);
    //fs.seekg(15 * L);
    //fs.read((char *)watch, L);
    cache[cur].valid = cache[cur].bit = true;
    cache[cur].pid = pid;
    cache[cur].pin++;
    char *rtn = cache[cur].data;
    cur = (cur + 1) % P;
    return rtn;
}
