#include "impl/RowStore.hpp"


using namespace dbms;


std::size_t get_alignment_requirement(const Attribute &attr) {
    switch (attr.type) {
        case Attribute::TY_Int:
        case Attribute::TY_Float:
        case Attribute::TY_Double:
            return attr.size;

        case Attribute::TY_Char:
            return 1;

        case Attribute::TY_Varchar:
            return sizeof(Varchar);

        default:
            dbms_unreachable("unknown attribute type");
    }
}

RowStore::~RowStore() {
    free(data_);
    free(offsets_);
}

RowStore RowStore::Create_Naive(const Relation &relation) {
    RowStore row_store;

    row_store.num_attributes_ = relation.size();
    row_store.offsets_ = static_cast<decltype(offsets_)>(malloc(relation.size() * sizeof(*offsets_)));

    /* Compute the required size of a row.  Remember that there can be padding. */
    std::size_t row_size = 0;
    std::size_t max_alignment = 0;

    for (auto attr : relation) {
        std::size_t elem_size;
        std::size_t elem_alignment;
        switch (attr.type) {
            case Attribute::TY_Int:
            case Attribute::TY_Float:
            case Attribute::TY_Double:
                elem_size = elem_alignment = attr.size;
                break;

            case Attribute::TY_Char:
                elem_size = attr.size;
                elem_alignment = 1;
                break;

            case Attribute::TY_Varchar:
                elem_size = elem_alignment = sizeof(void *);
                break;

            default:
                dbms_unreachable("unknown attribute type");
        }

        max_alignment = std::max(elem_alignment, max_alignment);
        if (row_size % elem_alignment) { // add padding bytes for alignment
            std::size_t padding = elem_alignment - row_size % elem_alignment;
            row_size += padding;
        }
        row_store.offsets_[attr.offset()] = row_size;
        row_size += elem_size;
    }
    if (row_size % max_alignment) row_size += max_alignment - row_size % max_alignment;
    row_store.row_size_ = row_size;

    return row_store;
}

RowStore RowStore::Create_Optimized(const Relation &relation) {

    std::size_t order[relation.size()];
    AttributeTupel ata[relation.size()];
    int i = 0;
    for (auto attr : relation) {
        ata[i] = AttributeTupel(attr, i);
        i++;
    }


    //ataInsertionSort(ata,relation.size());
    for (int l = 0; l < relation.size(); l++) {
        order[l] = ata[l].id_;
    }
    return Create_Explicit(relation, order);
}

RowStore RowStore::Create_Explicit(const Relation &relation, std::size_t *order) {


    RowStore rowStore;

    rowStore.num_attributes_ = relation.size();
    rowStore.offsets_ = static_cast<decltype(offsets_)>(malloc(relation.size() * sizeof(*offsets_)));

    std::size_t row_size = 0;
    std::size_t max_alignment = 0;


    for (int i = 0; i < relation.size(); i++) {
        std::size_t elem_size;
        std::size_t elem_alignment;

        auto attr = relation[order[i]];

        switch (attr.type) {
            case Attribute::TY_Int:
            case Attribute::TY_Float:
            case Attribute::TY_Double:
                elem_size = elem_alignment = attr.size;
                break;

            case Attribute::TY_Char:
                elem_size = attr.size;
                elem_alignment = 1;
                break;

            case Attribute::TY_Varchar:
                elem_size = elem_alignment = sizeof(void *);
                break;

            default:
                dbms_unreachable("unknown attribute type");
        }

        max_alignment = std::max(elem_alignment, max_alignment);
        if (row_size % elem_alignment) { // add padding bytes for alignment
            std::size_t padding = elem_alignment - row_size % elem_alignment;
            row_size += padding;
        }
        rowStore.offsets_[attr.offset()] = row_size;
        row_size += elem_size;

    }
    if (row_size % max_alignment) row_size += max_alignment - row_size % max_alignment;
    rowStore.row_size_ = row_size;


    return rowStore;
}

/*
    Rowstore allocates enough memory for new_cap many rows
    Where row_size is the size of one Row(guess)
 */
void RowStore::reserve(std::size_t new_cap) {

    if (data_ == nullptr) {
        data_ = malloc(new_cap * row_size_);
        capacity_ = new_cap;
        return;
    }

    void *newPointer;
    if (new_cap >= capacity()) {
        newPointer = realloc(data_, new_cap * row_size_);

        if (newPointer != nullptr) {
            data_ = newPointer;
            capacity_ = new_cap;
        }
    }
}

/*
    Reallocates Memory and returs an iterrator for the first new item.
    offset is the value of the previous allocated memory.
 */
RowStore::iterator RowStore::append(std::size_t n_rows) {

    if (data_ == NULL) {
        reserve(n_rows);

        //append the data from new rows here ***
    } else {

        std::size_t freespace = capacity() - size();
        if (freespace < n_rows) reserve(capacity() + n_rows);

        //append the data from new rows here ***
    }
    //Add iterators
    iterator iterator = end();
    size_ = size() + n_rows;
    return iterator;

}

void dbms::ataInsertionSort(AttributeTupel *ata, std::size_t size) {
    for (int i = 2; i < size; i++) {
        AttributeTupel k = ata[i];
        int j = i;
        while (j > 1 && ata[j - i].getAttr().size > k.getAttr().size) {
            ata[j] = ata[j - 1];
            j--;
        }
        ata[j] = k;
    }

}


