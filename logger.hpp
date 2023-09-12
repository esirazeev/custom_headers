#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <string>

namespace logger
{
inline void print(const std::string& str)
{
	std::cout << str;
}

inline void println(const std::string_view str)
{
	std::cout << str << std::endl;
}

inline void print_error(const std::string_view error)
{
	std::cerr << error;
}

inline void println_error(const std::string_view error)
{
	std::cerr << error << std::endl;
}
} // namespace logger

#endif // LOGGER_HPP
