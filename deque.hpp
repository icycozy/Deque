#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <cmath>
#include <iterator>
#include <memory>

namespace sjtu {

template<typename T>
class double_list {
private:
    struct Node {
        std::shared_ptr<T> data;  // 使用 shared_ptr 管理内存
        Node* pre;
        Node* nxt;
        Node(const T& val)
            : data(std::make_shared<T>(val)), pre(nullptr), nxt(nullptr) {}
    };

    Node* head;
    Node* tail;
    std::size_t size_;  // 跟踪列表大小

public:
    double_list() : head(nullptr), tail(nullptr), size_(0) {}

    double_list(const double_list<T>& other) {
        head = tail = nullptr;  // 初始化
        Node* current = other.head;  // 从另一个列表复制
        while (current) {
            insert_tail(*current->data);  // 插入尾部
            current = current->nxt;
        }
        size_ = other.size_;  // 更新大小
    }

    ~double_list() {
        clear();  // 清理所有节点
    }

    class iterator {
    public:
        using difference_type = std::ptrdiff_t;  // 添加 difference_type
        using iterator_category = std::bidirectional_iterator_tag;  // 迭代器类别
        using value_type = T;  // 迭代器的值类型
        using pointer = T*;  // 指向迭代器值的指针
        using reference = T&;  // 对迭代器值的引用

        Node* current;  // 当前节点
        iterator() : current(nullptr) {}
        iterator(Node* node) : current(node) {}

        iterator operator++(int) {  // 后缀增量
            iterator temp = *this;
            if (current) {
                current = current->nxt;  // 移动到下一个节点
            } else {
                throw std::out_of_range("Iterator out of range");
            }
            return temp;
        }

        iterator& operator++() {  // 前缀增量
            if (current) {
                current = current->nxt;  // 移动到下一个节点
            } else {
                throw std::out_of_range("Iterator out of range");
            }
            return *this;
        }

        iterator operator--(int) {  // 后缀减量
            iterator temp = *this;
            if (current && current->pre) {  // 检查边界
                current = current->pre;  // 移动到前一个节点
            } else {
                throw std::out_of_range("Iterator out of range");
            }
            return temp;
        }

        iterator& operator--() {  // 前缀减量
            if (current && current->pre) {  // 检查边界
                current = current->pre;  // 移动到前一个节点
            } else {
                throw std::out_of_range("Iterator out of range");
            }
            return *this;
        }

        T& operator*() const {  // 解引用
            if (!current) {
                throw std::out_of_range("Iterator out of range");
            }
            return *current->data;  // 返回数据
        }

        T* operator->() const noexcept {  // 指针访问
            return current ? current->data.get() : nullptr;  // 返回指向数据的指针
        }

        bool operator==(const iterator& rhs) const {  // 检查相等
            return current == rhs.current;
        }

        bool operator!=(const iterator& rhs) const {  // 检查不等
            return current != rhs.current;
        }
    };

    iterator begin() const {
        return iterator(head);  // 返回开始迭代器
    }
    iterator get_tail() const {
        return iterator(tail); 
    }
    iterator end() const {
        return iterator(nullptr);  // 返回结束迭代器
    }

    iterator erase(iterator pos) {
        if (!pos.current) {
            throw std::out_of_range("");  // 如果无效，返回结束迭代器
        }

        Node* node_to_delete = pos.current;
        if (node_to_delete->pre) {  // 调整链接
            node_to_delete->pre->nxt = node_to_delete->nxt;
        } else {
            head = node_to_delete->nxt;  // 如果在头部
        }

        if (node_to_delete->nxt) {  // 调整链接
            node_to_delete->nxt->pre = node_to_delete->pre;
        } else {
            tail = node_to_delete->pre;  // 如果在尾部
        }

        Node *node = node_to_delete->nxt;
        delete node_to_delete;  // 释放节点
        size_--;  // 更新大小

        return iterator(node);  // 返回下一个迭代器
    }

    iterator insert(iterator pos, const T& value) {
        if(pos == end()) {
            insert_tail(value);
            return iterator(tail);
        }

        if (!pos.current) {  // 检查有效性
            throw std::out_of_range("Invalid iterator");
        }

        Node* new_node = new Node(value);  // 创建新节点
        Node* current = pos.current;

        if (current->pre) {  // 如果不是在头部
            current->pre->nxt = new_node;
            new_node->pre = current->pre;
        } else {
            head = new_node;  // 如果在头部
        }

        new_node->nxt = current;
        current->pre = new_node;  // 连接节点
        size_++;  // 更新大小

        return iterator(new_node);  // 返回新迭代器
    }

    void insert_head(const T& value) {
        Node* new_node = new Node(value);  // 创建新节点
        if (!head) {  // 如果是空列表
            head = tail = new_node;
        } else {
            head->pre = new_node;
            new_node->nxt = head;
            head = new_node;
        }
        size_++;  // 更新大小
    }

    void insert_tail(const T& value) {
        Node* new_node = new Node(value);  // 创建新节点
        if (!tail) {  // 如果是空列表
            head = tail = new_node;
        } else {
            tail->nxt = new_node;
            new_node->pre = tail;
            tail = new_node;
        }
        size_++;  // 更新大小
    }

    void delete_head() {
        if (!head) {
            return;
        }

        Node* node_to_delete = head;  // 要删除的节点
        head = head->nxt;

        if (head) {
            head->pre = nullptr;
        } else {
            tail = nullptr;  // 如果是最后一个节点
        }

        delete node_to_delete;  // 释放内存
        size_--;  // 更新大小
    }

    void delete_tail() {
        if (!tail) {
            return;
        }

        Node* node_to_delete = tail;  // 要删除的节点
        tail = tail->pre;

        if (tail) {
            tail->nxt = nullptr;
        } else {
            head = nullptr;  // 如果是最后一个节点
        }

        delete node_to_delete;  // 释放内存
        size_--;  // 更新大小
    }

    bool empty() const {
        return size_ == 0;  // 检查是否为空
    }

    std::size_t size() const {
        return size_;  // 返回大小
    }

    void clear() {
        while (head != nullptr) {  // 确保正确迭代
            Node* temp = head;
            head = head->nxt;
            delete temp;  // 删除当前节点
        }
        tail = nullptr;  // 设置尾指针为 nullptr
        size_ = 0;  // 重置大小
    }
    
    T& front() {
        if (!head) {
            throw std::out_of_range("List is empty");  // 检查是否为空
        }
        return *head->data;  // 返回第一个元素
    }

    const T& front() const {
        if (!head) {
            throw std::out_of_range("List is empty");  // 检查是否为空
        }
        return *head->data;  // 返回第一个元素
    }

    T& back() {
        if (!tail) {
            throw std::out_of_range("List is empty");  // 检查是否为空
        }
        return *tail->data;  // 返回最后一个元素
    }

    const T& back() const {
        if (!tail) {
            throw std::out_of_range("List is empty");  // 检查是否为空
        }
        return *tail->data;  // 返回最后一个元素
    }
};

template <class T> class deque {
private:
  struct Block {
    double_list<T> items;
  };
  double_list<Block> blocks;
  size_t total_size = 0;
  size_t block_size = 4; // 

  void Balance() {
    if (blocks.empty()) return;

    size_t new_block_size = std::sqrt(total_size) + 1;
    if (new_block_size != block_size) {
      block_size = new_block_size;
      
      auto it = blocks.begin();
      while (it != blocks.end()) {
        if (it->items.size() > block_size * 2) {
          Split(it);
        }
        else if ((it->items.size()) * 2 < block_size) {
          Merge(it);
        }
        ++it;
      }
    }
  }

  void Merge(typename double_list<Block>::iterator it) {
    if (it == blocks.end()) return;
    auto& current_block = *it;
    ++it;
    if (it == blocks.end()) return;
    auto& next_block = *it;
    
    if (current_block.items.size() + next_block.items.size() <= block_size) {
      auto next_it = next_block.items.begin();
      while (next_it != next_block.items.end()) {
        current_block.items.insert_tail(*next_it);
        ++next_it;
      }
      blocks.erase(it);
    }
  }

  void Split(typename double_list<Block>::iterator it) {
    if (it->items.size() <= block_size) return;

    Block new_block;
    auto split_it = it->items.begin();
    std::advance(split_it, it->items.size() / 2);

    // Move half of the items to the new block
    while (split_it != it->items.end()) {
        new_block.items.insert_tail(*split_it);
        split_it = it->items.erase(split_it);
    }

    ++it;
    if(it != blocks.end()) blocks.insert(it, new_block);
    else blocks.insert_tail(new_block);
  }

public:
  class const_iterator;
  class iterator {
  public:
    typename double_list<T>::iterator item_it;
    typename double_list<Block>::iterator block_it;
    deque *outer;

    iterator() : item_it(), block_it(), outer(nullptr) {}

    iterator(const iterator& other) 
      : item_it(other.item_it), block_it(other.block_it), outer(other.outer) {}

    iterator(typename double_list<T>::iterator i_it, typename double_list<Block>::iterator b_it, deque *out):
      item_it(i_it), block_it(b_it), outer(out){}

    iterator& operator=(const iterator& other) {
      if (this != &other) {
        item_it = other.item_it;
        block_it = other.block_it;
        outer = other.outer;
      }
      return *this;
    }
    /**
     * return a new iterator which points to the n-next element.
     * if there are not enough elements, the behaviour is undefined.
     * same for operator-.
     */
    iterator operator+(const int &n) const {
      if(n<0) return *this - (-n);

      iterator temp = *this;
      if (n == 0) return temp;
      if(temp.block_it == nullptr) throw std::out_of_range("");

      int remain = n;
      int block_remaining = std::distance(temp.item_it, temp.block_it->items.end());
      if(remain<block_remaining) {
        std::advance(temp.item_it, remain);
        return temp;
      }

      remain -= block_remaining;
      if(!remain && temp.block_it == outer->blocks.get_tail())
        return outer->end();
      
      ++temp.block_it;
      if(temp.block_it == outer->blocks.end()) throw std::out_of_range("");
      temp.item_it = temp.block_it->items.begin();
      while (remain > 0) {       
        if (remain < temp.block_it->items.size())
          break;
        else {
          remain -= temp.block_it->items.size();   
          if (temp.block_it != outer->blocks.get_tail()) {
            ++temp.block_it;
            temp.item_it = temp.block_it->items.begin();
          }
          else {
            if(remain)
              throw std::out_of_range("");
            else temp.item_it = temp.block_it->items.end();
          }
        }
      }

      std::advance(temp.item_it, remain);
      return temp;
    }
    // additional iterator operations

    iterator operator-(const int &n) const {
      if(n<0) return *this + (-n);

      iterator temp = *this; 
      if (n == 0) return temp;

      int remain = n;
      if(temp == outer->end()) {
        temp.block_it = outer->blocks.get_tail();
        temp.item_it = temp.block_it->items.get_tail();
        --remain;
      }

      int block_remaining = std::distance(temp.block_it->items.begin(),temp.item_it);
      if(remain<=block_remaining){
        while(remain--)
          --temp.item_it;
        return temp;
      }
      if(temp.block_it==outer->blocks.begin()) throw std::out_of_range("");
      remain-=block_remaining;
      --remain;
      --temp.block_it;
      temp.item_it = temp.block_it->items.get_tail();

      while (remain > 0) {
        if(remain<temp.block_it->items.size())
          break;
           
        if(temp.block_it==outer->blocks.begin()) throw std::out_of_range("");
        remain-=temp.block_it->items.size();
        --temp.block_it;
        temp.item_it = temp.block_it->items.get_tail();
      } 

      while(remain--){
        --temp.item_it;
      }
      return temp;
    }

    /**
     * return the distance between two iterators.
     * if they point to different vectors, throw
     * invaild_iterator.
     */
    size_t operator-(const iterator& rhs) const {
      if (block_it==nullptr || rhs.block_it==nullptr)
        throw std::out_of_range("");       

      size_t distance = 0;
      auto temp_block_it = rhs.block_it;
      auto temp_item_it = rhs.item_it;     

      if (block_it == rhs.block_it) {
        bool f=0;
        while(temp_item_it!=item_it){
          if(temp_item_it == temp_block_it->items.end()){
            f=1;
            break;
          }
          distance++;
          ++temp_item_it;
        }
        if(f) return -std::distance(item_it, rhs.item_it);
        else return distance;
      }

      distance += std::distance(temp_item_it, temp_block_it->items.end());
      ++temp_block_it;
      
      bool f=1;
      while (temp_block_it != block_it) {
        if(temp_block_it == rhs.outer->blocks.end()){
          f=0;
          break;
        }        
        distance += temp_block_it->items.size();
        ++temp_block_it;
      }
      if(f) {
        temp_item_it = temp_block_it->items.begin();       
        distance += std::distance(temp_item_it, item_it);
        return distance;
      }

      distance = 0;
      temp_block_it = block_it;
      temp_item_it = item_it;  
      distance += std::distance(temp_item_it, temp_block_it->items.end());
      ++temp_block_it;
      
      f=1;
      while (temp_block_it != rhs.block_it) {
        if(temp_block_it == outer->blocks.end()){
          f=0;
          break;
        }        
        distance += temp_block_it->items.size();
        ++temp_block_it;
      }
      if(f) {
        temp_item_it = temp_block_it->items.begin();       
        distance += std::distance(temp_item_it, rhs.item_it);
        return -distance;
      }
      else  throw std::out_of_range("");
    }

    iterator operator+=(const int &n) {
      *this = *this + n;
      return *this;
    }

    iterator operator-=(const int &n) {
      *this = *this - n;
      return *this;
    }

    /**
     * iter++
     */
    iterator operator++(int) {
      iterator old = *this;
      if(block_it==outer->blocks.end()) throw std::out_of_range("");
      if(block_it == outer->blocks.get_tail() && item_it == block_it->items.end())
        throw std::out_of_range("");
      if(item_it == block_it->items.end())
        throw std::out_of_range("");
      ++item_it;
      if(item_it == block_it->items.end() && block_it != outer->blocks.get_tail()){
        ++block_it;
        item_it = block_it->items.begin();
      }        
      return old;
    }
    /**
     * ++iter
     */
    iterator &operator++() {
      if(block_it==outer->blocks.end()) throw std::out_of_range("");
      if(block_it == outer->blocks.get_tail() && item_it == block_it->items.end())
        throw std::out_of_range("");
      if(item_it == block_it->items.end())
        throw std::out_of_range("");
      ++item_it;
      if(item_it == block_it->items.end() && block_it != outer->blocks.get_tail()){
        ++block_it;
        item_it = block_it->items.begin();
      } 
      return *this;
    }
    iterator operator--(int) {
      iterator old = *this;
      if(*this == outer->end()) {
        block_it = outer->blocks.get_tail();
        item_it = block_it->items.get_tail();
        return old;
      }

      if(block_it==outer->blocks.begin() && item_it==block_it->items.begin())
        throw std::out_of_range("");

      if(item_it == block_it->items.end())
        throw std::out_of_range("");
            
      if(item_it == block_it->items.begin()) {
        --block_it;
        item_it = block_it->items.get_tail();
      }
      else --item_it;

      return old;
    }
    iterator &operator--() {
      if(*this == outer->end()) {
        block_it = outer->blocks.get_tail();
        item_it = block_it->items.get_tail();
        return *this;
      }     

      if(block_it==outer->blocks.begin() && item_it==block_it->items.begin())
        throw std::out_of_range("");

      if(item_it == block_it->items.end())
        throw std::out_of_range("");
            
      if(item_it == block_it->items.begin()) {
        --block_it;
        item_it = block_it->items.get_tail();
      }
      else --item_it;

      return *this;
    }

    // *it
    T &operator*() const {return *item_it;}
    // it->field
    T *operator->() const noexcept {return &(*item_it);}

    /**
     * check whether two iterators are the same (pointing to the same
     * memory).
     */
    bool operator==(const iterator &rhs) const {
      return block_it == rhs.block_it && item_it == rhs.item_it;
    }
    bool operator==(const const_iterator &rhs) const {
      return block_it == rhs.block_it && item_it == rhs.item_it;
    }
    bool operator!=(const iterator &rhs) const {
      return block_it != rhs.block_it || item_it !=rhs.item_it;
    }
    bool operator!=(const const_iterator &rhs) const {
      return block_it != rhs.block_it || item_it !=rhs.item_it;
    }
  };

  class const_iterator {
  public:
    typename double_list<T>::iterator item_it;
    typename double_list<Block>::iterator block_it;
    const deque *outer;

    const_iterator() : item_it(), block_it(), outer(nullptr) {}

    const_iterator(const iterator &it):
      item_it(it.item_it), block_it(it.block_it), outer(it.outer) {}
    const_iterator(const const_iterator& other) 
      : item_it(other.item_it), block_it(other.block_it), outer(other.outer) {}

    const_iterator(typename double_list<T>::iterator i_it, typename double_list<Block>::iterator b_it, const deque *out):
      item_it(i_it), block_it(b_it), outer(out){}

    const_iterator& operator=(const const_iterator& other) {
      if (this != &other) {
        item_it = other.item_it;
        block_it = other.block_it;
        outer = other.outer;
      }
      return *this;
    }

    const_iterator operator+(const int &n) const {
      if(n<0) return *this - (-n);

      const_iterator temp = *this;
      if (n == 0) return temp;
      if(temp.block_it == nullptr) throw std::out_of_range("");

      int remain = n;
      int block_remaining = std::distance(temp.item_it, temp.block_it->items.end());
      if(remain<block_remaining) {
        std::advance(temp.item_it, remain);
        return temp;
      }

      remain -= block_remaining;
      if(!remain && temp.block_it == outer->blocks.get_tail())
        return outer->cend();
      
      ++temp.block_it;
      if(temp.block_it == outer->blocks.end()) throw std::out_of_range("");
      temp.item_it = temp.block_it->items.begin();
      while (remain > 0) {       
        if (remain < temp.block_it->items.size())
          break;
        else {
          remain -= temp.block_it->items.size();   
          if (temp.block_it != outer->blocks.get_tail()) {
            ++temp.block_it;
            temp.item_it = temp.block_it->items.begin();
          }
          else {
            if(remain)
              throw std::out_of_range("");
            else temp.item_it = temp.block_it->items.end();
          }
        }
      }

      std::advance(temp.item_it, remain);
      return temp;
    }
    // additional iterator operations

    const_iterator operator-(const int &n) const {
      if(n<0) return *this + (-n);

      const_iterator temp = *this; 
      if (n == 0) return temp;

      int remain = n;
      if(temp == outer->cend()) {
        temp.block_it = outer->blocks.get_tail();
        temp.item_it = temp.block_it->items.get_tail();
        --remain;
      }

      int block_remaining = std::distance(temp.block_it->items.begin(),temp.item_it);
      if(remain<=block_remaining){
        while(remain--)
          --temp.item_it;
        return temp;
      }
      if(temp.block_it==outer->blocks.begin()) throw std::out_of_range("");
      remain-=block_remaining;
      --remain;
      --temp.block_it;
      temp.item_it = temp.block_it->items.get_tail();

      while (remain > 0) {
        if(remain<temp.block_it->items.size())
          break;
           
        if(temp.block_it==outer->blocks.begin()) throw std::out_of_range("");
        remain-=temp.block_it->items.size();
        --temp.block_it;
        temp.item_it = temp.block_it->items.get_tail();
      } 

      while(remain--){
        --temp.item_it;
      }
      return temp;
    }

    const size_t operator-(const const_iterator& rhs) const {
      if (block_it==nullptr || rhs.block_it==nullptr)
        throw std::out_of_range("");      

      size_t distance = 0;
      auto temp_block_it = rhs.block_it;
      auto temp_item_it = rhs.item_it;     

      if (block_it == rhs.block_it) {
        bool f=0;
        while(temp_item_it!=item_it){
          if(temp_item_it == temp_block_it->items.end()){
            f=1;
            break;
          }
          distance++;
          ++temp_item_it;
        }
        if(f) return -std::distance(item_it, rhs.item_it);
        else return distance;
      }

      distance += std::distance(temp_item_it, temp_block_it->items.end());
      ++temp_block_it;
      
      bool f=1;
      while (temp_block_it != block_it) {
        if(temp_block_it == rhs.outer->blocks.end()){
          f=0;
          break;
        }        
        distance += temp_block_it->items.size();
        ++temp_block_it;
      }
      if(f) {
        temp_item_it = temp_block_it->items.begin();       
        distance += std::distance(temp_item_it, item_it);
        return distance;
      }

      distance = 0;
      temp_block_it = block_it;
      temp_item_it = item_it;  
      distance += std::distance(temp_item_it, temp_block_it->items.end());
      ++temp_block_it;
      
      f=1;
      while (temp_block_it != rhs.block_it) {
        if(temp_block_it == outer->blocks.end()){
          f=0;
          break;
        }        
        distance += temp_block_it->items.size();
        ++temp_block_it;
      }
      if(f) {
        temp_item_it = temp_block_it->items.begin();       
        distance += std::distance(temp_item_it, rhs.item_it);
        return -distance;
      }
      else throw std::out_of_range("");
    }

    const_iterator operator+=(const int &n) {
      *this = *this + n;
      return *this;
    }

    const_iterator operator-=(const int &n) {
      *this = *this - n;
      return *this;
    }

    /**
     * iter++
     */
    const_iterator operator++(int) {
      const_iterator old = *this;
      if(block_it==outer->blocks.end()) throw std::out_of_range("");
      if(block_it == outer->blocks.get_tail() && item_it == block_it->items.end())
        throw std::out_of_range("");
      if(item_it == block_it->items.end())
        throw std::out_of_range("");
      ++item_it;
      if(item_it == block_it->items.end() && block_it != outer->blocks.get_tail()){
        ++block_it;
        item_it = block_it->items.begin();
      }        
      return old;
    }

    const_iterator &operator++() {
      if(block_it==outer->blocks.end()) throw std::out_of_range("");
      if(block_it == outer->blocks.get_tail() && item_it == block_it->items.end())
        throw std::out_of_range("");
      if(item_it == block_it->items.end())
        throw std::out_of_range("");
      ++item_it;
      if(item_it == block_it->items.end() && block_it != outer->blocks.get_tail()){
        ++block_it;
        item_it = block_it->items.begin();
      } 
      return *this;
    }
    const_iterator operator--(int) {
      const_iterator old = *this;
      if(*this == outer->end()) {
        block_it = outer->blocks.get_tail();
        item_it = block_it->items.get_tail();
        return old;
      }

      if(block_it==outer->blocks.begin() && item_it==block_it->items.begin())
        throw std::out_of_range("");

      if(item_it == block_it->items.end())
        throw std::out_of_range("");
            
      if(item_it == block_it->items.begin()) {
        --block_it;
        item_it = block_it->items.get_tail();
      }
      else --item_it;

      return old;
    }
    const_iterator &operator--() {
      if(*this == outer->cend()) {
        block_it = outer->blocks.get_tail();
        item_it = block_it->items.get_tail();
        return *this;
      }     

      if(block_it==outer->blocks.begin() && item_it==block_it->items.begin())
        throw std::out_of_range("");

      if(item_it == block_it->items.end())
        throw std::out_of_range("");
            
      if(item_it == block_it->items.begin()) {
        --block_it;
        item_it = block_it->items.get_tail();
      }
      else --item_it;

      return *this;
    }

    // *it
    const T &operator*() const {return *item_it;}
    // it->field
    const T *operator->() const noexcept {return &(*item_it);}

    /**
     * check whether two iterators are the same (pointing to the same
     * memory).
     */
    const bool operator==(const iterator &rhs) const {
      return block_it == rhs.block_it && item_it == rhs.item_it;
    }
    const bool operator==(const const_iterator &rhs) const {
      return block_it == rhs.block_it && item_it == rhs.item_it;
    }
    const bool operator!=(const iterator &rhs) const {
      return block_it != rhs.block_it || item_it !=rhs.item_it;
    }
    const bool operator!=(const const_iterator &rhs) const {
      return block_it != rhs.block_it || item_it !=rhs.item_it;
    }
  };

  deque(): total_size(0), block_size(4) {}
  deque(const deque &other) {
    clear(); 
    total_size = other.total_size; 
    block_size = other.block_size; 

    for (auto it = other.blocks.begin(); it != other.blocks.end(); ++it) {
      Block new_block;
      for (auto item_it = it->items.begin(); item_it != it->items.end(); ++item_it) {
        new_block.items.insert_tail(*item_it);
      }
      blocks.insert_tail(new_block); 
    }
  }

  ~deque() {
    clear(); 
  }

  deque& operator=(const deque& other) const {
    if (this == &other) return *this;
    clear(); 
    total_size = other.total_size; 
    block_size = other.block_size;
    for (auto it = other.blocks.begin(); it != other.blocks.end(); ++it) {
      Block new_block;
      for (auto item_it = it->items.begin(); item_it != it->items.end(); ++item_it) {
        new_block.items.insert_tail(*item_it); 
      }
      blocks.insert_tail(new_block);
    }

    return *this;
  }

  T& at(const size_t& pos) {
    if (pos >= total_size) {
      throw std::out_of_range("");
    }

    size_t current_pos = 0;
    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
      if (current_pos + it->items.size() > pos) {
        auto item_it = it->items.begin();
        std::advance(item_it, pos - current_pos);  // 找到正确的项
        return *item_it;
      }
      current_pos += it->items.size();
    }
    throw std::out_of_range("");
  }
  const T &at(const size_t &pos) const {
    if (pos >= total_size) {
      throw std::out_of_range("");
    }

    size_t current_pos = 0;
    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
      if (current_pos + it->items.size() > pos) {
        auto item_it = it->items.begin();
        std::advance(item_it, pos - current_pos);  // 找到正确的项
        return *item_it;
      }
      current_pos += it->items.size();
    }
    throw std::out_of_range("");
  }
  T &operator[](const size_t &pos) {
    return at(pos);
  }
  const T &operator[](const size_t &pos) const {
    return at(pos);
  }

  /**
   * access the first element.
   * throw container_is_empty when the container is empty.
   */
  const T &front() const {
    if (blocks.empty()) {
      throw std::out_of_range("");
    }
    return blocks.front().items.front();
  }
  const T &back() const {
    if (blocks.empty()) {
      throw std::out_of_range("");
    }
    return blocks.back().items.back();  
  }

  iterator begin() {
    if (blocks.empty()) {
      return iterator(nullptr, nullptr, this);
    }
    return iterator(blocks.begin()->items.begin(), blocks.begin(), this);
  }
  const_iterator cbegin() const {
    if (blocks.empty()) {
      return const_iterator(nullptr, nullptr, this);
    }
    return const_iterator(blocks.begin()->items.begin(), blocks.begin(), this);
  }

  iterator end() {
    if (blocks.empty()) {
      return iterator(nullptr, nullptr, this);
    }
    return iterator(blocks.back().items.end(), blocks.get_tail(), this);
  }

  const_iterator cend() const {
    if (blocks.empty()) {
      return const_iterator(nullptr, nullptr, this);
    }
    return const_iterator(blocks.back().items.end(), blocks.get_tail(), this);   
  }

  bool empty() const { return total_size == 0; }
  size_t size() const { return total_size; }

  void clear() {
    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
      it->items.clear();
    }
    blocks.clear();
    total_size = 0;
    block_size = 4;
  }


  /**
   * insert value before pos.
   * return an iterator pointing to the inserted value.
   * throw if the iterator is invalid or it points to a wrong place.
   */
  
  iterator insert(iterator pos, const T& value) {
    if (blocks.empty()) {
      if(pos!=end()) throw std::out_of_range("");
      blocks.insert_tail(Block{}); 
      blocks.back().items.insert_tail(value);
      total_size++;
      return iterator(blocks.back().items.begin(), blocks.get_tail(), this);
    }

    if (pos.block_it == nullptr) { 
      throw std::out_of_range("");
    }

    size_t dis = pos - (this->begin());
    if(dis>total_size) throw std::out_of_range("");
    auto block_it = pos.block_it;  
    auto item_it = block_it->items.insert(pos.item_it, value); 
    total_size++;  

    Balance(); 
    
    block_it = blocks.begin();
    while(dis >= block_it->items.size()){
      dis -= block_it->items.size();   
      if(block_it == blocks.get_tail()) break;
      ++block_it;
    }

    item_it = block_it->items.begin();
    std::advance(item_it, dis);

    return iterator(item_it, block_it, this);  // 返回指向插入位置的 iterator
  }


  iterator erase(iterator pos) {
    if (pos.block_it == nullptr) {
      throw std::out_of_range("Invalid iterator");
    }

    size_t dis = pos - (this->begin());
    if(dis>=total_size) throw std::out_of_range("");

    auto block_it = pos.block_it;
    auto item_it = block_it->items.erase(pos.item_it); 
    total_size--; 

    if (block_it->items.empty()) {
      blocks.erase(block_it); 
    }

    if(empty() || dis==total_size)
      return end();

    Balance();     
    
    block_it = blocks.begin();
    while(dis >= block_it->items.size()){
      dis -= block_it->items.size();
      if(block_it == blocks.get_tail()) break;
      ++block_it;
    }

    item_it = block_it->items.begin();
    std::advance(item_it, dis);

    return iterator(item_it, block_it, this); 
  }

  void push_back(const T &value) {
    if (blocks.empty() || blocks.back().items.size() >= block_size) {
      blocks.insert_tail(Block{});
    }
    blocks.back().items.insert_tail(value);
    total_size++;
    Balance();
  }
  void pop_back() {
    if (empty()) throw std::out_of_range("");

    blocks.back().items.delete_tail();
    if (blocks.back().items.empty())
      blocks.delete_tail();
    total_size--;
    Balance();
  }

  void push_front(const T &value) {
    if (blocks.empty() || blocks.front().items.size() >= block_size) {
      blocks.insert_head(Block{});
    }
    blocks.front().items.insert_head(value);
    total_size++;
    Balance();
  }
  void pop_front() {
    if (empty()) throw std::out_of_range("");

    blocks.front().items.delete_head();
    if (blocks.front().items.empty())
      blocks.delete_head();
    total_size--;
    Balance();
  }
};

} // namespace sjtu

#endif
