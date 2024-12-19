// src/Core/DeletionQueue.cpp

#include "DeletionQueue.hpp"

void DeletionQueue::flush() {
    for (auto i = m_deletors.rbegin(); i != m_deletors.rend(); i++) {
        (*i)();
    }

    m_deletors.clear();
}

void DeletionQueue::push(std::function<void()>&& function) {
    m_deletors.push_back(function);
}
