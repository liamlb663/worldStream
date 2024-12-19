// src/Core/DeletionQueue.hpp

#pragma once

#include <deque>
#include <functional>

class DeletionQueue {
public:
    void push(std::function<void()>&& function);
    void flush();

private:
    std::deque<std::function<void()>> m_deletors;

};
