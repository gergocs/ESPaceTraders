//
// Created by gergocs on 2023.05.16..
//

// Credit: phostershop#9408 (deadmeat)
// https://discord.com/channels/792864705139048469/1106601900155076658/1107762279304413387

#ifndef ESPACETRADERS_BURSTMANAGER_H
#define ESPACETRADERS_BURSTMANAGER_H

#include <chrono>
#include <mutex>
#include <vector>

using duration_t = std::chrono::duration<long long, std::ratio<1, 1000>>;
using burstLock = std::lock_guard<std::mutex>;

static constexpr duration_t rateLimit = std::chrono::milliseconds(500); // 2 per second
static constexpr std::size_t burstLimit = 10;

class BurstManager {
public:
    static duration_t getDelay();

    static void markRequestPosted();

private:
    static std::vector<std::time_t> bucketS;
    static std::mutex burstMutexS;
};


#endif //ESPACETRADERS_BURSTMANAGER_H
