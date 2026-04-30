#ifndef THREADSAFE_LOGGER_HPP
#define THREADSAFE_LOGGER_HPP

#include <atomic>
#include <fstream>
#include <map>
#include <memory>
#include <mutex>
#include <string>

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

    enum LOG_LEVEL
    {
      TRACE,
      DEBUG,
      INFO,
      WARNING,
      ERROR,
      FATAL,
      OFF
    };

    threadsafe_logger(const std::string& id,
                      std::shared_ptr< output > out_ptr,
                      LOG_LEVEL lowest_to_log = LOG_LEVEL::ERROR);

    LOG_LEVEL log_level();
    void set_log_level(LOG_LEVEL log_level);
    void log(const std::string& message, LOG_LEVEL log_level);
    void trace(const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void fatal(const std::string& message);

  private:
    std::atomic< LOG_LEVEL > log_level_;
    const std::shared_ptr< output > output_ptr_;
    const std::string identifier_;

    const inline static std::map< LOG_LEVEL, std::string >
      log_stamp_factory_ = {
        { LOG_LEVEL::TRACE, "[ TRACE ]" },
        { LOG_LEVEL::DEBUG, "[ DEBUG ]" },
        { LOG_LEVEL::INFO, "[ INFO ]" },
        { LOG_LEVEL::WARNING, "[ WARNING ]" },
        { LOG_LEVEL::ERROR, "[ ERROR ]" },
        { LOG_LEVEL::FATAL, "[ FATAL ]" },
      };
  };
} // namespace mt

#endif
