#pragma once

#include <vector>
#include <random>
#include <utility>
#include <memory>

class Thread;
class Memory;

class PathChooser {
public:
    virtual size_t ChooseThread(const std::vector<Thread>&, const std::shared_ptr<const Memory>&) = 0;
    virtual bool ExecSilent() = 0;
    virtual int ChooseSilent(const std::string&, const std::vector<std::string>&) = 0;
    virtual ~PathChooser() = default;
};
