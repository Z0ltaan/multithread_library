#ifndef THREADSAFE_QUEUE_HPP
#define THREADSAFE_QUEUE_HPP

#include <algorithm>
#include <memory>
#include <mutex>

namespace mt
{
  template< class T >
  class threadsafe_queue
  {
    struct node
    {
      T data;
      std::unique_ptr< node > next;

      node() : data(), next() {}
      node(T data) : data(std::move(data)), next() {}
    };

  public:
    threadsafe_queue() : head_(std::make_unique< node >()), tail_(head_.get())
    {}

    threadsafe_queue(const threadsafe_queue& rhs) = delete;
    threadsafe_queue(threadsafe_queue&& other) = delete;

    threadsafe_queue& operator=(const threadsafe_queue& rhs) = delete;
    threadsafe_queue& operator=(threadsafe_queue&& other) = delete;

    void push(T rhs)
    {
      std::unique_ptr< node > new_dummy = std::make_unique< node >();
      std::lock_guard tail_lock{ tail_mutex_ };

      tail_->data = std::move(rhs);
      tail_->next = std::move(new_dummy);
      tail_ = tail_->next.get();
    }

    bool try_pop(T& lhs)
    {
      std::unique_ptr< node > popped_head = try_pop_head();
      if (!popped_head.get())
      {
        return false;
      }
      lhs = std::move(popped_head->data);
      return true;
    }

    bool empty() const
    {
      std::lock_guard lock{ head_mutex_ };
      return head_.get() == get_tail();
    }

  private:
    std::unique_ptr< node > head_;
    std::mutex head_mutex_;

    node* tail_;
    std::mutex tail_mutex_;

    node* get_tail()
    {
      std::lock_guard tail_lock{ tail_mutex_ };
      return tail_;
    }

    std::unique_ptr< node > try_pop_head()
    {
      std::lock_guard head_lock{ head_mutex_ };
      if (head_.get() == get_tail())
      {
        return std::unique_ptr< node >{};
      }

      std::unique_ptr< node > tmp = std::move(head_);
      head_ = std::move(tmp->next);
      return tmp;
    }
  };
} // namespace mt

#endif
