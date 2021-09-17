#pragma once
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>


typedef struct loaderqueue{
    std::queue<std::function<void()>> q;
    std::mutex mtx;
} loaderdata;

typedef struct multi_dispatch{
    loaderqueue mq;
    loaderqueue wq;
    std::condition_variable cv;
}multi_dispatch;
