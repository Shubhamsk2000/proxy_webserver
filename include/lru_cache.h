#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include<unordered_map>
#include<string>
#include<mutex>
#include<list>


class LRUCache{
    private:
        std::list<std::pair<std::string, std::string>>cacheList;
        std::unordered_map<std::string, std::list<std::pair<std::string, std::string>>::iterator>cacheMap;

        size_t capacity;
        std::mutex mtx;

    public:
        LRUCache(size_t capacity);
        bool getNode(const std::string& key, std::string& value);
        void putNode(const std::string& key, const std::string& value);
};

#endif