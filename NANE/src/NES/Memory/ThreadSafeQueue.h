#pragma once

#include <vector>
#include <queue>
#include <algorithm>
#include <mutex>
#include <iostream>

template <class T>
class ThreadSafeQueue
{
    private:
    std::queue<T> queue;
    std::mutex mu;
    int maxSize;

    void RemoveOldestRecords(int numOfRecorders)
    {
        for(int i = 0; i < numOfRecorders; ++i)
        {
            this->queue.pop();
        }
    }

    public:
    ThreadSafeQueue(int maxSize = 0)
    {
        this->maxSize = maxSize;
    }

    void Push(const std::vector<T>& data)
    {
        const std::lock_guard<std::mutex> lock(mu);

        if(this->maxSize > 0)
        {
            // if max size specified then see if overflow will occur

            int overflowAmount = this->maxSize - (this->queue.size() + data.size());
            if(overflowAmount > 0)
            {
                std::cerr << "ThreadSafeQueue has overflowed. Discarding: " << overflowAmount << " records" << std::endl;
                this->Pop(overflowAmount);
            }
        }

        for(const T& value : data)
        {
            this->queue.push(value);
        }
    }

    void Push(const T& value)
    {
        const std::lock_guard<std::mutex> lock(mu);

        if(this->maxSize > 0)
        {
            // if max size specified then see if overflow will occur

            int overflowAmount = (this->queue.size() + 1) - this->maxSize;
            if(overflowAmount > 0)
            {
                std::cerr << "ThreadSafeQueue has overflowed. Discarding: " << overflowAmount << " records" << std::endl;
                this->RemoveOldestRecords(this->maxSize);
            }
        }
        this->queue.push(value);
    }

    std::vector<T> Pop(unsigned int len)
    {
        const std::lock_guard<std::mutex> lock(mu);

        int return_len = std::min(len, (unsigned int)this->queue.size());
        std::vector<T> retrievedData(return_len);
        for(int i = 0; i < return_len; ++i)
        {
            T value = this->queue.front();
            retrievedData.at(i) = value;
            this->queue.pop();
        }
        return retrievedData;
    }
};