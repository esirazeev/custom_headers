#ifndef LOGGER_HPP
#define LOGGER_HPP

#define log_info(...) logger::log_info_(__VA_ARGS__)
#define log_warning(...) logger::log_warning_(__PRETTY_FUNCTION__, __VA_ARGS__)
#define log_success() logger::log_success_(__PRETTY_FUNCTION__)
#define log_error(...) logger::log_error_(__FILE__, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)

#include <chrono>
#include <ctime>
#include <fstream>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

enum class log_stream : uint8_t
{
	CONSOLE,
	FILE
};

enum class log_type : uint8_t
{
	NONE,
	INFO,
	WARNING,
	SUCCESS,
	ERROR,
	ALL
};

struct item
{
	explicit item(const char* str) : str_(str) {}
	const char* str_;
};

struct color : public item
{
	explicit color(const char* str) : item(str) {}
};

struct lable : public item
{
	explicit lable(const char* str) : item(str) {}
};

struct lable_attr : public item
{
	explicit lable_attr(const char* str) : item(str) {}
};

struct cur_time
{
};

decltype(auto) operator<<(std::ostream& out_stream, const item& obj)
{
	return out_stream << obj.str_;
}

decltype(auto) operator<<(std::ostream& out_stream, const cur_time& obj)
{
	const auto current_time_t{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
	return out_stream << std::put_time(std::localtime(&current_time_t), "[%Y-%m-%d %H:%M:%S]");
}

class logger final
{
public:
	logger() = delete;

	inline static const color reset_color{"\033[0m"};
	inline static const color red_color{"\033[0;31m"};
	inline static const color green_color{"\033[0;32m"};
	inline static const color yellow_color{"\033[0;33m"};
	inline static const color blue_color{"\033[0;34m"};
	inline static const color purple_color{"\033[0;35m"};
	inline static const color cyan_color{"\033[0;36m"};
	inline static const color white_color{"\033[0;37m"};
	inline static const color red_bold_color{"\033[1;31m"};
	inline static const color green_bold_color{"\033[1;32m"};
	inline static const color yellow_bold_color{"\033[1;33m"};
	inline static const color blue_bold_color{"\033[1;34m"};
	inline static const color purple_bold_color{"\033[1;35m"};
	inline static const color cyan_bold_color{"\033[1;36m"};
	inline static const color white_bold_color{"\033[1;37m"};

	inline static void set_log_stream(log_stream stream)
	{
		cur_log_stream = stream;

		switch (cur_log_stream)
		{
		case log_stream::CONSOLE:
			log_file.close();
			break;
		case log_stream::FILE:
			log_file.open(log_file_path, std::ios_base::binary | std::ios_base::app);
		default:
			break;
		}
	}

	inline static void set_log_type(std::initializer_list<log_type> types)
	{
		log_types.clear();
		log_types.insert(types);
	}

	inline static void set_log_file_path(std::string_view path)
	{
		log_file_path = path;
	}

	template <typename... Args>
	inline static void log_info_(Args&&... args)
	{
		log(log_type::INFO, cur_time{}, lable{get_lable(log_type::INFO)}, std::forward<Args>(args)...,
		    reset_color);
	}

	template <typename... Args>
	inline static void log_warning_(const char* function_name, Args&&... args)
	{
		log(log_type::WARNING, cur_time{}, lable{get_lable(log_type::WARNING)}, white_bold_color,
		    lable_attr{function_name}, lable_attr{": "}, reset_color, std::forward<Args>(args)...,
		    reset_color);
	}

	template <typename... Args>
	inline static void log_error_(const char* file_name, const char* function_name, int32_t line, Args&&... args)
	{
		log(log_type::ERROR, cur_time{}, lable{get_lable(log_type::ERROR)}, white_bold_color,
		    lable_attr{file_name}, lable_attr{":"},
		    lable_attr{std::to_string(line).c_str()}, lable_attr{":"},
		    lable_attr{function_name}, lable_attr{": "}, reset_color, std::forward<Args>(args)...,
		    reset_color);
	}

	inline static void log_success_(const char* function_name)
	{
		log(log_type::SUCCESS, cur_time{}, lable{get_lable(log_type::SUCCESS)}, white_bold_color,
		    lable_attr{function_name}, reset_color);
	}

private:
	struct lable_type
	{
		const char* console;
		const char* file;
	};

	inline static const char* get_lable(log_type type)
	{
		auto& lable{log_lables.find(type)->second};

		return cur_log_stream == log_stream::CONSOLE ? lable.console : lable.file;
	}

	inline static std::ostream& get_stream()
	{
		return cur_log_stream == log_stream::CONSOLE ? std::cerr : log_file;
	}

	inline static bool is_logging_acceptable(log_type type)
	{
		return (log_types.contains(log_type::NONE) ||
			(!log_types.contains(type) && !log_types.contains(log_type::ALL))) ?
			       false :
				     true;
	}

	template <typename... Args>
	inline static void log(log_type type, Args&&... args)
	{
		if (!is_logging_acceptable(type))
		{
			return;
		}

		auto&& stream{get_stream()};
		bool space_needed{false};

		(print(stream, std::forward<Args>(args), space_needed), ...);

		stream << std::endl;
	}

	template <typename T>
	inline static void print(std::ostream& stream, T&& item, bool& space_needed)
	{
		if ((std::is_same_v<std::decay_t<T>, color> && cur_log_stream == log_stream::FILE) ||
		    (std::is_same_v<std::decay_t<T>, cur_time> && cur_log_stream != log_stream::FILE))
		{
			return;
		}

		if (!std::is_same_v<std::decay_t<T>, color> && !std::is_same_v<std::decay_t<T>, lable> &&
		    !std::is_same_v<std::decay_t<T>, lable_attr> && !std::is_same_v<std::decay_t<T>, cur_time>)
		{
			if (space_needed)
			{
				stream << " ";
			}

			space_needed = true;
		}

		stream << item;
	}

	inline static std::ofstream log_file;
	inline static std::string log_file_path{"log.txt"};
	inline static auto cur_log_stream{log_stream::CONSOLE};
	inline static std::unordered_set<log_type> log_types{log_type::ALL};
	inline static const std::unordered_map<log_type, lable_type> log_lables{
		{log_type::INFO, {"", "[INFO]:"}},
		{log_type::ERROR, {"\033[1;91m[ERROR]\033[0m:", "[ERROR]:"}},
		{log_type::SUCCESS, {"\033[1;92m[SUCCESS]\033[0m:", "[SUCCESS]:"}},
		{log_type::WARNING, {"\033[1;93m[WARNING]\033[0m:", "[WARNING]:"}},
	};
};

#endif // LOGGER_HPP
