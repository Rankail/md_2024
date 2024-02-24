#include "utils/Arguments.h"

#include <utils/log/Logger.h>

Arguments::Arguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg.starts_with("-")) {
            if (arg.find('=') != std::string::npos) {
                parseNamed(arg);
            } else {
                parseFlag(arg);
            }
        } else {
            parseSimpleArg(arg);
        }
    }
}

bool Arguments::hasFlag(const std::string &flag) const {
    return flags.contains(flag);
}

std::optional<std::string> Arguments::getNamed(const std::string &name) const {
    auto it = namedArgs.find(name);
    if (it == namedArgs.end()) {
        return {};
    }
    return it->second;
}

void Arguments::parseFlag(const std::string &value) {
    auto first = value.find_first_not_of('-');
    if (first == std::string::npos) {
        TET_ERROR("Expected a flag-name after '{}'", value);
        return;
    }
    if (first > 2) {
        TET_ERROR("Expected '-' or '--' not '{}'", value.substr(0, first-1));
        return;
    }
    auto flag = value.substr(first);
    if (!flags.emplace(flag).second) {
        TET_WARN("Flag '{}' was mentioned multiple times", flag);
    }
}

void Arguments::parseNamed(const std::string &arg) {
    auto first = arg.find_first_not_of('-');
    if (first == std::string::npos) {
        TET_ERROR("Expected a flag-name after '{}'", arg);
        return;
    }
    if (first > 2) {
        TET_ERROR("Expected '-' or '--' not '{}'", arg.substr(0, first));
        return;
    }
    auto equal = arg.find('=');
    if (equal == first) {
        TET_ERROR("Missing name between '{}' and '='", arg.substr(0, first));
        return;
    }
    if (equal == arg.size() - 1) {
        TET_ERROR("Expected value after '='");
        return;
    }
    auto name = arg.substr(first, equal - first);
    auto value = arg.substr(equal + 1);
    if (!namedArgs.emplace(name, value).second) {
        TET_WARN("Nmaed argument '{}' was set multiple times", name);
    }
}

void Arguments::parseSimpleArg(const std::string &value) {
    args.emplace_back(value);
}

#define ARG_STREAM(type, transformFn)                       \
Arguments &Arguments::operator>>(type &t) {                 \
    std::string str = args.front();                         \
    args.erase(args.begin());                               \
                                                            \
    type value;                                             \
    try {                                                   \
        value = transformFn(str);                           \
    } catch (const std::invalid_argument& ia) {             \
        TET_ERROR("Invalid argument: {}", ia.what());       \
        value = 0.0;                                        \
    } catch (const std::out_of_range& oor) {                \
        TET_ERROR("Number out of range: {}", oor.what());   \
        value = 0.0;                                        \
    }                                                       \
    t = value;                                              \
                                                            \
    return *this;                                           \
}

ARG_STREAM(double, std::stod)
ARG_STREAM(float, std::stof)
ARG_STREAM(unsigned, std::stoul)
ARG_STREAM(int, std::stoi)

Arguments &Arguments::operator>>(char &c) {
    std::string str = args.front();
    args.erase(args.begin());

    if (str.size() != 1) {
        TET_ERROR("Expected exactly 1 character. Got {}", str.size());
        return *this;
    }

    c = str[0];
    return *this;
}

Arguments &Arguments::operator>>(std::string &s) {
    std::string str = args.front();
    args.erase(args.begin());
    s = str;
    return *this;
}
