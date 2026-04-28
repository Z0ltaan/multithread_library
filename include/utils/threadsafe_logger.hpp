#ifndef THREADSAFE_LOGGER_HPP
#define THREADSAFE_LOGGER_HPP

#include <fstream>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <utils/log_level.hpp>

namespace mt
{
  class threadsafe_logger
  {
  public:
    struct output
    {
      std::ofstream out;
      std::mutex mtx;

      output() = default;
      explicit output(std::ofstream out) : out(std::move(out)), mtx() {}
    };

    threadsafe_logger(const std::string& id,
                      std::shared_ptr< output > out_ptr,
                      mt::LOG_LEVEL lowest_to_log = LOG_LEVEL::ERROR);

    mt::LOG_LEVEL log_level();
    void set_log_level(mt::LOG_LEVEL log_level);
    void log(const std::string& message, mt::LOG_LEVEL log_level);
    void trace(const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void fatal(const std::string& message);

  private:
    mt::LOG_LEVEL log_level_;
    const std::shared_ptr< output > output_ptr_;
    const std::string identifier_;

    const inline static std::map< mt::LOG_LEVEL, std::string >
      log_stamp_factory_ = {
        { mt::LOG_LEVEL::TRACE, "[ TRACE ]" },
        { mt::LOG_LEVEL::DEBUG, "[ DEBUG ]" },
        { mt::LOG_LEVEL::INFO, "[ INFO ]" },
        { mt::LOG_LEVEL::WARNING, "[ WARNING ]" },
        { mt::LOG_LEVEL::ERROR, "[ ERROR ]" },
        { mt::LOG_LEVEL::FATAL, "[ FATAL ]" },
      };
  };
} // namespace mt

#endif
