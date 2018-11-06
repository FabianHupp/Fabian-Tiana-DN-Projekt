#pragma once

#include "dbms/Store.hpp"
#include "dbms/util.hpp"


namespace dbms {

    namespace iterator {

        template<bool C, typename T>
        typename column_iterator<C, T>::reference_type column_iterator<C, T>::operator*() const {
            auto columnadress = static_cast<T *>(column_.data_);
            columnadress = (idx_) + columnadress;

            return *columnadress;
        }

    }

    template<typename T>
    void Column<T>::push_back(T value) {
        if (capacity_ == size_) {
            this->reserve(capacity_ * 2);
        }

        new(static_cast<T *>(data_) + (size_))T(value);

        size_++;
    }

    template<typename T>
    Column<T> &ColumnStore::get_column(std::size_t offset) {

        return static_cast<Column<T> &>(*(columns_[offset]));
    }

}
