#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sstream>
#include <thread>
#include <utils/threadsafe_logger.hpp>
#include <vector>

namespace mt::tests
{
  using namespace testing;

  class threadsafe_logger_fixture: public Test
  {
  protected:
    std::stringstream oss;
    std::shared_ptr< threadsafe_logger::output > output_ptr;

    std::unique_ptr< threadsafe_logger > logger;

    void SetUp() override
    {
      output_ptr = std::make_shared< threadsafe_logger::output >(oss);
      logger = std::make_unique< threadsafe_logger >(
        "TEST_ID", output_ptr, threadsafe_logger::LOG_LEVEL::ERROR);
    }
  };

  TEST_F(threadsafe_logger_fixture, doesnt_log_below_threshold)
  {
    logger->info("This should be ignored");
    EXPECT_TRUE(oss.str().empty());
  }

  TEST_F(threadsafe_logger_fixture, logs_with_correct_format)
  {
    std::string msg = "Critical failure";
    logger->error(msg);

    std::string expected = "[ ERROR ] TEST_ID: " + msg + "\n";
    EXPECT_EQ(oss.str(), expected);
  }

  TEST_F(threadsafe_logger_fixture, changes_log_level_dynamically)
  {
    logger->set_log_level(threadsafe_logger::LOG_LEVEL::TRACE);
    logger->trace("Now visible");

    EXPECT_THAT(oss.str(), HasSubstr("[ TRACE ]"));
    EXPECT_THAT(oss.str(), HasSubstr("Now visible"));
  }

  TEST_F(threadsafe_logger_fixture, concurrent_logging)
  {
    const int thread_count = 10;
    const int messages_per_thread = 100;
    logger->set_log_level(threadsafe_logger::LOG_LEVEL::TRACE);

    std::vector< std::thread > threads;
    for (int i = 0; i < thread_count; ++i)
    {
      threads.emplace_back(
        [this, i]()
        {
          for (int j = 0; j < messages_per_thread; ++j)
          {
            logger->info("Thread " + std::to_string(i) + " msg " +
                         std::to_string(j));
          }
        });
    }

    for (auto& t: threads)
    {
      t.join();
    }

    std::string line;
    int line_count = 0;
    while (std::getline(oss, line))
      line_count++;

    EXPECT_EQ(line_count, thread_count * messages_per_thread);
  }

  TEST_F(threadsafe_logger_fixture, RespectsOffLevel)
  {
    logger->set_log_level(threadsafe_logger::LOG_LEVEL::OFF);
    logger->fatal("This should never appear");
    EXPECT_TRUE(oss.str().empty());
  }
}
