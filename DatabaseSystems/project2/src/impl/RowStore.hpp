#pragma once

#include "dbms/Store.hpp"
#include "dbms/util.hpp"
#include <algorithm>
#include <vector>


namespace dbms {

    namespace iterator {

        template<bool C>
        template<typename T>
        std::conditional_t<rowstore_iterator<C>::is_const, const T &, T &>
        rowstore_iterator<C>::get(std::size_t offset) const {
            std::size_t concreteoffset = store_.offsets_[offset];
            auto rowadress = row_ + concreteoffset;
            void *voidaddress = static_cast<void *>(rowadress);
            return *static_cast<T *>(voidaddress);

        }
    }

    struct AttributeTupel {
        AttributeTupel() {}

        AttributeTupel(Attribute attr, int id) {
            attr_ = attr;
            id_ = id;
        }

        Attribute getAttr() { return attr_; }

    public:
        Attribute attr_;
        int id_;

    };

    void ataInsertionSort(AttributeTupel *ata, std::size_t size);
}
