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

	fs.open(filename);
	cache = new Cache[P];
	cur = 0;
}

DataManager::~DataManager()
{
	delete[] cache;
	fs.close();
}

char *DataManager::read(index_t pid) {
	bool found = false;
	int i;
	for (i = 0; i < P; i++) {
		if (cache[i].valid == true && cache[i].pid == pid) {
			cache[i].bit = true;
			found = true;
			break;
		}
	}
	// �����б��������ڵ��Ե�
	int watch[L / sizeof(int)] = {};
	fs.seekg(15 * L);
	// ִ����1�оͲ��ᷢ���ļ���ȡ���죬ֱ�ӵ��µĴ�����VLRUtil::insert����-3
	//fs.read((char *)watch, L);
	return found ? cache[i].data : readAndReplace(pid);
}

void DataManager::write(char *data, index_t pid, bool check) {
	int watch[L / sizeof(int)] = {};
	//fs.seekg(15 * L);
	//fs.read((char *)watch, L);
	if (check) {
		// ���Ҷ�Ӧ���ڴ�ҳ��Ϊ�µ�ҳid��ҳidԭ����Ӧ���ڴ�ҳ��Ϊ��Ч
		for (int i = 0; i < P; i++) {
			if (cache[i].pid == pid) {
				cache[i].valid = false;
			}
			else if (cache[i].data == data) {
				cache[i].pid = pid;
			}
		}
	}
	//memcpy(watch, data, L);
	// д�����
	fs.seekp(pid * L);
	fs.write(data, L);
	fs.clear();

	//fs.seekg(15*L);
	//fs.read((char *)watch, L);

}

//int DataManager::write(index_t pid) {
//    bool found = false;
//    char *src = NULL;
//    for (int i = 0; i < P; i++) {
//        if (cache[i].valid == true && cache[i].pid == pid) {
//            src = cache[i].data;
//            found = true;
//            break;
//        }
//    }
//    if (src != NULL) {
//        fs.seekp(pid * L, std::ios::beg);
//        fs.write(src, L);
//    }
//    return 0;
//}

char *DataManager::readAndReplace(index_t pid) {
	int last = cur;
	do
	{
		if (!cache[cur].valid || !cache[cur].bit)
			break;
		cache[cur].bit = false;
		cur = (cur + 1) % P;
	} while (last != cur);
	memset(cache[cur].data, 0, L);
	fs.seekg(pid * L);
	fs.read(cache[cur].data, L);
	fs.clear();
	int watch[L / sizeof(int)] = {};
	memcpy(watch, cache[cur].data, L);
	fs.seekg(15 * L);
	fs.read((char *)watch, L);
	cache[cur].valid = cache[cur].bit = true;
	cache[cur].pid = pid;
	char *rtn = cache[cur].data;
	cur = (cur + 1) % P;
	return rtn;
}
