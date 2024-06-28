#include <memory>  // 使用智能指针
#include <cstddef>  // 使用标准类型大小
#include <iterator>  // 迭代器操作

namespace sjtu {

template <typename T>
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

}  // namespace sjtu