#include "dbms/assert.hpp"
#include "dbms/Compression.hpp"
#include <cstdint>
#include <dbms/Compression.hpp>


namespace dbms {

/**
 * This class implements a generic dictionary.
 * It keeps an internal bi-directional mapping from input value to dictionary index.
 */
template<typename T, typename S>
struct Dictionary
{

    using value_type = T;
    using index_type = S;

    /** Returns the size of the dictionary, i.e. the number of distinct values. */
    std::size_t size() const { return kti.size();}

    /** Returns the index of the given value in the dictionary. */
    index_type operator()(T value) {

        //einfügen wenn nicht in der map
        auto iter = kti.find(value);
        int idx;
        if(iter == kti.end()) {
            idx = kti.size();
            kti.insert(std::pair<T, uint32_t>(value, idx));
            itk.insert(std::pair<uint32_t, T>(idx, value));
        }

        return kti.at(value);
    }

    /** Returns the value at the given index in the dictionary. */
    const T & operator[](index_type idx) const {
        return itk.at(idx);
    }

    friend std::ostream & operator<<(std::ostream &out, const Dictionary<T> &dict) {
        dbms_unreachable("Not implemented.");
    }
    DECLARE_DUMP

    public:
    std::unordered_map<value_type, uint32_t> kti;
    std::unordered_map<uint32_t, value_type> itk;
};


    template<typename T>
void Column<RLE<T>>::push_back(T value)
{
    if (capacity_ == size_) {
        reserve(capacity_ * 2);
    }

    auto it = this->end();

    if(num_rows_ > 0){

        auto run = it.run_-1;
        if(run->value == value) {
            run->count++;
            num_rows_++;
            return;
        }
    }

    new (it.run_)RLE(value);
    size_++;
    num_rows_++;

}

template<typename T, typename S>
void Column<Dictionary<T, S>>::push_back(T value)
{
    int idx = dict_(value);
    Base::push_back(idx);
}

template<typename T, typename S>
void Column<RLE<Dictionary<T, S>>>::push_back(T value)
{
    int idx = dict_(value);
    Base::push_back(idx);
}

}
