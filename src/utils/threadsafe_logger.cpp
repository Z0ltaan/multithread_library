#include <atomic>
#include <mutex>
#include <string>
#include <utils/threadsafe_logger.hpp>

mt::threadsafe_logger::threadsafe_logger(const std::string& id,
                                         std::shared_ptr< output > out_ptr,
                                         LOG_LEVEL lowest_to_log) :
  log_level_(lowest_to_log), output_ptr_(std::move(out_ptr)), identifier_(id)
{}

mt::threadsafe_logger::LOG_LEVEL
mt::threadsafe_logger::log_level()
{
  return log_level_.load(std::memory_order_relaxed);
}

void
mt::threadsafe_logger::set_log_level(LOG_LEVEL log_level)
{
  log_level_.store(log_level, std::memory_order_relaxed);
}

void
mt::threadsafe_logger::log(const std::string& message, LOG_LEVEL log_level)
{
  if (log_level >= log_level_.load(std::memory_order_relaxed))
  {
    std::string formated_message = log_stamp_factory_.at(log_level) + " " +
                                   identifier_ + ": " + message + '\n';

    std::lock_guard output_lock{ output_ptr_->mtx };
    output_ptr_->out << formated_message;
  }
}

void
mt::threadsafe_logger::trace(const std::string& message)
{
  this->log(message, LOG_LEVEL::TRACE);
}

void
mt::threadsafe_logger::debug(const std::string& message)
{
  this->log(message, LOG_LEVEL::DEBUG);
}

void
mt::threadsafe_logger::info(const std::string& message)
{
  this->log(message, LOG_LEVEL::INFO);
}

void
mt::threadsafe_logger::warning(const std::string& message)
{
  this->log(message, LOG_LEVEL::WARNING);
}

void
mt::threadsafe_logger::error(const std::string& message)
{
  this->log(message, LOG_LEVEL::ERROR);
}

void
mt::threadsafe_logger::fatal(const std::string& message)
{
  this->log(message, LOG_LEVEL::FATAL);
  output_ptr_->out.flush();
}
