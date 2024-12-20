// src/RenderEngine/CommandSubmitter.cpp

#include "CommandSubmitter.hpp"

bool CommandSubmitter::initialize() {

    return true;
}

void CommandSubmitter::immediateSubmit(const std::function<void(VkCommandBuffer)>& function) {

}

void CommandSubmitter::shutdown() {

}

