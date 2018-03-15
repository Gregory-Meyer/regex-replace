#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>

std::string read_buffer(std::istream &is) {
    return std::string{ std::istreambuf_iterator<char>{ is },
                        std::istreambuf_iterator<char>{ } };
}

std::string replace_commas(std::string_view text) {
    std::string replaced;
    replaced.reserve(text.size() * 3 / 2);

    for (auto found = text.find(','); found != std::string_view::npos;
         found = text.find(',')) {
        replaced += text.substr(0, found);
        replaced += ",\n";
        text.remove_prefix(found + 1);
    }

    return replaced;
}

template <typename Representation, typename Invocable, typename ...Args,
          typename = std::enable_if_t<std::is_invocable_v<Invocable, Args...>>>
std::pair<std::chrono::duration<Representation>,
          std::invoke_result_t<Invocable, Args...>>
time_invoke(Invocable &&f, Args &&...args)
noexcept(std::is_nothrow_invocable_v<Invocable, Args...>) {
    using PairT = std::pair<std::chrono::duration<Representation>,
                            std::invoke_result_t<Invocable, Args...>>;

    const auto start = std::chrono::high_resolution_clock::now();

    PairT result{
        std::piecewise_construct,
        std::forward_as_tuple(),
        std::forward_as_tuple(std::invoke(std::forward<Invocable>(f),
                                          std::forward<Args>(args)...))
    };

    const auto finish = std::chrono::high_resolution_clock::now();

    result.first = finish - start;

    return result;
}

int main(const int argc, const char *const argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    if (argc < 3) {
        std::cerr << "usage: replace INPUT OUTPUT" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    const std::string input_filename = argv[1];
    const std::string output_filename = argv[2];

    std::ifstream ifs{ input_filename };
    std::ofstream ofs{ output_filename };

    if (not ifs.is_open()) {
        std::cerr << "error: unable to open '" << input_filename << "'"
            << std::endl;
        std::exit(EXIT_FAILURE);
    } else if (not ofs.is_open()) {
        std::cerr << "error: unable to open '" << output_filename << "'"
            << std::endl;
        std::exit(EXIT_FAILURE);
    }

    const std::string contents = read_buffer(ifs);

    std::cout << "read " << contents.size() << " bytes from '"
              << input_filename << "'\n";

    const auto [elapsed, replaced] = time_invoke<double>(replace_commas,
                                                         contents);

    ofs.write(replaced.data(), replaced.size());

    std::cout << "wrote " << replaced.size() << " bytes to '"
              << output_filename << "'\n";
    std::cout << "took " << elapsed.count() << " seconds" << std::endl;
}   
