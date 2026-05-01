#include <containers/threadsafe_queue.hpp>
#include <gtest/gtest.h>
#include <thread>

namespace mt::tests
{
  class threadsafe_queue_fixture: public testing::Test
  {
  protected:
    mt::threadsafe_queue< int > queue;
  };

  class threadsafe_queue_fixture_with_pushed_and_result
    : public mt::tests::threadsafe_queue_fixture
  {
  protected:
    int result = 0;
    const int pushed_value = 1;
  };

  TEST_F(threadsafe_queue_fixture_with_pushed_and_result, push_try_pop)
  {
    queue.push(pushed_value);

    EXPECT_FALSE(queue.empty());
    EXPECT_TRUE(queue.try_pop(result));
    EXPECT_EQ(result, pushed_value);
  }

  TEST_F(threadsafe_queue_fixture_with_pushed_and_result, push_wait_and_pop)
  {
    queue.push(pushed_value);

    EXPECT_FALSE(queue.empty());
    queue.wait_and_pop(result);
    EXPECT_EQ(result, pushed_value);
  }

  TEST_F(threadsafe_queue_fixture_with_pushed_and_result, empty_try_pop)
  {
    EXPECT_TRUE(queue.empty());
    EXPECT_FALSE(queue.try_pop(result));
  }

  TEST_F(threadsafe_queue_fixture, concurrent_wait_pop_and_push)
  {
    constexpr int push_number = 1000;

    bool correct_queue_ordering = true;
    int popped_count = 0;

    std::thread consumer(
      [this, push_number, &correct_queue_ordering, &popped_count]
      {
        int dummy = 0;
        for (popped_count = 0; popped_count < push_number; ++popped_count)
        {
          queue.wait_and_pop(dummy);
          if (dummy != popped_count)
          {
            correct_queue_ordering = false;
            break;
          }
        }
      });

    std::thread producer(
      [this, push_number]
      {
        for (int i = 0; i < push_number; ++i)
        {
          queue.push(i);
        }
      });

    producer.join();
    consumer.join();

    EXPECT_TRUE(correct_queue_ordering);
    EXPECT_EQ(push_number, popped_count);
  }
}
