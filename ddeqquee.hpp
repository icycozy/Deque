#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include "double_list.hpp"
#include <cstddef>
#include <cmath>
#include <memory>

namespace sjtu {

template <class T> class deque {
private:
  struct Block {
    double_list<T> items;
  };
  
  double_list<Block> blocks;
  size_t total_size = 0;
  size_t block_size = 4; // Initial block size

  // Function to balance the blocks
  void Balance() {
    if (blocks.empty()) return;

    size_t expected_block_size = std::sqrt(total_size) + 1;
    if (expected_block_size != block_size) {
      block_size = expected_block_size;
      
      auto it = blocks.begin();
      while (it != blocks.end()) {
        if (it->items.size() > block_size) {
          Split(it);
        } else if (it->items.size() < block_size / 2) {
          Merge(it);
        }
        ++it;
      }
    }
  }

  // Merge small blocks
  void Merge(typename double_list<Block>::iterator it) {
    if (it == blocks.end() || std::next(it) == blocks.end()) return;

    auto& current_block = *it;
    auto& next_block = *std::next(it);
    
    if (current_block.items.size() + next_block.items.size() <= block_size) {
      current_block.items.splice(current_block.items.end(), next_block.items);
      blocks.erase(std::next(it));
    }
  }

  // Split large blocks
  void Split(typename double_list<Block>::iterator it) {
    if (it->items.size() <= block_size) return;

    double_list<T> new_block_items;
    auto split_it = it->items.begin();
    std::advance(split_it, it->items.size() / 2);

    new_block_items.splice(new_block_items.begin(), it->items, split_it, it->items.end());

    Block new_block;
    new_block.items = new_block_items;

    blocks.insert(std::next(it), new_block);
  }

public:
  class const_iterator;
  class iterator {
  private:
    typename double_list<T>::iterator item_it;
    typename double_list<Block>::iterator block_it;
    deque *const outer;

  public:
    iterator(typename double_list<T>::iterator i_it, typename double_list<Block>::iterator b_it, deque *const out)
      : item_it(i_it), block_it(b_it), outer(out) {}

    // Additional iterator operations, like + and - for moving across blocks
    iterator operator+(const int &n) const {
      if (n == 0) return *this;
      
      iterator temp = *this;
      int remain = n;
      while (remain > 0) {
        int block_remaining = std::distance(temp.item_it, temp.block_it->items.end());
        if (remain < block_remaining) {
          std::advance(temp.item_it, remain);
          break;
        } else {
          remain -= block_remaining;
          ++temp.block_it;
          if (temp.block_it != outer->blocks.end()) {
            temp.item_it = temp.block_it->items.begin();
          } else {
            throw sjtu::index_out_of_bound("out of bounds");
          }
        }
      }
      return temp;
    }

    iterator operator-(const int &n) const {
      if (n == 0) return *this;

      iterator temp = *this;
      int remain = n;
      while (remain > 0) {
        if (temp.item_it == temp.block_it->items.begin()) {
          if (temp.block_it == outer->blocks.begin()) {
            throw sjtu::index_out_of_bound("out of bounds");
          }
          --temp.block_it;
          temp.item_it = temp.block_it->items.end();
        }
        
        --temp.item_it;
        --remain;
      }
      
      return temp;
    }

    iterator operator+=(const int &n) {
      *this = *this + n;
      return *this;
    }

    iterator operator-=(const int &n) {
      *this = *this - n;
      return *this;
    }

    iterator operator++(int) {
      iterator old = *this;
      ++(*this);
      return old;
    }

    iterator &operator++() {
      ++item_it;
      if (item_it == block_it->items.end()) {
        if (++block_it != outer->blocks.end()) {
          item_it = block_it->items.begin();
        }
      }
      return *this;
    }

    iterator operator--(int) {
      iterator old = *this;
      --(*this);
      return old;
    }

    iterator &operator--() {
      if (item_it == block_it->items.begin()) {
        if (block_it == outer->blocks.begin()) {
          throw sjtu::index_out_of_bound("out of bounds");
        }
        --block_it;
        item_it = block_it->items.end();
      }
      --item_it;
      return *this;
    }

    T &operator*() const {
      return *item_it;
    }

    T *operator->() const noexcept {
      return &(*item_it);
    }

    bool operator==(const iterator &rhs) const {
      return block_it == rhs.block_it && item_it == rhs.item_it;
    }

    bool operator!=(const iterator &rhs) const {
      return !(*this == rhs);
    }

    friend class const_iterator;
  };

  class const_iterator {
  private:
    typename double_list<T>::const_iterator item_it;
    typename double_list<Block>::const_iterator block_it;

  public:
    const_iterator(typename double_list<T>::const_iterator i_it, typename double_list<Block>::const_iterator b_it)
      : item_it(i_it), block_it(b_it) {}

    const_iterator(const iterator &other)
      : item_it(other.item_it), block_it(other.block_it) {}

    const_iterator operator+(const int &n) const {
      iterator iter(item_it, block_it, nullptr);
      return const_iterator(iter + n);
    }

    const_iterator operator-(const int &n) const {
      iterator iter(item_it, block_it, nullptr);
      return const_iterator(iter - n);
    }

    const_iterator operator++(int) {
      const_iterator old = *this;
      ++(*this);
      return old;
    }

    const_iterator &operator++() {
      ++item_it;
      if (item_it == block_it->items.end()) {
        ++block_it;
        if (block_it != nullptr) {
          item_it = block_it->items.begin();
        }
      }
      return *this;
    }

    const_iterator operator--(int) {
      const_iterator old = *this;
      --(*this);
      return old;
    }

    const_iterator &operator--() {
      if (item_it == block_it->items.begin()) {
        if (block_it == nullptr) {
          throw sjtu::index_out_of_bound("out of bounds");
        }
        --block_it;
        item_it = block_it->items.end();
      }
      --item_it;
      return *this;
    }

    const T &operator*() const {
      return *item_it;
    }

    const T *operator->() const noexcept {
      return &(*item_it);
    }

    bool operator==(const const_iterator &rhs) const {
      return block_it == rhs.block_it && item_it == rhs.item_it;
    }

    bool operator!=(const const_iterator &rhs) const {
      return !(*this == rhs);
    }
  };

  deque() {}

  deque(const deque &other) {
    total_size = other.total_size;
    block_size = other.block_size;
    
    blocks = other.blocks;
  }

  deque &operator=(const deque &other) {
    if (this != &other) {
      total_size = other.total_size;
      block_size = other.block_size;
      blocks = other.blocks;
    }
    return *this;
  }

  ~deque() {}

  size_t size() const {
    return total_size;
  }

  bool empty() const {
    return total_size == 0;
  }

  iterator begin() {
    if (blocks.empty()) {
      return iterator(nullptr, nullptr, this);
    }
    return iterator(blocks.front().items.begin(), blocks.begin(), this);
  }

  iterator end() {
    if (blocks.empty()) {
      return iterator(nullptr, nullptr, this);
    }
    return iterator(blocks.back().items.end(), blocks.end(), this);
  }

  const_iterator cbegin() const {
    if (blocks.empty()) {
      return const_iterator(nullptr, nullptr);
    }
    return const_iterator(blocks.front().items.begin(), blocks.begin());
  }

  const_iterator cend() const {
    if (blocks.empty()) {
      return const_iterator(nullptr, nullptr);
    }
    return const_iterator(blocks.back().items.end(), blocks.end());
  }

  T &at(const size_t &pos) {
    if (pos >= total_size) {
      throw sjtu::index_out_of_bound("Index out of bound");
    }

    auto block_it = blocks.begin();
    size_t remaining = pos;

    while (block_it != blocks.end() && remaining >= block_it->items.size()) {
      remaining -= block_it->items.size();
      ++block_it;
    }

    auto item_it = block_it->items.begin();
    std::advance(item_it, remaining);

    return *item_it;
  }

  const T &at(const size_t &pos) const {
    return const_cast<deque*>(this)->at(pos);
  }

  T &operator[](const size_t &pos) {
    return at(pos);
  }

  const T &operator[](const size_t &pos) const {
    return at(pos);
  }

  const T &front() const {
    if (empty()) {
      throw sjtu::container_is_empty("Deque is empty");
    }
    return blocks.front().items.front();
  }

  const T &back() const {
    if (empty()) {
      throw sjtu::container_is_empty("Deque is empty");
    }
    return blocks.back().items.back();
  }

  void clear() {
    blocks.clear();
    total_size = 0;
    block_size = 4;
  }

  void push_back(const T &value) {
    if (blocks.empty() || blocks.back().items.size() >= block_size) {
      blocks.push_back(Block{});
    }
    blocks.back().items.push_back(value);
    total_size++;
    Balance();
  }

  void pop_back() {
    if (empty()) {
      throw sjtu::container_is_empty("Deque is empty");
    }
    blocks.back().items.pop_back();
    if (blocks.back().items.empty()) {
      blocks.pop_back();
    }
    total_size--;
    Balance();
  }

  void push_front(const T &value) {
    if (blocks.empty() || blocks.front().items.size() >= block_size) {
      blocks.push_front(Block{});
    }
    blocks.front().items.push_front(value);
    total_size++;
    Balance();
  }

  void pop_front() {
    if (empty()) {
      throw sjtu::container_is_empty("Deque is empty");
    }
    blocks.front().items.pop_front();
    if (blocks.front().items.empty()) {
      blocks.pop_front();
    }
    total_size--;
    Balance();
  }

  iterator insert(iterator pos, const T &value) {
    if (pos.block_it == nullptr) {
      throw sjtu::invalid_iterator("Invalid iterator");
    }

    pos.block_it->items.insert(pos.item_it, value);
    total_size++;
    Balance();
    return iterator(pos.item_it, pos.block_it, this);
  }

  iterator erase(iterator pos) {
    if (pos.block_it == nullptr) {
      throw sjtu::invalid_iterator("Invalid iterator");
    }

    pos.block_it->items.erase(pos.item_it);
    total_size--;
    
    if (pos.block_it->items.empty()) {
      blocks.erase(pos.block_it);
    }

    Balance();
    
    auto next_block_it = std::next(pos.block_it);
    if (next_block_it != blocks.end()) {
      return iterator(next_block_it->items.begin(), next_block_it, this);
    } else {
      return end();
    }
  }
};

} // namespace sjtu

#endif
