//
// Created by gergocs on 2023.05.16..
//

#include <algorithm>
#include "BurstManager.h"

std::vector<std::time_t> BurstManager::bucketS;
std::mutex BurstManager::burstMutexS;

duration_t BurstManager::getDelay() {
    duration_t result{0};
    burstLock lock(burstMutexS);
    const long long pastLimit(
            std::time(nullptr) -
            std::chrono::duration_cast<std::chrono::seconds>(rateLimit * burstLimit)
                    .count());
    // remove all timestamps that are earlier than our past limit.
    bucketS.erase(std::remove_if(bucketS.begin(), bucketS.end(),
                                 [&](long ts) { return ts < pastLimit; }),
                  bucketS.end());
    auto bucketSize = bucketS.size();

    if (bucketSize > burstLimit) {
        result = (bucketSize - burstLimit) * rateLimit;
    }

    return result;
}

void BurstManager::markRequestPosted() {
    burstLock lock(burstMutexS);
    bucketS.push_back(std::time(nullptr));
}
