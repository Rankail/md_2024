#ifndef MD_2024_ARGUMENTS_H
#define MD_2024_ARGUMENTS_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <optional>

class Arguments {
private:
    std::unordered_set<std::string> flags; // starts with - or --
    std::unordered_map<std::string, std::string> namedArgs; // starts with - or -- and contains =
    std::vector<std::string> args; // everything else

public:
    Arguments(int argc, char* argv[]);

    bool hasFlag(const std::string& flag) const;
    std::optional<std::string> getNamed(const std::string& name) const;
    std::vector<std::string> getArgs() const { return args; }

    size_t getArgCount() const { return args.size(); }

    Arguments& operator>>(double& d);
    Arguments& operator>>(float& f);
    Arguments& operator>>(int& i);
    Arguments& operator>>(unsigned& u);
    Arguments& operator>>(char& c);
    Arguments& operator>>(std::string& s);

private:
    void parseFlag(const std::string& value);
    void parseNamed(const std::string& value);
    void parseSimpleArg(const std::string& value);

};


#endif //MD_2024_ARGUMENTS_H
