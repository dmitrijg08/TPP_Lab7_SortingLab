#pragma once
#include <iostream>
#include <string>
#include <utility>
#include <vector>

class ScopeLogger
{
private:
    std::string name_;
    std::vector<std::string>* externalLog_;

public:
    explicit ScopeLogger(std::string name, std::vector<std::string>* externalLog = nullptr)
        : name_(std::move(name)), externalLog_(externalLog)
    {
        std::cout << "[ENTER] " << name_ << '\n';
        if (externalLog_ != nullptr)
        {
            externalLog_->push_back("ENTER:" + name_);
        }
    }

    ~ScopeLogger() noexcept
    {
        std::cout << "[EXIT ] " << name_ << '\n';
        if (externalLog_ != nullptr)
        {
            externalLog_->push_back("EXIT:" + name_);
        }
    }

    ScopeLogger(const ScopeLogger&) = delete;
    ScopeLogger& operator=(const ScopeLogger&) = delete;
};
