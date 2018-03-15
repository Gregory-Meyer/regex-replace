#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <utility>

std::string make_random_email() {
    static const std::uint64_t now = std::chrono::high_resolution_clock::now()
        .time_since_epoch()
        .count();
    static std::mt19937_64 device{ now };
    static std::uniform_int_distribution<char> distribution{ 'a', 'z' };

    std::string email;
    email.reserve(12);

    while (email.size() < 12) {
        if (email.size() == 8) {
            email.push_back('@');
        } else {
            email.push_back(distribution(device));
        }
    }

    return email;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cout.tie(nullptr);
    std::cin.tie(nullptr);

    static constexpr int COUNT = 1 << 24;

    for (int i = 0; i < COUNT; ++i) {
        std::cout << make_random_email() << ",";
    }

    std::cout << std::endl;
}
