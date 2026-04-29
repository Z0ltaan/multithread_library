#include <algorithm>
#include <containers/threadsafe_queue.hpp>
#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <thread>
#include <utils/threadsafe_logger.hpp>
#include <vector>

namespace foo {
struct message {
  std::string content;
};

void consumer_func(
    size_t iterations,
    std::shared_ptr<mt::threadsafe_queue<foo::message>> queue_ptr,
    std::shared_ptr<mt::threadsafe_logger::output> output_ptr) {
  foo::message dummy{};
  std::string empty{""};
  mt::threadsafe_logger logger("Consumer", output_ptr,
                               mt::threadsafe_logger::LOG_LEVEL::INFO);
  for (size_t i = 0; i < iterations; ++i) {
    queue_ptr->try_pop(dummy);
    logger.info(dummy.content);
  }
}

void supplier_func(
    size_t iterations,
    std::shared_ptr<mt::threadsafe_queue<foo::message>> queue_ptr,
    std::shared_ptr<mt::threadsafe_logger::output> output_ptr) {
  mt::threadsafe_logger logger("Supplier", output_ptr,
                               mt::threadsafe_logger::LOG_LEVEL::INFO);
  for (size_t i = 0; i < iterations; ++i) {
    queue_ptr->push(foo::message{"message1\n"});
    logger.info("pushed");
  }
}
template <class Callable, class... Args>
void launch_threads(std::vector<std::thread> &pool, size_t thread_count,
                    Callable thread_func, Args &&...args) {
  for (size_t i = 0; i < thread_count; ++i) {
    pool.emplace_back(thread_func, std::forward<Args>(args)...);
  }
}

void join_threads(std::vector<std::thread> &pool) {
  std::for_each(pool.begin(), pool.end(), [](auto &thread) { thread.join(); });
}

std::filesystem::path get_program_dir(const char *argv0) {
  return std::filesystem::path(argv0).parent_path();
}

} // namespace foo

int main(int argc, char **argv) {
  std::shared_ptr<mt::threadsafe_queue<foo::message>> queue_ptr =
      std::make_shared<mt::threadsafe_queue<foo::message>>();

  size_t supplier_count = 4;
  size_t consumer_count = supplier_count;

  std::vector<std::thread> suppliers;
  std::vector<std::thread> consumers;

  suppliers.reserve(supplier_count);
  consumers.reserve(consumer_count);

  size_t iterations_count = 10000;

  {
    std::shared_ptr<mt::threadsafe_logger::output> output_ptr =
        std::make_shared<mt::threadsafe_logger::output>(
            std::ofstream(foo::get_program_dir(argv[0]) / "log.txt"));
    launch_threads(suppliers, supplier_count, foo::supplier_func,
                   iterations_count, queue_ptr, output_ptr);
    launch_threads(consumers, consumer_count, foo::consumer_func,
                   iterations_count, queue_ptr, output_ptr);
  }

  foo::join_threads(suppliers);
  foo::join_threads(consumers);
}
