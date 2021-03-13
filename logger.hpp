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
#include <unordered_map>
#include <unordered_set>

class logger final
{
  class color final
  {
  public:
    explicit color(const char* color_str) : color_str_(color_str)
    {
    }
    color(const color&) = delete;
    color& operator=(const color&) = delete;
    ~color() = default;

    friend std::ostream& operator<<(std::ostream& out_stream, const color& obj)
    {
      return out_stream << obj.color_str_;
    }

  private:
    const char* color_str_;
  };

  template <typename T>
  class add_space final
  {
  public:
    explicit add_space(const T& item) : ref_item_(item)
    {
    }
    add_space(const add_space&) = delete;
    add_space& operator=(const add_space&) = delete;
    ~add_space() = default;

    friend std::ostream& operator<<(std::ostream& out_stream, const add_space<T>& obj)
    {
      out_stream << obj.ref_item_;

      if constexpr (!std::is_same_v<std::decay_t<decltype(obj.ref_item_)>, color>)
      {
        out_stream << " ";
      }

      return out_stream;
    }

  private:
    const T& ref_item_;
  };

  class lable final
  {
  public:
    lable(const char* lable_console, const char* lable_file) : lable_console_(lable_console), lable_file_(lable_file)
    {
    }

    const char* console_lable() const noexcept
    {
      return lable_console_;
    }

    const char* file_lable() const noexcept
    {
      return lable_file_;
    }

  private:
    const char* lable_console_;
    const char* lable_file_;
  };

  enum stream : uint8_t
  {
    none_,
    console_,
    file_
  };

  enum type : uint8_t
  {
    info_,
    warning_,
    success_,
    error_,
    all_
  };

  template <typename... Args>
  inline static void log(type type, Args&&... args);

  inline static std::ofstream log_file_;
  inline static auto log_file_name_{"log.txt"};
  inline static auto stream_{stream::console_};

  inline static std::unordered_set<type> type_{type::all_};

  inline static const std::unordered_map<type, lable> lable_{
      {type::info_, {"", "[INFO]:     "}},
      {type::error_, {"\033[1;91m[ERROR]\033[0m:", "[ERROR]:    "}},
      {type::success_, {"\033[1;92m[SUCCESS]\033[0m:", "[SUCCESS]:  "}},
      {type::warning_, {"\033[1;93m[WARNING]\033[0m:", "[WARNING]:  "}},
  };

public:
  logger() = delete;

  constexpr static auto none{stream::none_};
  constexpr static auto console{stream::console_};
  constexpr static auto file{stream::file_};

  constexpr static auto info{type::info_};
  constexpr static auto warning{type::warning_};
  constexpr static auto success{type::success_};
  constexpr static auto error{type::error_};
  constexpr static auto all{type::all_};

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

  inline static void set_stream(stream type);
  inline static void set_log_file_path(const char* path);
  inline static void set_type(std::initializer_list<type> type_list);

  template <typename... Args>
  inline static void log_info_(Args&&... args);

  template <typename... Args>
  inline static void log_warning_(const char* function_name, Args&&... args);

  template <typename... Args>
  inline static void log_error_(const char* file_name, const char* function_name, int line, Args&&... args);

  inline static void log_success_(const char* function_name);

  inline static std::_Put_time<char> current_time(const char* format = "[%Y-%m-%d %H:%M:%S]");
};

inline void logger::set_stream(stream type)
{
  stream_ = type;

  switch (stream_)
  {
    case stream::none_:
    case stream::console_:
      log_file_.close();
      break;
    case stream::file_:
      log_file_.open(log_file_name_, std::ios_base::binary | std::ios_base::app);
    default:
      break;
  }
}

inline void logger::set_log_file_path(const char* path)
{
  log_file_name_ = path;
}

inline void logger::set_type(std::initializer_list<type> type_list)
{
  type_.clear();
  type_.insert(type_list);
}

template <typename... Args>
inline void logger::log_info_(Args&&... args)
{
  log(info, args...);
  std::cerr << reset_color;
}

template <typename... Args>
inline void logger::log_warning_(const char* function_name, Args&&... args)
{
  log(warning, function_name, ":", args...);
  std::cerr << reset_color;
}

template <typename... Args>
inline void logger::log_error_(const char* file_name, const char* function_name, int line, Args&&... args)
{
  log(error, file_name, ":", line, ":", function_name, ":", args...);
  std::cerr << reset_color;
}

inline void logger::log_success_(const char* function_name)
{
  log(success, function_name);
}

template <typename... Args>
inline void logger::log(type type, Args&&... args)
{
  if (!type_.count(type) && !type_.count(all) && lable_.count(type))
  {
    return;
  }

  switch (stream_)
  {
    case console:
      (std::cerr << lable_.find(type)->second.console_lable() << ... << add_space(args)) << std::endl;
      break;

    case file: {
      if (log_file_.is_open())
      {
        (log_file_ << current_time() << lable_.find(type)->second.file_lable() << ... << add_space(args)) << std::endl;
      }
    }
    break;
    default:
      break;
  }
}

inline std::_Put_time<char> logger::current_time(const char* format)
{
  const auto current_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  return std::put_time(std::localtime(&current_time_t), format);
}

#endif  // LOGGER_HPP
