#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <string>

namespace logger
{
template<typename T>
inline void print(T&& item)
{
	std::cout << std::forward<T>(item);
}

template<typename T>
inline void println(T&& item)
{
	std::cout << std::forward<T>(item) << std::endl;
}

inline void println()
{
	std::cout << std::endl;
}

template<typename T>
inline void print_error(T&& item)
{
	std::cerr << std::forward<T>(item);
}

template<typename T>
inline void println_error(T&& item)
{
	std::cerr << std::forward<T>(item) << std::endl;
}
} // namespace logger

#endif // LOGGER_HPP
