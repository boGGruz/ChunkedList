#pragma once

#include <iterator>
#include <memory>

namespace fefu_laboratory_two {
    template<typename T>
    class Allocator {
    public:
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = T *;
        using const_pointer = const T *;
        using reference = T &;
        using const_reference = const T &;

        Allocator() noexcept = default;

        Allocator(const Allocator &other) noexcept = default;

        template<class U>
        explicit Allocator(const Allocator<U> &other) noexcept {};

        ~Allocator() = default;

        pointer allocate(size_type n) {
            pointer ptr = static_cast<pointer>(malloc(sizeof(value_type) * n));
            return ptr ? ptr : throw std::bad_alloc();
        }

        void deallocate(pointer p) noexcept {
            free(p);
        }
    };

    template<typename ValueType>
    class Chunk {
    public:
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType *;
        using size_type = std::size_t;
        using value_type = ValueType;

        int chunk_size = 0; //Number of elements in chunk
        int current_chunk_size = 0; //Number of already located in chunk
        pointer chunk = nullptr;
        Allocator<value_type> allocator;
        Chunk *prev;
        Chunk *next;

        explicit Chunk(int size) : chunk_size(size) {
            chunk = allocator.allocate(size);
        }

        size_type GetChunkSize() const {
            return chunk_size;
        }

        ValueType &operator[](difference_type index) {
            return chunk[index];
        }

        Chunk(const Chunk<ValueType> &other) noexcept {
            chunk = allocator.allocate(other.chunk_size);
            for (size_type i = 0; i < other.chunk_size; i++) {
                chunk[i] = other.chunk[i];
            }
        }
    };

    template<typename ValueType>
    class ChunkList_iterator {
    protected:
        Chunk<ValueType> *chunk = nullptr; //Chunk, which is pointed by iterator
        int iterator_position = 0; // Iterator position in chunk

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = ValueType;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType *;
        using reference = ValueType &;

        ChunkList_iterator() noexcept = default;

        ChunkList_iterator(const ChunkList_iterator &other) noexcept = default;

        ChunkList_iterator &operator=(const ChunkList_iterator &) = default;

        ~ChunkList_iterator() = default;

        ChunkList_iterator(Chunk<value_type> *current_chunk, std::size_t index) : chunk(current_chunk),
                                                                                  iterator_position(index) {}

        friend void swap(ChunkList_iterator<ValueType> &left, ChunkList_iterator<ValueType> &right) {
            std::swap(left.chunk, right.chunk);
            std::swap(left.iterator_position, right.iterator_position);
        }

        friend bool operator==(const ChunkList_iterator<ValueType> &left, const ChunkList_iterator<ValueType> &right) {
            return left.iterator_position == right.iterator_position;
        }

        friend bool operator!=(const ChunkList_iterator<ValueType> &left, const ChunkList_iterator<ValueType> &right) {
            return left.iterator_position != right.iterator_position;
        }

//        reference operator*() const {
//            return static_cast<reference>(chunk[iterator_position]);
//        }
//
//        pointer operator->() const {
//            return this->chunk[this->iterator_position];
//        }

        ChunkList_iterator &operator++() {
            if (chunk == nullptr)
                throw std::exception();
            int chunk_size = this->chunk->GetChunkSize();
            if (this->iterator_position == chunk_size - 1) {
                this->chunk = nullptr;
                this->iterator_position = 0;
                return *this;
            } else {
                ++this->iterator_position;
                return *this;
            }
        }

        ChunkList_iterator operator++(int) {
            if (chunk == nullptr)
                throw std::exception();
            ChunkList_iterator temp(*this);
            if (iterator_position == chunk->GetChunkSize() - 1) {
                chunk = nullptr;
                iterator_position = 0;
            } else {
                ++this->iterator_position;
            }
            return temp;
        }

        ChunkList_iterator &operator--() {
            if (chunk == nullptr)
                throw std::exception();
            if (this->iterator_position == 0) {
                this->chunk = nullptr;
                return *this;
            } else {
                --this->iterator_position;
                return *this;
            }
        }

        ChunkList_iterator operator--(int) {
            if (chunk == nullptr)
                throw std::exception();
            ChunkList_iterator temp(*this);
            if (iterator_position == 0) {
                chunk = nullptr;
            } else {
                --this->iterator_position;
            }
            return temp;
        }

        ChunkList_iterator operator+(const difference_type &index) const {
            ChunkList_iterator temp(*this);

            if (iterator_position + index >= chunk->GetChunkSize()) {
                temp.chunk = nullptr;
                temp.iterator_position = 0;
            } else {
                temp.iterator_position = iterator_position + index;
            }

            return temp;
        }

        ChunkList_iterator &operator+=(const difference_type &index) {
            if (iterator_position + index >= chunk->GetChunkSize()) {
                chunk = nullptr;
                iterator_position = 0;
            } else {
                iterator_position = iterator_position + index;
            }
            return *this;
        }

        ChunkList_iterator operator-(const difference_type &index) const {
            ChunkList_iterator temp(*this);

            if (iterator_position - index < 0) {
                temp.chunk = nullptr;
                temp.iterator_position = 0;
            } else {
                temp.iterator_position = iterator_position - index;
            }

            return temp;
        }

        ChunkList_iterator &operator-=(const difference_type &index) {
            if (iterator_position - index < 0) {
                chunk = nullptr;
                iterator_position = 0;
            } else {
                iterator_position = iterator_position - index;
            }
            return *this;
        }

        reference operator[](const difference_type &index) {
            return reinterpret_cast<int &>(chunk[index]);
        }

        friend bool operator<(const ChunkList_iterator<ValueType> &left, const ChunkList_iterator<ValueType> &right) {
            return left.iterator_position < right.iterator_position;
        }

        friend bool operator<=(const ChunkList_iterator<ValueType> &left, const ChunkList_iterator<ValueType> &right) {
            return left.iterator_position <= right.iterator_position;
        }

        friend bool operator>(const ChunkList_iterator<ValueType> &left, const ChunkList_iterator<ValueType> &right) {
            return left.iterator_position > right.iterator_position;
        }

        friend bool operator>=(const ChunkList_iterator<ValueType> &left, const ChunkList_iterator<ValueType> &right) {
            return left.iterator_position >= right.iterator_position;
        }
    };

    template<typename ValueType>
    class ChunkList_const_iterator : public ChunkList_iterator<ValueType> {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = ValueType;
        using difference_type = std::ptrdiff_t;
        using pointer = const ValueType *;
        using reference = const ValueType &;

        ChunkList_const_iterator() : ChunkList_iterator<ValueType>() {};

        ChunkList_const_iterator(const ChunkList_const_iterator &other) noexcept = default;

        ChunkList_const_iterator(Chunk<value_type> *current_chunk, std::size_t index) :
                ChunkList_iterator<value_type>(const_cast<Chunk<value_type> *>(current_chunk), index) {}

        ChunkList_const_iterator &operator=(const ChunkList_const_iterator &) = default;

        ~ChunkList_const_iterator() = default;

        friend void swap(ChunkList_const_iterator<ValueType> &left, ChunkList_const_iterator<ValueType> &right) {
            std::swap(left.chunk, right.chunk);
            std::swap(left.iterator_position, right.iterator_position);
        }

        friend bool
        operator==(const ChunkList_const_iterator<ValueType> &left, const ChunkList_const_iterator<ValueType> &right) {
            return left.iterator_position == right.iterator_position;
        }

        friend bool
        operator!=(const ChunkList_const_iterator<ValueType> &left, const ChunkList_const_iterator<ValueType> &right) {
            return left.iterator_position != right.iterator_position;
        }

//        reference operator*() const;
//
//        pointer operator->() const;

        ChunkList_const_iterator &operator++() {
            if (this->chunk == nullptr)
                throw std::exception();
            int chunk_size = this->chunk->GetChunkSize();
            if (this->iterator_position == chunk_size - 1) {
                this->chunk = nullptr;
                this->iterator_position = 0;
                return *this;
            } else {
                ++this->iterator_position;
                return *this;
            }
        }

        ChunkList_const_iterator operator++(int) {
            if (this->chunk == nullptr)
                throw std::exception();
            ChunkList_const_iterator temp(*this);
            if (this->iterator_position == this->chunk->GetChunkSize() - 1) {
                this->chunk = nullptr;
                this->iterator_position = 0;
            } else {
                ++this->iterator_position;
            }
            return temp;
        }

        ChunkList_const_iterator &operator--() {
            if (this->chunk == nullptr)
                throw std::exception();
            if (this->iterator_position == 0) {
                this->chunk = nullptr;
                return *this;
            } else {
                --this->iterator_position;
                return *this;
            }
        }

        ChunkList_const_iterator operator--(int) {
            if (this->chunk == nullptr)
                throw std::exception();
            ChunkList_const_iterator temp(*this);
            if (this->iterator_position == 0) {
                this->chunk = nullptr;
            } else {
                --this->iterator_position;
            }
            return temp;
        }

        ChunkList_const_iterator operator+(const difference_type &index) const {
            ChunkList_const_iterator temp(*this);

            if (this->iterator_position + index >= this->chunk->GetChunkSize()) {
                temp.chunk = nullptr;
                temp.iterator_position = 0;
            } else {
                temp.iterator_position = this->iterator_position + index;
            }

            return temp;
        }

        ChunkList_const_iterator &operator+=(const difference_type &index) {
            if (this->iterator_position + index >= this->chunk->GetChunkSize()) {
                this->chunk = nullptr;
                this->iterator_position = 0;
            } else {
                this->iterator_position = this->iterator_position + index;
            }
            return *this;
        }

        ChunkList_const_iterator operator-(const difference_type &) const {
            ChunkList_const_iterator temp(*this);

            if (this->iterator_position - this->index < 0) {
                temp.chunk = nullptr;
                temp.iterator_position = 0;
            } else {
                temp.iterator_position = this->iterator_position - this->index;
            }

            return temp;
        }

        ChunkList_const_iterator &operator-=(const difference_type &) {
            if (this->iterator_position - this->index < 0) {
                this->chunk = nullptr;
                this->iterator_position = 0;
            } else {
                this->iterator_position = this->iterator_position - this->index;
            }
            return *this;
        }

        reference operator[](const difference_type &index) {
            return reinterpret_cast<int &>(this->chunk[index]);
        }

        friend bool operator<(const ChunkList_const_iterator<ValueType> &left,
                              const ChunkList_const_iterator<ValueType> &right) {
            return left.iterator_position < right.iterator_position;
        }

        friend bool operator<=(const ChunkList_const_iterator<ValueType> &left,
                               const ChunkList_const_iterator<ValueType> &right) {
            return left.iterator_position <= right.iterator_position;
        }

        friend bool operator>(const ChunkList_const_iterator<ValueType> &left,
                              const ChunkList_const_iterator<ValueType> &right) {
            return left.iterator_position > right.iterator_position;
        }

        friend bool operator>=(const ChunkList_const_iterator<ValueType> &left,
                               const ChunkList_const_iterator<ValueType> &right) {
            return left.iterator_position >= right.iterator_position;
        }
    };

    template<typename T, int N, typename Allocator = Allocator<T>>
    class ChunkList {
    public:
        using value_type = T;
        using allocator_type = Allocator;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type &;
        using const_reference = const value_type &;
        using pointer = typename std::allocator_traits<Allocator>::pointer;
        using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
        using iterator = ChunkList_iterator<value_type>;
        using const_iterator = ChunkList_const_iterator<value_type>;

        /// @brief Default constructor. Constructs an empty container with a
        /// default-constructed allocator.
        ChunkList();

        /// @brief Constructs an empty container with the given allocator
        /// @param alloc allocator to use for all memory allocations of this container
        explicit ChunkList(const Allocator &alloc);

        /// @brief Constructs the container with count copies of elements with value
        /// and with the given allocator
        /// @param count the size of the container
        /// @param value the value to initialize elements of the container with
        /// @param alloc allocator to use for all memory allocations of this container
        ChunkList(size_type count, const T &value, const Allocator &alloc = Allocator());

        /// @brief Constructs the container with count default-inserted instances of
        /// T. No copies are made.
        /// @param count the size of the container
        /// @param alloc allocator to use for all memory allocations of this container
        explicit ChunkList(size_type count, const Allocator &alloc = Allocator());

        /// @brief Constructs the container with the contents of the range [first,
        /// last).
        /// @tparam InputIt Input Iterator
        /// @param first, last 	the range to copy the elements from
        /// @param alloc allocator to use for all memory allocations of this container
        template<class InputIt>
        ChunkList(InputIt first, InputIt last, const Allocator &alloc = Allocator());

        /// @brief Copy constructor. Constructs the container with the copy of the
        /// contents of other.
        /// @param other another container to be used as source to initialize the
        /// elements of the container with
        ChunkList(const ChunkList &other);

        /// @brief Constructs the container with the copy of the contents of other,
        /// using alloc as the allocator.
        /// @param other another container to be used as source to initialize the
        /// elements of the container with
        /// @param alloc allocator to use for all memory allocations of this container
        ChunkList(const ChunkList &other, const Allocator &alloc);

        /**
         * @brief Move constructor.
         *
         * Constructs the container with the contents of other using move semantics.
         * Allocator is obtained by move-construction from the allocator belonging to
         * other.
         *
         * @param other another container to be used as source to initialize the
         * elements of the container with
         */
        ChunkList(ChunkList &&other);

        /**
         * @brief Allocator-extended move constructor.
         * Using alloc as the allocator for the new container, moving the contents
         * from other; if alloc != other.get_allocator(), this results in an
         * element-wise move.
         *
         * @param other another container to be used as source to initialize the
         * elements of the container with
         * @param alloc allocator to use for all memory allocations of this container
         */
        ChunkList(ChunkList &&other, const Allocator &alloc);

        /// @brief Constructs the container with the contents of the initializer list
        /// init.
        /// @param init initializer list to initialize the elements of the container
        /// with
        /// @param alloc allocator to use for all memory allocations of this container
        ChunkList(std::initializer_list<T> init, const Allocator &alloc = Allocator());

        /// @brief Destructs the ChunkList.
        ~ChunkList();

        /// @brief Copy assignment operator. Replaces the contents with a copy of the
        /// contents of other.
        /// @param other another container to use as data source
        /// @return *this
        ChunkList &operator=(const ChunkList &other);

        /**
         * Move assignment operator.
         *
         * Replaces the contents with those of other using move semantics
         * (i.e. the data in other is moved from other into this container).
         * other is in a valid but unspecified state afterwards.
         *
         * @param other another container to use as data source
         * @return *this
         */
        ChunkList &operator=(ChunkList &&other);

        /// @brief Replaces the contents with those identified by initializer list
        /// ilist.
        /// @param ilist
        /// @return this
        ChunkList &operator=(std::initializer_list<T> ilist);

        /// @brief Replaces the contents with count copies of value
        /// @param count
        /// @param value
        void assign(size_type count, const T &value);

        /// @brief Replaces the contents with copies of those in the range [first,
        /// last).
        /// @tparam InputIt
        /// @param first
        /// @param last
        template<class InputIt>
        void assign(InputIt first, InputIt last);

        /// @brief Replaces the contents with the elements from the initializer list
        /// ilis
        /// @param ilist
        void assign(std::initializer_list<T> ilist);

        /// @brief Returns the allocator associated with the container.
        /// @return The associated allocator.
        allocator_type get_allocator() const noexcept;

        /// ELEMENT ACCESS

        /// @brief Returns a reference to the element at specified location pos, with
        /// bounds checking. If pos is not within the range of the container, an
        /// exception of type std::out_of_range is thrown.
        /// @param pos position of the element to return
        /// @return Reference to the requested element.
        /// @throw std::out_of_range
        reference at(size_type pos);

        /// @brief Returns a const reference to the element at specified location pos,
        /// with bounds checking. If pos is not within the range of the container, an
        /// exception of type std::out_of_range is thrown.
        /// @param pos position of the element to return
        /// @return Const Reference to the requested element.
        /// @throw std::out_of_range
        const_reference at(size_type pos) const;

        /// @brief Returns a reference to the element at specified location pos. No
        /// bounds checking is performed.
        /// @param pos position of the element to return
        /// @return Reference to the requested element.
        reference operator[](size_type pos);

        /// @brief Returns a const reference to the element at specified location pos.
        /// No bounds checking is performed.
        /// @param pos position of the element to return
        /// @return Const Reference to the requested element.
        const_reference operator[](size_type pos) const;

        /// @brief Returns a reference to the first element in the container.
        /// Calling front on an empty container is undefined.
        /// @return Reference to the first element
        reference front();

        /// @brief Returns a const reference to the first element in the container.
        /// Calling front on an empty container is undefined.
        /// @return Const reference to the first element
        const_reference front() const;

        /// @brief Returns a reference to the last element in the container.
        /// Calling back on an empty container causes undefined behavior.
        /// @return Reference to the last element.
        reference back();

        /// @brief Returns a const reference to the last element in the container.
        /// Calling back on an empty container causes undefined behavior.
        /// @return Const Reference to the last element.
        const_reference back() const;

        /// ITERATORS

        /// @brief Returns an iterator to the first element of the ChunkList.
        /// If the ChunkList is empty, the returned iterator will be equal to end().
        /// @return Iterator to the first element.
        iterator begin() noexcept;

        /// @brief Returns an iterator to the first element of the ChunkList.
        /// If the ChunkList is empty, the returned iterator will be equal to end().
        /// @return Iterator to the first element.
        const_iterator begin() const noexcept;

        /// @brief Same to begin()
        const_iterator cbegin() const noexcept;

        /// @brief Returns an iterator to the element following the last element of
        /// the ChunkList. This element acts as a placeholder; attempting to access it
        /// results in undefined behavior.
        /// @return Iterator to the element following the last element.
        iterator end() noexcept;

        /// @brief Returns an constant iterator to the element following the last
        /// element of the ChunkList. This element acts as a placeholder; attempting to
        /// access it results in undefined behavior.
        /// @return Constant Iterator to the element following the last element.
        const_iterator end() const noexcept;

        /// @brief Same to end()
        const_iterator cend() const noexcept;

        /// CAPACITY

        /// @brief Checks if the container has no elements
        /// @return true if the container is empty, false otherwise
        bool empty() const noexcept;

        /// @brief Returns the number of elements in the container
        /// @return The number of elements in the container.
        size_type size() const noexcept;

        /// @brief Returns the maximum number of elements the container is able to
        /// hold due to system or library implementation limitations
        /// @return Maximum number of elements.
        size_type max_size() const noexcept;

        /// @brief Requests the removal of unused capacity.
        /// It is a non-binding request to reduce the memory usage without changing
        /// the size of the sequence. All iterators and references are invalidated.
        /// Past-the-end iterator is also invalidated.
        void shrink_to_fit();

        /// MODIFIERS

        /// @brief Erases all elements from the container.
        /// nvalidates any references, pointers, or iterators referring to contained
        /// elements. Any past-the-end iterators are also invalidated.
        void clear() noexcept;

        /// @brief Inserts value before pos.
        /// @param pos iterator before which the content will be inserted.
        /// @param value element value to insert
        /// @return Iterator pointing to the inserted value.
        iterator insert(const_iterator pos, const T &value);

        /// @brief Inserts value before pos.
        /// @param pos iterator before which the content will be inserted.
        /// @param value element value to insert
        /// @return Iterator pointing to the inserted value.
        iterator insert(const_iterator pos, T &&value);

        /// @brief Inserts count copies of the value before pos.
        /// @param pos iterator before which the content will be inserted.
        /// @param count number of elements to insert
        /// @param value element value to insert
        /// @return Iterator pointing to the first element inserted, or pos if count
        /// == 0.
        iterator insert(const_iterator pos, size_type count, const T &value);

        /// @brief Inserts elements from range [first, last) before pos.
        /// @tparam InputIt Input Iterator
        /// @param pos iterator before which the content will be inserted.
        /// @param first,last the range of elements to insert, can't be iterators into
        /// container for which insert is called
        /// @return Iterator pointing to the first element inserted, or pos if first
        /// == last.
        template<class InputIt>
        iterator insert(const_iterator pos, InputIt first, InputIt last);

        /// @brief Inserts elements from initializer list before pos.
        /// @param pos iterator before which the content will be inserted.
        /// @param ilist initializer list to insert the values from
        /// @return Iterator pointing to the first element inserted, or pos if ilist
        /// is empty.
        iterator insert(const_iterator pos, std::initializer_list<T> ilist);

        /// @brief Inserts a new element into the container directly before pos.
        /// @param pos iterator before which the new element will be constructed
        /// @param ...args arguments to forward to the constructor of the element
        /// @return terator pointing to the emplaced element.
        template<class... Args>
        iterator emplace(const_iterator pos, Args &&... args);

        /// @brief Removes the element at pos.
        /// @param pos iterator to the element to remove
        /// @return Iterator following the last removed element.
        iterator erase(const_iterator pos);

        /// @brief Removes the elements in the range [first, last).
        /// @param first,last range of elements to remove
        /// @return Iterator following the last removed element.
        iterator erase(const_iterator first, const_iterator last);

        /// @brief Appends the given element value to the end of the container.
        /// The new element is initialized as a copy of value.
        /// @param value the value of the element to append
        void push_back(const T &value);

        /// @brief Appends the given element value to the end of the container.
        /// Value is moved into the new element.
        /// @param value the value of the element to append
        void push_back(T &&value);

        /// @brief Appends a new element to the end of the container.
        /// @param ...args arguments to forward to the constructor of the element
        /// @return A reference to the inserted element.
        template<class... Args>
        reference emplace_back(Args &&... args);

        /// @brief Removes the last element of the container.
        void pop_back();

        /// @brief Prepends the given element value to the beginning of the container.
        /// @param value the value of the element to prepend
        void push_front(const T &value);

        /// @brief Prepends the given element value to the beginning of the container.
        /// @param value moved value of the element to prepend
        void push_front(T &&value);

        /// @brief Inserts a new element to the beginning of the container.
        /// @param ...args arguments to forward to the constructor of the element
        /// @return A reference to the inserted element.
        template<class... Args>
        reference emplace_front(Args &&... args);

        /// @brief Removes the first element of the container.
        void pop_front();

        /// @brief Resizes the container to contain count elements.
        /// If the current size is greater than count, the container is reduced to its
        /// first count elements. If the current size is less than count, additional
        /// default-inserted elements are appended
        /// @param count new size of the container
        void resize(size_type count);

        /// @brief Resizes the container to contain count elements.
        /// If the current size is greater than count, the container is reduced to its
        /// first count elements. If the current size is less than count, additional
        /// copies of value are appended.
        /// @param count new size of the container
        /// @param value the value to initialize the new elements with
        void resize(size_type count, const value_type &value);

        /// @brief Exchanges the contents of the container with those of other.
        /// Does not invoke any move, copy, or swap operations on individual elements.
        /// All iterators and references remain valid. The past-the-end iterator is
        /// invalidated.
        /// @param other container to exchange the contents with
        void swap(ChunkList &other);

        /// COMPARISIONS

        /// @brief Checks if the contents of lhs and rhs are equal
        /// @param lhs,rhs ChunkLists whose contents to compare
//        template<class U, class Alloc>
//        friend bool operator==(const ChunkList<U, Alloc> &lhs,
//                               const ChunkList<U, Alloc> &rhs);
//
//        /// @brief Checks if the contents of lhs and rhs are not equal
//        /// @param lhs,rhs ChunkLists whose contents to compare
//        template<class U, class Alloc>
//        friend bool operator!=(const ChunkList<U, Alloc> &lhs,
//                               const ChunkList<U, Alloc> &rhs);
//
//        /// @brief Compares the contents of lhs and rhs lexicographically.
//        /// @param lhs,rhs ChunkLists whose contents to compare
//        template<class U, class Alloc>
//        friend bool operator>(const ChunkList<U, Alloc> &lhs, const ChunkList<U, Alloc> &rhs);
//
//        /// @brief Compares the contents of lhs and rhs lexicographically.
//        /// @param lhs,rhs ChunkLists whose contents to compare
//        template<class U, class Alloc>
//        friend bool operator<(const ChunkList<U, Alloc> &lhs, const ChunkList<U, Alloc> &rhs);
//
//        /// @brief Compares the contents of lhs and rhs lexicographically.
//        /// @param lhs,rhs ChunkLists whose contents to compare
//        template<class U, class Alloc>
//        friend bool operator>=(const ChunkList<U, Alloc> &lhs,
//                               const ChunkList<U, Alloc> &rhs);
//
//        /// @brief Compares the contents of lhs and rhs lexicographically.
//        /// @param lhs,rhs ChunkLists whose contents to compare
//        template<class U, class Alloc>
//        friend bool operator<=(const ChunkList<U, Alloc> &lhs,
//                               const ChunkList<U, Alloc> &rhs);

        // operator <=> will be handy
    };

/// NON-MEMBER FUNCTIONS

/// @brief  Swaps the contents of lhs and rhs.
/// @param lhs,rhs containers whose contents to swap
    template<class T, int N, class Alloc>
    void swap(ChunkList<T, N, Alloc> &lhs, ChunkList<T, N, Alloc> &rhs);

/// @brief Erases all elements that compare equal to value from the container.
/// @param c container from which to erase
/// @param value value to be removed
/// @return The number of erased elements.
    template<class T, int N, class Alloc, class U>
    typename ChunkList<T, N, Alloc>::size_type erase(ChunkList<T, N, Alloc> &c, const U &value);

/// @brief Erases all elements that compare equal to value from the container.
/// @param c container from which to erase
/// @param pred unary predicate which returns ​true if the element should be
/// erased.
/// @return The number of erased elements.
    template<class T, int N, class Alloc, class Pred>
    typename ChunkList<T, N, Alloc>::size_type erase_if(ChunkList<T, N, Alloc> &c, Pred pred);
}