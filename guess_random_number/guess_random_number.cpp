#include <iostream>
#include <random>

int main(int argc, char** argv) {
    using namespace std::literals::string_literals;

    if (argc < 3) {
        std::cout << "Usage example:\n"
                  << "$ ./a.out 1 10" << std::endl;
        std::exit(1);
    }

    int range_start = std::stoi(std::string(argv[1]));
    int range_end = std::stoi(std::string(argv[2]));

    std::cout << "Hey! Try to guess the number between "s << range_start << " and " << range_end << "?" << std::endl;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution dist(range_start, range_end);

    while (true) {
        int answer = dist(gen);

        int your_num;
        std::cin >> your_num;

        if (your_num == answer) {
            std::cout << "Contgratulation! The answer is " << answer << std::endl;

            break;
        }

        std::cout << "Oh... the number was " << answer << std::endl;
    }

    return 0;
}
