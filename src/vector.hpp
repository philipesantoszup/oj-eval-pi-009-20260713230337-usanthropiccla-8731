#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>
#include <cstring>

namespace sjtu
{

template<typename T>
class vector
{
private:
    T *data_;
    size_t size_;
    size_t capacity_;

    void expand() {
        size_t new_cap = (capacity_ == 0) ? 1 : capacity_ * 2;
        T *new_data = (T*) operator new(new_cap * sizeof(T));
        for (size_t i = 0; i < size_; ++i) {
            new (new_data + i) T(data_[i]);
            data_[i].~T();
        }
        operator delete(data_);
        data_ = new_data;
        capacity_ = new_cap;
    }

    void ensure_capacity(size_t needed) {
        if (needed <= capacity_) return;
        size_t new_cap = (capacity_ == 0) ? 1 : capacity_;
        while (new_cap < needed) new_cap *= 2;
        T *new_data = (T*) operator new(new_cap * sizeof(T));
        for (size_t i = 0; i < size_; ++i) {
            new (new_data + i) T(data_[i]);
            data_[i].~T();
        }
        operator delete(data_);
        data_ = new_data;
        capacity_ = new_cap;
    }

public:
    class const_iterator;
    class iterator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::output_iterator_tag;

    private:
        T *ptr_;
        const vector<T> *vec_;

    public:
        iterator() : ptr_(nullptr), vec_(nullptr) {}
        iterator(T *ptr, const vector<T> *vec) : ptr_(ptr), vec_(vec) {}

        iterator operator+(const int &n) const {
            return iterator(ptr_ + n, vec_);
        }
        iterator operator-(const int &n) const {
            return iterator(ptr_ - n, vec_);
        }
        int operator-(const iterator &rhs) const {
            if (vec_ != rhs.vec_) throw invalid_iterator();
            return (int)(ptr_ - rhs.ptr_);
        }
        iterator& operator+=(const int &n) {
            ptr_ += n;
            return *this;
        }
        iterator& operator-=(const int &n) {
            ptr_ -= n;
            return *this;
        }
        iterator operator++(int) {
            iterator tmp = *this;
            ++ptr_;
            return tmp;
        }
        iterator& operator++() {
            ++ptr_;
            return *this;
        }
        iterator operator--(int) {
            iterator tmp = *this;
            --ptr_;
            return tmp;
        }
        iterator& operator--() {
            --ptr_;
            return *this;
        }
        T& operator*() const {
            return *ptr_;
        }
        bool operator==(const iterator &rhs) const {
            return ptr_ == rhs.ptr_;
        }
        bool operator==(const const_iterator &rhs) const {
            return ptr_ == rhs.ptr_;
        }
        bool operator!=(const iterator &rhs) const {
            return ptr_ != rhs.ptr_;
        }
        bool operator!=(const const_iterator &rhs) const {
            return ptr_ != rhs.ptr_;
        }

        T* get_ptr() const { return ptr_; }
        const vector<T>* get_vec() const { return vec_; }

        friend class const_iterator;
    };

    class const_iterator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::output_iterator_tag;

    private:
        T *ptr_;
        const vector<T> *vec_;

    public:
        const_iterator() : ptr_(nullptr), vec_(nullptr) {}
        const_iterator(T *ptr, const vector<T> *vec) : ptr_(ptr), vec_(vec) {}
        const_iterator(const iterator &it) : ptr_(it.ptr_), vec_(it.vec_) {}

        const_iterator operator+(const int &n) const {
            return const_iterator(ptr_ + n, vec_);
        }
        const_iterator operator-(const int &n) const {
            return const_iterator(ptr_ - n, vec_);
        }
        int operator-(const const_iterator &rhs) const {
            if (vec_ != rhs.vec_) throw invalid_iterator();
            return (int)(ptr_ - rhs.ptr_);
        }
        const_iterator& operator+=(const int &n) {
            ptr_ += n;
            return *this;
        }
        const_iterator& operator-=(const int &n) {
            ptr_ -= n;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++ptr_;
            return tmp;
        }
        const_iterator& operator++() {
            ++ptr_;
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator tmp = *this;
            --ptr_;
            return tmp;
        }
        const_iterator& operator--() {
            --ptr_;
            return *this;
        }
        const T& operator*() const {
            return *ptr_;
        }
        bool operator==(const iterator &rhs) const {
            return ptr_ == rhs.ptr_;
        }
        bool operator==(const const_iterator &rhs) const {
            return ptr_ == rhs.ptr_;
        }
        bool operator!=(const iterator &rhs) const {
            return ptr_ != rhs.ptr_;
        }
        bool operator!=(const const_iterator &rhs) const {
            return ptr_ != rhs.ptr_;
        }

        friend class iterator;
    };

    vector() : data_(nullptr), size_(0), capacity_(0) {}

    vector(const vector &other) : data_(nullptr), size_(0), capacity_(0) {
        if (other.size_ > 0) {
            data_ = (T*) operator new(other.capacity_ * sizeof(T));
            capacity_ = other.capacity_;
            for (size_t i = 0; i < other.size_; ++i) {
                new (data_ + i) T(other.data_[i]);
            }
            size_ = other.size_;
        }
    }

    ~vector() {
        for (size_t i = 0; i < size_; ++i) {
            data_[i].~T();
        }
        operator delete(data_);
    }

    vector &operator=(const vector &other) {
        if (this == &other) return *this;
        // Destroy existing
        for (size_t i = 0; i < size_; ++i) {
            data_[i].~T();
        }
        operator delete(data_);
        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
        if (other.size_ > 0) {
            data_ = (T*) operator new(other.capacity_ * sizeof(T));
            capacity_ = other.capacity_;
            for (size_t i = 0; i < other.size_; ++i) {
                new (data_ + i) T(other.data_[i]);
            }
            size_ = other.size_;
        }
        return *this;
    }

    T & at(const size_t &pos) {
        if (pos >= size_) throw index_out_of_bound();
        return data_[pos];
    }
    const T & at(const size_t &pos) const {
        if (pos >= size_) throw index_out_of_bound();
        return data_[pos];
    }

    T & operator[](const size_t &pos) {
        if (pos >= size_) throw index_out_of_bound();
        return data_[pos];
    }
    const T & operator[](const size_t &pos) const {
        if (pos >= size_) throw index_out_of_bound();
        return data_[pos];
    }

    const T & front() const {
        if (size_ == 0) throw container_is_empty();
        return data_[0];
    }

    const T & back() const {
        if (size_ == 0) throw container_is_empty();
        return data_[size_ - 1];
    }

    iterator begin() {
        return iterator(data_, this);
    }
    const_iterator begin() const {
        return const_iterator(data_, this);
    }
    const_iterator cbegin() const {
        return const_iterator(data_, this);
    }

    iterator end() {
        return iterator(data_ + size_, this);
    }
    const_iterator end() const {
        return const_iterator(data_ + size_, this);
    }
    const_iterator cend() const {
        return const_iterator(data_ + size_, this);
    }

    bool empty() const {
        return size_ == 0;
    }

    size_t size() const {
        return size_;
    }

    void clear() {
        for (size_t i = 0; i < size_; ++i) {
            data_[i].~T();
        }
        size_ = 0;
    }

    iterator insert(iterator pos, const T &value) {
        size_t ind = pos.get_ptr() - data_;
        if (size_ == capacity_) {
            expand();
        }
        // Shift elements right
        for (size_t i = size_; i > ind; --i) {
            new (data_ + i) T(data_[i - 1]);
            data_[i - 1].~T();
        }
        new (data_ + ind) T(value);
        ++size_;
        return iterator(data_ + ind, this);
    }

    iterator insert(const size_t &ind, const T &value) {
        if (ind > size_) throw index_out_of_bound();
        if (size_ == capacity_) {
            expand();
        }
        for (size_t i = size_; i > ind; --i) {
            new (data_ + i) T(data_[i - 1]);
            data_[i - 1].~T();
        }
        new (data_ + ind) T(value);
        ++size_;
        return iterator(data_ + ind, this);
    }

    iterator erase(iterator pos) {
        size_t ind = pos.get_ptr() - data_;
        data_[ind].~T();
        for (size_t i = ind; i < size_ - 1; ++i) {
            new (data_ + i) T(data_[i + 1]);
            data_[i + 1].~T();
        }
        --size_;
        return iterator(data_ + ind, this);
    }

    iterator erase(const size_t &ind) {
        if (ind >= size_) throw index_out_of_bound();
        data_[ind].~T();
        for (size_t i = ind; i < size_ - 1; ++i) {
            new (data_ + i) T(data_[i + 1]);
            data_[i + 1].~T();
        }
        --size_;
        return iterator(data_ + ind, this);
    }

    void push_back(const T &value) {
        if (size_ == capacity_) {
            expand();
        }
        new (data_ + size_) T(value);
        ++size_;
    }

    void pop_back() {
        if (size_ == 0) throw container_is_empty();
        data_[size_ - 1].~T();
        --size_;
    }
};

}

#endif
