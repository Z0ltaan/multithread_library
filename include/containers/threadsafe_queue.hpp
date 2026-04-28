#ifndef THREADSAFE_QUEUE_HPP
#define THREADSAFE_QUEUE_HPP

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
    threadsafe_queue() :
      dummy_head_(std::make_unique< node >()), tail_(dummy_head_.get())
    {}
    // threadsafe_queue(const threadsafe_queue& rhs) = delete;
    // threadsafe_queue(threadsafe_queue&& other) {}
    // threadsafe_queue& operator=(const threadsafe_queue& rhs) = delete;
    // threadsafe_queue& operator=(threadsafe_queue&& other) {}

    void push(T rhs)
    {
      std::unique_ptr< node > new_node =
        std::make_unique< node >(std::ref(rhs));

      std::lock_guard tail_lock{ tail_mutex_ };
      tail_->next = std::move(new_node);
      tail_ = tail_->next.get();
    }

    // std::unique_ptr< T > try_pop() {

    // std::unique_ptr< node > head = try_pop_head();
    // return head.get() == nullptr ? std::unique_ptr< T >{}
    //                              : std::make_unique(std::move(head->data));
    // }

    bool try_pop(T& lhs)
    {
      std::unique_ptr< node > head = try_pop_head();
      if (head.get() == nullptr)
      {
        return false;
      }
      lhs = std::move(head->data);
      return true;
    }

    bool empty() const
    {
      std::scoped_lock lock{ head_mutex_, tail_mutex_ };
      return tail_ == dummy_head_.get();
    }

  private:
    std::unique_ptr< node > dummy_head_;
    std::mutex head_mutex_;

    node* tail_;
    std::mutex tail_mutex_;

    std::unique_ptr< node > try_pop_head()
    {
      std::lock_guard head_lock{ head_mutex_ };
      if (dummy_head_.get() == tail_)
      {
        return std::unique_ptr< node >{};
      }

      std::lock_guard tail_lock{ tail_mutex_ };
      std::unique_ptr< node > tmp = std::move(dummy_head_->next);
      dummy_head_->next = std::move(tmp->next);

      if (tmp.get() == tail_)
      {
        tail_ = dummy_head_.get();
      }
      return tmp;
    }
  };
} // namespace mt

#endif
