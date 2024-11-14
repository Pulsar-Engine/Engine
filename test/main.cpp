#include <string>

constexpr bool (*tests[])(void) = {
    nullptr
};

int main(int argc, char **argv)
{
    if (argc != 2)
        return (84);
    int test = std::stoi(argv[1]);
    for (int i = 0; tests[i] != nullptr; i++) {
        if (i == test)
            return (tests[i]() ? 0 : 1);
    }
    return (0);
}