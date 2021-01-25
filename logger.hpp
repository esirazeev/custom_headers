#ifndef LOGGER_HPP
#define LOGGER_HPP

#define LOG_INFO(...) Logger::LogInfo(__VA_ARGS__)
#define LOG_DEBUG(...) Logger::LogDebug(__PRETTY_FUNCTION__, __VA_ARGS__)
#define LOG_SUCCESS() Logger::LogSuccess(__PRETTY_FUNCTION__)
#define LOG_ERROR(...) Logger::LogError(__FILE__, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)

#include <chrono>
#include <ctime>
#include <fstream>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <string_view>
#include <unordered_set>

class Color final
{
public:
  Color() = delete;
  inline explicit Color(std::string_view color);
  Color(const Color&) = delete;
  Color(Color&&) = delete;
  Color& operator=(const Color&) = delete;
  Color& operator=(Color&&) = delete;
  ~Color() = default;

  inline friend std::ostream& operator<<(std::ostream& out_stream, const Color& obj);

private:
  std::string_view color_;
};

inline Color::Color(std::string_view color) : color_(color)
{
}

inline std::ostream& operator<<(std::ostream& out_stream, const Color& obj)
{
  return out_stream << obj.color_;
}

template <typename T>
class AddSpace final
{
public:
  AddSpace() = delete;
  inline explicit AddSpace(const T& item);
  AddSpace(const AddSpace&) = delete;
  AddSpace(AddSpace&&) = delete;
  AddSpace& operator=(const AddSpace&) = delete;
  AddSpace& operator=(AddSpace&&) = delete;
  ~AddSpace() = default;

  template <typename U>
  inline friend std::ostream& operator<<(std::ostream& out_stream, const AddSpace<U>& obj);

private:
  const T& ref_item_;
};

template <typename T>
inline AddSpace<T>::AddSpace(const T& item) : ref_item_{item}
{
}

template <typename T>
inline std::ostream& operator<<(std::ostream& out_stream, const AddSpace<T>& obj)
{
  out_stream << obj.ref_item_;

  if constexpr (!std::is_same_v<std::decay_t<decltype(obj.ref_item_)>, Color>)
  {
    out_stream << " ";
  }

  return out_stream;
}

class Logger final
{
private:
  enum StreamType : uint8_t
  {
    kNone,
    kConsole,
    kFile
  };

  enum LogType : uint8_t
  {
    kInfo,
    kDebug,
    kSuccess,
    kError,
    kAll
  };

public:
  Logger() = delete;

  inline static void SetStream(StreamType type);
  inline static void SetLogType(std::initializer_list<LogType> type_list);

  template <typename... Args>
  inline static void LogInfo(Args&&... args);

  template <typename... Args>
  inline static void LogDebug(const char* function_name, Args&&... args);

  template <typename... Args>
  inline static void LogError(const char* file_name, const char* function_name, int line, Args&&... args);

  inline static void LogSuccess(const char* function_name);

  constexpr static auto none{StreamType::kNone};
  constexpr static auto console{StreamType::kConsole};
  constexpr static auto file{StreamType::kFile};

  constexpr static auto info{LogType::kInfo};
  constexpr static auto debug{LogType::kDebug};
  constexpr static auto success{LogType::kSuccess};
  constexpr static auto error{LogType::kError};
  constexpr static auto all{LogType::kAll};

  inline static const Color reset_color{"\033[0m"};
  inline static const Color red_color{"\033[0;31m"};
  inline static const Color green_color{"\033[0;32m"};
  inline static const Color yellow_color{"\033[0;33m"};
  inline static const Color blue_color{"\033[0;34m"};
  inline static const Color purple_color{"\033[0;35m"};
  inline static const Color cyan_color{"\033[0;36m"};
  inline static const Color white_color{"\033[0;37m"};
  inline static const Color red_bold_color{"\033[1;31m"};
  inline static const Color green_bold_color{"\033[1;32m"};
  inline static const Color yellow_bold_color{"\033[1;33m"};
  inline static const Color blue_bold_color{"\033[1;34m"};
  inline static const Color purple_bold_color{"\033[1;35m"};
  inline static const Color cyan_bold_color{"\033[1;36m"};
  inline static const Color white_bold_color{"\033[1;37m"};

private:
  template <typename... Args>
  inline static void Log(LogType type, Args&&... args);

  inline static std::_Put_time<char> CurrentTime();
  inline static const char* GetLogTypeLable(LogType type);
  inline static const char* GetLogTypeColorLable(LogType type);

  inline static auto stream_type_{console};
  constexpr static auto log_file_name_{"log.txt"};

  constexpr static auto empty_lable_{""};
  constexpr static auto info_lable_{"[INFO]:    "};
  constexpr static auto error_lable_{"[ERROR]:   "};
  constexpr static auto debug_lable_{"[DEBUG]:   "};
  constexpr static auto success_lable_{"[SUCCESS]: "};

  constexpr static auto error_color_lable_{"\033[1;91m[ERROR]\033[0m:"};
  constexpr static auto debug_color_lable_{"\033[1;93m[DEBUG]\033[0m:"};
  constexpr static auto success_color_lable_{"\033[1;92m[SUCCESS]\033[0m:"};

  inline static std::unordered_set<LogType> log_types_{all};
  inline static std::ofstream log_file_;
};

inline void Logger::SetStream(StreamType type)
{
  stream_type_ = type;

  switch (stream_type_)
  {
    case StreamType::kNone:
    case StreamType::kConsole:
      log_file_.close();
      break;
    case StreamType::kFile:
      log_file_.open(log_file_name_, std::ios_base::binary | std::ios_base::app);
    default:
      break;
  }
}

inline void Logger::SetLogType(std::initializer_list<LogType> type_list)
{
  log_types_.clear();
  log_types_.insert(type_list);
}

template <typename... Args>
inline void Logger::LogInfo(Args&&... args)
{
  Log(info, args...);
  std::cout << reset_color;
}

template <typename... Args>
inline void Logger::LogDebug(const char* function_name, Args&&... args)
{
  Log(debug, function_name, ":", args...);
  std::cout << reset_color;
}

template <typename... Args>
inline void Logger::LogError(const char* file_name, const char* function_name, int line, Args&&... args)
{
  Log(error, file_name, ":", line, ":", function_name, ":", args...);
  std::cerr << reset_color;
}

inline void Logger::LogSuccess(const char* function_name)
{
  Log(success, function_name);
}

template <typename... Args>
inline void Logger::Log(LogType type, Args&&... args)
{
  if (!log_types_.count(type) && !log_types_.count(all))
  {
    return;
  }

  switch (stream_type_)
  {
    case console:
      (std::cerr << GetLogTypeColorLable(type) << ... << AddSpace(args)) << std::endl;
      break;

    case file:
    {
      if (log_file_.is_open())
      {
        (log_file_ << CurrentTime() << GetLogTypeLable(type) << ... << AddSpace(args)) << std::endl;
      }
    }
    break;

    default:
      break;
  }
}

inline std::_Put_time<char> Logger::CurrentTime()
{
  const auto current_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  return std::put_time(gmtime(&current_time_t), "[%Y-%m-%d %H:%M:%S]");
}

inline const char* Logger::GetLogTypeLable(LogType type)
{
  switch (type)
  {
    case LogType::kInfo:
      return info_lable_;
    case LogType::kDebug:
      return debug_lable_;
    case LogType::kSuccess:
      return success_lable_;
    case LogType::kError:
      return error_lable_;
    default:
      return empty_lable_;
  }
}

inline const char* Logger::GetLogTypeColorLable(LogType type)
{
  switch (type)
  {
    case LogType::kDebug:
      return debug_color_lable_;
    case LogType::kSuccess:
      return success_color_lable_;
    case LogType::kError:
      return error_color_lable_;
    default:
      return empty_lable_;
  }
}

#endif // LOGGER_HPP
