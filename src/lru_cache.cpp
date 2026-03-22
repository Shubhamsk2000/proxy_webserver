#include "lru_cache.h"

LRUCache::LRUCache(size_t capacity): capacity(capacity){}

bool LRUCache::getNode(const std::string& key, std::string& value){
    std::lock_guard<std::mutex>lock(mtx);

    auto it = cacheMap.find(key);

    //we did not find the node, return false and put the node next
    if(it == cacheMap.end()){
        return false;
    }

    //we found the node present in the cache, now we will move it in front of list
    cacheList.splice(cacheList.begin(), cacheList, it->second);

    value = it->second->second;
    return true;

}

void LRUCache::putNode(const std::string& key, const std::string& value){
    std::lock_guard<std::mutex>lock(mtx);

    auto it = cacheMap.find(key);

    if(it != cacheMap.end()){
        it->second->second = value;
        cacheList.splice(cacheList.begin(), cacheList, it->second);
        return;
    }
    //TODO- list is pair
    cacheList.emplace_front(key, value);
    cacheMap[key] = cacheList.begin();

    if(cacheMap.size() > capacity){
        auto lastKey = cacheList.back().first;

        cacheMap.erase(lastKey);
        cacheList.pop_back();
    }
}