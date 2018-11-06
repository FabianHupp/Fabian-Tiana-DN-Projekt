#pragma once

#include "dbms/assert.hpp"
#include "dbms/util.hpp"
#include <cstdint>
#include <functional>
#include <utility>


namespace dbms {

template<
    typename Key,
    typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>>
struct HashTable
{
    using key_type = Key;
    using hasher = Hash;
    using key_equal = KeyEqual;
    using size_type = std::size_t;

    using reference = Key&;
    using const_reference = const Key&;
    using pointer = Key*;
    using const_pointer = const Key*;

    private:
    template<bool C>
    struct the_iterator
    {
        friend struct HashTable;

        static constexpr bool is_const = C;
        using pointer_type = std::conditional_t<is_const, const_pointer, pointer>;
        using reference_type = std::conditional_t<is_const, const_reference, reference>;

        the_iterator(std::size_t idx, HashTable &table): idx_(idx) , table_(table)  { }

        /** Compare this iterator with an other iterator for equality. */
        bool operator==(the_iterator other) const { return this->idx_ == other.idx_; }
        bool operator!=(the_iterator other) const { return this->idx_ != other.idx_;}

        /** Advance the iterator to the next element. */
        the_iterator & operator++() {
            ++idx_; return *this;
        }

        /** Return a reference to the designated element */
        reference_type operator*() const {
            return table_.map[idx_];
        }
        /** Return a pointer to the designated element. */
        pointer_type operator->() const {

            return &table_.map[idx_];
        }

        private:
        std::size_t idx_;
        HashTable &table_;
    };
    public:
    using iterator = the_iterator<false>;
    using const_iterator = the_iterator<true>;

    iterator begin() { return iterator(0, *this); }
    iterator end()   { return iterator(map.size(), *this); }
    const_iterator begin() const { return const_iterator(0,*this); }
    const_iterator end()   const { return const_iterator(map.size(),*this); }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend()   const { return end(); }

    HashTable()
    {
        amount = 0;
    }

    ~HashTable() {
        amount = 0;
    }

    size_type size() const { return map.size(); }
    size_type capacity() const { return amount; }

    /** If an element with key exists, returns an iterator to that element.  Otherwise, returns end(). */
    iterator find(const key_type &key) {
        iterator a = begin();
        for (int i = 0; i < map.size(); ++i) {

            if (KeyEqual{}(map[i],key)){
                return a;
            }
            ++a;
        }

        return a;
    }
    const_iterator find(const key_type &key) const {
        const_iterator a = begin();
        for (int i = 0; i < map.size(); ++i) {
            if (KeyEqual{}(map[i],key)){
                return a;
            }
            ++a;
        }
        return a;

    }

    /** Returns an iterator to the element in the table and a flag whether insertion succeeded.  The flag is true, if
     * the element was newly inserted into the table, and false otherwise.  The iterator designates the newly inserted
     * element respectively the element already present in the table. */
    std::pair<iterator, bool> insert(const key_type &key) {

        if (map.size() > 1000){
            map.push_back(key);
            return std::make_pair(end(), true);
        }
        auto search = find(key);

        if (search == end()){
            map.push_back(key);
            amount++;
            //printf("New Insert \n\n\n\n");
            return std::make_pair(search, true);
        }
        else{
            //printf("Old Insert \n\n\n\n");

            return std::make_pair(search, false);
        }
        /*
        for(int i=0 ; i< map.size() ; ++i){
            if (map[i] == key){
                return std::make_pair(iterator(i,*this), false);
            }
        }*/


    }

    int amount;
    std::vector<Key> map;
};

template<
    typename Key,
    typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>>
using hash_set = HashTable<Key, Hash, KeyEqual>;

namespace {

template<typename Key, typename Value, typename Hash>
struct map_hash
{
    std::size_t operator()(const std::pair<Key, Value> &pair) const {
        return Hash{}(pair.first);
    }
};

template<typename Key, typename Value, typename KeyEqual>
struct map_equal
{
    bool operator()(const std::pair<Key, Value> &first, const std::pair<Key, Value> &second) const {
        return KeyEqual{}(first.first, second.first);
    }
};

}

template<
    typename Key,
    typename Value,
    typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>>
struct hash_map
{
    public:
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<const Key, Value>;
    using hasher = map_hash<const Key, Value, Hash>;
    using key_equal = map_equal<const Key, Value, KeyEqual>;
    using size_type = std::size_t;

    using reference = Key&;
    using const_reference = const Key&;
    using pointer = Key*;
    using const_pointer = const Key*;

    private:
    using table_type = HashTable<std::pair<key_type, mapped_type>, hasher, key_equal>;

    public:
    using iterator = typename table_type::iterator;
    using const_iterator = typename table_type::const_iterator;


    public:
    hash_map() { }
    /* OPTIONAL TODO 3.1 - define alternative constructors */

    iterator begin() { return table_.begin(); }
    iterator end() { return table_.end(); }
    const_iterator begin() const { return table_.begin(); }
    const_iterator end() const { return table_.end(); }
    const_iterator cbegin() const { return table_.cbegin(); }
    const_iterator cend() const { return table_.cend(); }

    size_type size() const { return table_.size(); }
    size_type capacity() const { return table_.capacity(); }

    iterator find(const key_type &key) { return table_.find({key, mapped_type()}); }
    const_iterator find(const key_type &key) const { return table_.find({key, mapped_type()}); }

    std::pair<iterator, bool> insert(const value_type &value) { return table_.insert(value); }

    mapped_type & operator[](const key_type& key) {
        return table_.insert(std::make_pair(key, mapped_type())).first->second;
    }

    private:
    table_type table_;
};

}
