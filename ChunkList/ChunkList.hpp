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
        using reference = ValueType &;

        int chunk_size = 0; //Number of elements in chunk
        int current_chunk_size = 0; //Number of already located in chunk
        pointer chunk = nullptr;
        Allocator<value_type> allocator;
        Chunk *prev = nullptr;
        Chunk *next = nullptr;

        Chunk() = default;

        explicit Chunk(int size) : chunk_size(size) {
            chunk = allocator.allocate(size);
        }

        Chunk(int size, Allocator<ValueType> allocator1) {
            chunk_size = size;
            chunk = allocator1.allocate(size);
            allocator = allocator1;
        }

        size_type GetChunkSize() const {
            return chunk_size;
        }

        ValueType &operator[](difference_type index) {
            return chunk[index];
        }

        reference at(size_type position) {
            if (position >= chunk_size || position < 0) {
                throw std::out_of_range("Position is out of range!");
            }
            return chunk[position];
        }

        Chunk(const Chunk<ValueType> &other) noexcept {
            chunk = allocator.allocate(other.chunk_size);
            chunk_size = other.chunk_size;
            current_chunk_size = other.current_chunk_size;
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
        ValueType *current_value = nullptr;

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

        ChunkList_iterator(Chunk<value_type> *current_chunk, std::size_t index, value_type *value) : chunk(
                current_chunk),
                                                                                                     iterator_position(
                                                                                                             index),
                                                                                                     current_value(
                                                                                                             value) {}

        friend void swap(ChunkList_iterator<ValueType> &left, ChunkList_iterator<ValueType> &right) {
            std::swap(left.chunk, right.chunk);
            std::swap(left.iterator_position, right.iterator_position);
            std::swap(left.current_value, right.current_value);
        }

        friend bool operator==(const ChunkList_iterator<ValueType> &left, const ChunkList_iterator<ValueType> &right) {
            return left.current_value == right.current_value;
        }

        friend bool operator!=(const ChunkList_iterator<ValueType> &left, const ChunkList_iterator<ValueType> &right) {
            return left.current_value != right.current_value;
        }

        reference operator*() const {
            return *current_value;
        }

        pointer operator->() const {
            return current_value;
        }

        ChunkList_iterator &operator++() {
            if (chunk == nullptr)
                throw std::exception();
            int chunk_size = this->chunk->GetChunkSize();
            if (this->iterator_position == chunk_size - 1) {
                this->chunk = nullptr;
                this->iterator_position = 0;
                this->current_value = nullptr;
                return *this;
            } else {
                ++iterator_position;
                current_value = &chunk->at(iterator_position);
                return *this;
            }
        }

        ChunkList_iterator operator++(int) {
            if (chunk == nullptr)
                throw std::exception();
            ChunkList_iterator temp(*this);
            if (iterator_position == chunk->GetChunkSize() - 1) {
                chunk = nullptr;
                current_value = nullptr;
                iterator_position = 0;
            } else {
                ++this->iterator_position;
                current_value = &chunk->at(iterator_position);
            }
            return temp;
        }

        ChunkList_iterator &operator--() {
            if (chunk == nullptr)
                throw std::exception();
            if (this->iterator_position == 0) {
                this->chunk = nullptr;
                current_value = nullptr;
                return *this;
            } else {
                --this->iterator_position;
                current_value = &chunk->at(iterator_position);
                return *this;
            }
        }

        ChunkList_iterator operator--(int) {
            if (chunk == nullptr)
                throw std::exception();
            ChunkList_iterator temp(*this);
            if (iterator_position == 0) {
                chunk = nullptr;
                current_value = nullptr;
            } else {
                --this->iterator_position;
                current_value = &chunk->at(iterator_position);
            }
            return temp;
        }

        ChunkList_iterator operator+(const difference_type &index) const {
            ChunkList_iterator temp(*this);

            if (iterator_position + index >= chunk->GetChunkSize()) {
                temp.chunk = nullptr;
                temp.iterator_position = 0;
                temp.current_value = nullptr;
            } else {
                temp.iterator_position = iterator_position + index;
                temp.current_value = temp.chunk->at(iterator_position);
            }

            return temp;
        }

        ChunkList_iterator &operator+=(const difference_type &index) {
            if (iterator_position + index >= chunk->GetChunkSize()) {
                chunk = nullptr;
                iterator_position = 0;
                current_value = nullptr;
            } else {
                iterator_position = iterator_position + index;
                current_value = &chunk->at(iterator_position);
            }
            return *this;
        }

        ChunkList_iterator operator-(const difference_type &index) const {
            ChunkList_iterator temp(*this);

            if (iterator_position - index < 0) {
                temp.chunk = nullptr;
                temp.iterator_position = 0;
                temp.current_value = nullptr;
            } else {
                temp.iterator_position = iterator_position - index;
                temp.current_value = temp.chunk->at(iterator_position);
            }

            return temp;
        }

        ChunkList_iterator &operator-=(const difference_type &index) {
            if (iterator_position - index < 0) {
                chunk = nullptr;
                iterator_position = 0;
                current_value = nullptr;
            } else {
                iterator_position = iterator_position - index;
                current_value = &chunk->at(iterator_position);
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
        using value_type = ValueType;
        using difference_type = std::ptrdiff_t;
        using pointer = const ValueType *;
        using reference = const ValueType &;

        ChunkList_const_iterator() : ChunkList_iterator<ValueType>() {};

        ChunkList_const_iterator(const ChunkList_const_iterator &other) noexcept = default;

        ChunkList_const_iterator(Chunk<value_type> *current_chunk, std::size_t index, value_type *value) :
                ChunkList_iterator<value_type>(const_cast<Chunk<value_type> *>(current_chunk), index,
                                               const_cast<value_type *>(value)) {}

        ChunkList_const_iterator &operator=(const ChunkList_const_iterator &) = default;

        ~ChunkList_const_iterator() = default;

        friend void swap(ChunkList_const_iterator<ValueType> &left, ChunkList_const_iterator<ValueType> &right) {
            std::swap(left.chunk, right.chunk);
            std::swap(left.iterator_position, right.iterator_position);
            std::swap(left.current_value, right.current_value);
        }

        friend bool
        operator==(const ChunkList_const_iterator<ValueType> &left, const ChunkList_const_iterator<ValueType> &right) {
            return left.current_value == right.current_value;
        }

        friend bool
        operator!=(const ChunkList_const_iterator<ValueType> &left, const ChunkList_const_iterator<ValueType> &right) {
            return left.current_value != right.current_value;
        }

        reference operator*() const {
            return *this->current_value;
        }

        pointer operator->() const {
            return this->current_value;
        }

        ChunkList_const_iterator &operator++() {
            if (this->chunk == nullptr)
                throw std::exception();
            int chunk_size = this->chunk->GetChunkSize();
            if (this->iterator_position == chunk_size - 1) {
                this->chunk = nullptr;
                this->current_value = nullptr;
                this->iterator_position = 0;
                return *this;
            } else {
                ++this->iterator_position;
                this->current_value = &this->chunk->at(this->iterator_position);
                return *this;
            }
        }

        ChunkList_const_iterator operator++(int) {
            if (this->chunk == nullptr)
                throw std::exception();
            ChunkList_const_iterator temp(*this);
            if (this->iterator_position == this->chunk->GetChunkSize() - 1) {
                this->chunk = nullptr;
                this->current_value = nullptr;
                this->iterator_position = 0;
            } else {
                ++this->iterator_position;
                this->current_value = &this->chunk->at(this->iterator_position);
            }
            return temp;
        }

        ChunkList_const_iterator &operator--() {
            if (this->chunk == nullptr)
                throw std::exception();
            if (this->iterator_position == 0) {
                this->current_value = nullptr;
                this->chunk = nullptr;
                return *this;
            } else {
                --this->iterator_position;
                this->current_value = &this->chunk->at(this->iterator_position);
                return *this;
            }
        }

        ChunkList_const_iterator operator--(int) {
            if (this->chunk == nullptr)
                throw std::exception();
            ChunkList_const_iterator temp(*this);
            if (this->iterator_position == 0) {
                this->current_value = nullptr;
                this->chunk = nullptr;
            } else {
                --this->iterator_position;
                this->current_value = &this->chunk->at(this->iterator_position);
            }
            return temp;
        }

        ChunkList_const_iterator operator+(const difference_type &index) const {
            ChunkList_const_iterator temp(*this);

            if (this->iterator_position + index >= this->chunk->GetChunkSize()) {
                temp.chunk = nullptr;
                temp.iterator_position = 0;
                temp.current_value = nullptr;
            } else {
                temp.iterator_position = this->iterator_position + index;
                temp.current_value = temp.chunk->at(this->iterator_position);
            }

            return temp;
        }

        ChunkList_const_iterator &operator+=(const difference_type &index) {
            if (this->iterator_position + index >= this->chunk->GetChunkSize()) {
                this->chunk = nullptr;
                this->iterator_position = 0;
                this->current_value = nullptr;
            } else {
                this->iterator_position = this->iterator_position + index;
                this->current_value = &this->chunk->at(this->iterator_position);
            }
            return *this;
        }

        ChunkList_const_iterator operator-(const difference_type &) const {
            ChunkList_const_iterator temp(*this);

            if (this->iterator_position - this->index < 0) {
                temp.chunk = nullptr;
                temp.iterator_position = 0;
                temp.current_value = nullptr;
            } else {
                temp.iterator_position = this->iterator_position - this->index;
                temp.current_value = temp.chunk->at(this->iterator_position);
            }

            return temp;
        }

        ChunkList_const_iterator &operator-=(const difference_type &) {
            if (this->iterator_position - this->index < 0) {
                this->chunk = nullptr;
                this->iterator_position = 0;
                this->current_value = nullptr;
            } else {
                this->iterator_position = this->iterator_position - this->index;
                this->current_value = &this->chunk->at(this->iterator_position);
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
    class ChunkList : Chunk<T> {
    protected:
        int chunk_list_size = 0;
        Chunk<T> *chunks = nullptr;
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

        ChunkList() : chunks(new Chunk<T>(N)) {}

        explicit ChunkList(const Allocator &alloc) : chunks(new Chunk<T>(N, alloc)) {}

        ChunkList(size_type count, const T &value, const Allocator &alloc = Allocator()) : chunks(new Chunk<T>(N)) {
            Chunk<value_type> *current_chunk = chunks;
            for (chunk_list_size; chunk_list_size < count;) {
                current_chunk->allocator = alloc;
                for (int i = 0; i < N; i++) {
                    if (chunk_list_size < count) {
                        current_chunk->chunk[i] = value;
                        current_chunk->current_chunk_size++;
                        chunk_list_size++;
                    } else {
                        return;
                    }
                }
                Chunk<value_type> *previous_chunk = current_chunk;
                previous_chunk->next = new Chunk<value_type>(N, alloc);
                current_chunk = previous_chunk->next;
                current_chunk->prev = previous_chunk;
            }
        }

        explicit ChunkList(size_type count, const Allocator &alloc = Allocator()) : chunks(new Chunk<T>(N)) {
            Chunk<value_type> *current_chunk = chunks;
            for (chunk_list_size; chunk_list_size < count;) {
                current_chunk->allocator = alloc;
                for (int i = 0; i < N; i++) {
                    if (chunk_list_size < count) {
                        current_chunk->current_chunk_size++;
                        chunk_list_size++;
                    } else {
                        return;
                    }
                }
                Chunk<value_type> *previous_chunk = current_chunk;
                previous_chunk->next = new Chunk<value_type>(N, alloc);
                current_chunk = previous_chunk->next;
                current_chunk->prev = previous_chunk;
            }
        }

        ChunkList(const ChunkList &other) : chunks(new Chunk<T>(N)) {
            Chunk<value_type> *our = chunks;
            Chunk<value_type> *not_our = other.chunks;
            chunk_list_size = other.chunk_list_size;
            while (not_our != nullptr) {
                for (int i = 0; i < not_our->current_chunk_size; i++) {
                    our->chunk[i] = not_our->chunk[i];
                    our->current_chunk_size++;
                }
                Chunk<value_type> *next_chunk = new Chunk<T>(N);
                next_chunk->prev = our;
                our->next = next_chunk;
                our = next_chunk;
                not_our = not_our->next;
            }
        }

        ChunkList(const ChunkList &other, const Allocator &alloc) : chunks(new Chunk<value_type>(N,alloc)){
            Chunk<value_type> *our = chunks;
            Chunk<value_type> *not_our = other.chunks;
            chunk_list_size = other.chunk_list_size;
            while (not_our != nullptr) {
                for (int i = 0; i < not_our->current_chunk_size; i++) {
                    our->chunk[i] = not_our->chunk[i];
                    our->current_chunk_size++;
                }
                Chunk<value_type> *next_chunk = new Chunk<T>(N, alloc);
                next_chunk->prev = our;
                our->next = next_chunk;
                our = next_chunk;
                not_our = not_our->next;
            }
        }

        ChunkList(ChunkList &&other) {
            chunks = std::move(other.chunks);
            chunk_list_size = std::move(other.chunk_list_size);
            other.chunks = nullptr;
            other.chunk_list_size = 0;
        }

        ChunkList(ChunkList &&other, const Allocator &alloc) : chunks(other.chunks),
                                                               chunk_list_size(other.chunk_list_size) {
            Chunk<value_type> *current_chunk = this->chunks;
            while (current_chunk != nullptr) {
                current_chunk->allocator = alloc;
                current_chunk = current_chunk->next;
            }
            other.chunk_list_size = 0;
            other.chunks = nullptr;
        }

        ChunkList(std::initializer_list<T> init, const Allocator &alloc = Allocator()) {
            if (init.size() == 0) return;
            chunks = new Chunk(alloc);

            auto it = init.begin();

            for (; it != init.end(); ++it)
                push_back(*it);
        }

        ~ChunkList() = default;

        ChunkList &operator=(const ChunkList &other) {
            this = new ChunkList(other);
            return *this;
        }

        ChunkList &operator=(ChunkList &&other) {
            chunks = std::move(other.chunks);
            chunk_list_size = std::move(other.chunk_list_size);
            other.clear();
            return *this;
        }

        ChunkList &operator=(std::initializer_list<T> ilist) {
            auto it = ilist.begin();

            for (; it != ilist.end(); ++it) push_back(*it);
            return *this;
        };

        void assign(size_type count, const T &value) {
            if (count > 0) {
                clear();
                for (size_type i = 0; i < count; i++) push_back(value);
                chunk_list_size = count;
            }
        };

        void assign(std::initializer_list<T> ilist) {
            if (ilist.size() == 0) return;
            clear();

            auto it = ilist.begin();

            for (; it != ilist.end(); ++it) push_back(*it);
        };

        allocator_type get_allocator() const noexcept {
            return chunks->allocator;
        }

        reference at(size_type pos) {
            Chunk<value_type> *current_chunk = chunks;
            int count = 0;
            while (current_chunk != nullptr) {
                count++;
                current_chunk = current_chunk->next;
            }
            int chunk_pos = pos / N;
            int elem_pos = pos % N;
            if (pos >= N * count || pos < 0) throw std::out_of_range("Out of bounds");

            Chunk<value_type> *temp = chunks;
            for (int i = 0; i < chunk_pos; i++) {
                temp = temp->next;
            }
            return temp->chunk[elem_pos];
        }

        const_reference at(size_type pos) const {
            Chunk<value_type> *current_chunk = chunks;
            int count = 0;
            while (current_chunk != nullptr) {
                count++;
                current_chunk = current_chunk->next;
            }
            int chunk_pos = pos / N;
            int elem_pos = pos % N;
            if (pos >= N * count || pos < 0) throw std::out_of_range("Out of bounds");

            Chunk<value_type> *temp = chunks;
            for (int i = 0; i < chunk_pos; i++) {
                temp = temp->next;
            }
            return temp->chunk[elem_pos];
        }

        reference operator[](size_type pos) {
            return at(pos);
        }

        const_reference operator[](size_type pos) const {
            return at(pos);
        }

        reference front() {
            return chunk_list_size ? chunks->chunk[0] : throw std::runtime_error("Empty");
        }

        const_reference front() const {
            return chunk_list_size ? static_cast<reference>(chunks->chunk[0]) : throw std::runtime_error("Empty");
        }

        reference back() {
            if (chunk_list_size == 0) throw std::runtime_error("Empty");
            Chunk<value_type> *current_chunk = chunks;
            while (current_chunk->next != nullptr) current_chunk = current_chunk->next;
            return current_chunk->chunk[current_chunk->current_chunk_size - 1];
        }

        const_reference back() const {
            if (chunk_list_size == 0) throw std::runtime_error("Empty");
            Chunk<value_type> *current_chunk = chunks;
            while (current_chunk->next != nullptr) current_chunk = current_chunk->next;
            return const_cast<reference>(current_chunk->chunk[current_chunk->current_chunk_size - 1]);
        }

        iterator begin() noexcept {
            if (!chunk_list_size) {
                return end();
            }
            ChunkList_const_iterator<value_type> iter1(this->chunks, 0, &at(0));
            return iter1;
        }

        const_iterator begin() const noexcept {
            if (!chunk_list_size) {
                return end();
            }

            ChunkList_const_iterator<value_type> iter1(this->chunks, 0, &at(0));
            return iter1;
        }

        const_iterator cbegin() const noexcept {
            return begin();
        }

        iterator end() noexcept {
            return ChunkList_iterator<value_type>();
        }

        const_iterator end() const noexcept {
            return ChunkList_const_iterator<value_type>();
        }

        const_iterator cend() const noexcept {
            return end();
        }

        bool empty() const noexcept {
            return !chunk_list_size;
        }

        size_type size() const noexcept {
            return chunk_list_size;
        }

        size_type max_size() const noexcept {
            return (chunk_list_size % N) == 0 ? chunk_list_size : (chunk_list_size + N - chunk_list_size % N);
        }

        void shrink_to_fit() {
            Chunk<value_type> *current_chunk = chunks;

            while (current_chunk->next != nullptr) {
                current_chunk = current_chunk->next;
            }

            while (current_chunk->current_chunk_size == 0) {
                current_chunk = current_chunk->prev;
                current_chunk->next = nullptr;
            }
        }

        void clear() noexcept {
            Chunk<value_type> *current_chunk = chunks;
            while (current_chunk != nullptr) {
                Chunk<value_type>* temp_pointer = current_chunk;
                current_chunk = current_chunk->next;
                delete temp_pointer;
            }
            chunk_list_size = 0;
            chunks = nullptr;
        }

        iterator insert(const_iterator pos, const T &value) {
            push_back(value_type());
            ChunkList_const_iterator<value_type> iter(this, chunk_list_size - 1, &at(chunk_list_size - 1));

            bool flag = true; // Dislocated by 1 to right
            while (flag) {
                *iter = *(iter - 1);
                iter--;
                if (iter == pos)
                    flag = false;
            }

            *iter = value;
            return iter;
        }

        iterator insert(const_iterator pos, T &&value) {
            push_back(value_type());
            ChunkList_const_iterator<value_type> iter(this, chunk_list_size - 1, &at(chunk_list_size - 1));

            bool flag = true; // Dislocated by 1 to right
            while (flag) {
                *iter = *(iter - 1);
                iter--;
                if (iter == pos)
                    flag = false;
            }

            *iter = std::move(value);
            return iter;
        }

        iterator insert(const_iterator pos, size_type count, const T &value) {
            if (count == 0) return pos;

            ChunkList_const_iterator<value_type> iter(this, pos.iterator_position, at(pos.iterator_position));

            for (int i = 0; i < count; i++)
                insert(pos, value);

            return iter;
        }

        iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
            if (empty(ilist))
                return pos;

            ChunkList_const_iterator<value_type> iter(this, pos.iterator_position, at(pos.iterator_position));

            T current_element = ilist.end() - 1;

            bool flag = true;
            while (flag) {
                insert(pos, *current_element);
                if (current_element == ilist.begin()) {
                    flag = false;
                    break;
                }
                current_element--;
            }
            return iter;
        }

        template<class... Args>
        iterator emplace(const_iterator pos, Args &&... args) {
            auto data = { std::forward<Args>(args)... };
            ChunkList_const_iterator<value_type> iter(this, pos.iterator_position, &at(pos.iterator_position));

            for (auto i : data)
                insert(pos, i);

            return iter;
        }

        iterator erase(const_iterator pos) {
            ChunkList_const_iterator<value_type> iter(this, pos.iterator_position, &at(pos.iterator_position));

            while (true) {
                *iter = *(iter +1);
                iter++;
                if (iter.iterator_position == chunk_list_size - 1) break;
            }

            chunk_list_size--;

            ChunkList_const_iterator<value_type> iter1(this, pos.iterator_position, &at(pos.iterator_position));
            return iter1;
        }

        iterator erase(const_iterator first, const_iterator last) {
            auto t = last.iterator_position - first.iterator_position;

            ChunkList_const_iterator<value_type> iter(this, first.iterator_position, &at(first.iterator_position));

            for (int i = 0; i < t; i++)
                erase(first+1);

            return iter;
        }

        void push_back(const T &value) {
            if (chunks == nullptr)
                chunks = new Chunk<value_type>(N);

            Chunk<value_type> * current_chunk = chunks;
            while(current_chunk->next != nullptr){
                current_chunk = current_chunk->next;
            }

            if (current_chunk->current_chunk_size == N){
                current_chunk->next = new Chunk<value_type>(N);
                auto temp = current_chunk;
                current_chunk = current_chunk->next;
                current_chunk->prev = temp;
            }
            current_chunk->chunk[current_chunk->current_chunk_size] = value;
            current_chunk->current_chunk_size++;
            chunk_list_size++;
        }

        void push_back(T &&value) {
            if (chunks == nullptr)
                chunks = new Chunk<value_type>(N);

            Chunk<value_type> * current_chunk = chunks;
            while(current_chunk->next != nullptr){
                current_chunk = current_chunk->next;
            }

            if (current_chunk->current_chunk_size == N){
                current_chunk->next = new Chunk<value_type>(N);
                auto temp = current_chunk;
                current_chunk = current_chunk->next;
                current_chunk->prev = temp;
            }
            current_chunk->chunk[current_chunk->current_chunk_size] = std::move(value);
            current_chunk->current_chunk_size++;
            chunk_list_size++;
        }

        template<class... Args>
        reference emplace_back(Args &&... args){
            auto data = { std::forward<Args>(args)... };
            for (auto iter = data.begin(); iter != data.end(); iter++)
                push_back(*iter);
            return back();
        }

        void pop_back() {
            if(chunks == nullptr){
                throw std::runtime_error("empty");
                return;
            }
            chunk_list_size--;
            Chunk<value_type> *current_chunk = chunks;

            while(current_chunk->next != nullptr) {
                current_chunk = current_chunk->next;
            }

            current_chunk->current_chunk_size--;
        }

        void push_front(const T &value) {
            insert(cbegin(), value);
        }

        void push_front(T &&value) {
            insert(cbegin(), std::move(value));
        }

        template<class... Args>
        reference emplace_front(Args &&... args) {
            auto data = {std::forward<Args>(args)...};
            auto current_element = data.end() - 1;

            while (true) {
                insert(cbegin(), *current_element);
                if (current_element == data.begin())
                    break;
                current_element--;
            }
            return back();
        }

        void pop_front() {
            erase(cbegin());
        }

        void resize(size_type count) {
            if (chunk_list_size == count)
                return;
            else if (chunk_list_size > count){
                while(chunk_list_size != count)
                    pop_back();
            }
            else {
                while(chunk_list_size != count)
                    push_back(value_type());
            }
        }

        void resize(size_type count, const value_type &value){
            if (chunk_list_size == count)
                return;
            else if (chunk_list_size > count){
                while(chunk_list_size != count)
                    pop_back();
            }
            else {
                while(chunk_list_size != count)
                    push_back(value);
            }
        }

        void swap(ChunkList &other) {
            Chunk<value_type>* temp;
            int temp_size;
            temp = other.chunks;
            temp_size = other.chunk_list_size;
            other.chunks = this->chunks;
            other.chunk_list_size = this->chunk_list_size;
            this->chunks = temp;
            this->chunk_list_size=temp_size;
        }

        template<class U, class Alloc>
        friend bool operator==(const ChunkList<U, N, Alloc> &lhs,
                               const ChunkList<U, N, Alloc> &rhs) {
            bool flag = true;
            if (lhs.chunk_list_size == rhs.chunk_list_size) {
                for (int i = 0; i < lhs.chunk_list_size; i++) {
                    if (lhs.at(i) == rhs.at(i)) {
                        flag = true;
                    } else {
                        flag = false;
                        break;
                    }
                }
            }
            return flag;
        }

        template<class U, class Alloc>
        friend bool operator!=(const ChunkList<U, N, Alloc> &lhs,
                               const ChunkList<U, N, Alloc> &rhs) {
            return !operator==(lhs, rhs);
        }

        template<class U, class Alloc>
        friend bool operator>(const ChunkList<U, N, Alloc> &lhs, const ChunkList<U, N, Alloc> &rhs) {
            if (lhs.chunk_list_size <= rhs.chunk_list_size)
                return false;
            if (lhs.chunk_list_size > rhs.chunk_list_size)
                return true;
            for (int i = 0; i < lhs.chunk_list_size; i++) {
                if (rhs.at(i) >= rhs.at(i))
                    return false;
            }
            return true;
        }

        template<class U, class Alloc>
        friend bool operator<(const ChunkList<U, N, Alloc> &lhs, const ChunkList<U, N, Alloc> &rhs) {
            return !(operator==(lhs, rhs) || operator>(lhs, rhs));
        }

        template<class U, class Alloc>
        friend bool operator>=(const ChunkList<U, N, Alloc> &lhs,
                               const ChunkList<U, N, Alloc> &rhs) {
            return !(operator<(lhs, rhs));
        }

        template<class U, class Alloc>
        friend bool operator<=(const ChunkList<U, N, Alloc> &lhs,
                               const ChunkList<U, N, Alloc> &rhs) {
            return !(operator>(lhs, rhs));
        }

    };

    template<class T, int N, class Alloc>
    void swap(ChunkList<T, N, Alloc> &lhs, ChunkList<T, N, Alloc> &rhs) {
        std::swap(lhs, rhs);
    }

    template<class T, int N, class Alloc, class U>
    typename ChunkList<T, N, Alloc>::size_type erase(ChunkList<T, N, Alloc> &c, const U &value);

    template<class T, int N, class Alloc, class Pred>
    typename ChunkList<T, N, Alloc>::size_type erase_if(ChunkList<T, N, Alloc> &c, Pred pred);
}