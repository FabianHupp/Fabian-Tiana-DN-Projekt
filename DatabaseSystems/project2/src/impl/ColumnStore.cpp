#include "impl/ColumnStore.hpp"


using namespace dbms;


void GenericColumn::reserve(std::size_t new_cap) {

    void *newPointer;
    if (new_cap >= this->capacity_) {
        this->capacity_ += new_cap;
        newPointer = realloc(data_, capacity_ * elem_size_);
        data_ = newPointer;
    }
}

ColumnStore::~ColumnStore() {
    for (auto column: columns_) {
        delete (column);
    }
    columns_.clear();
}

ColumnStore ColumnStore::Create_Naive(const Relation &relation) {
    ColumnStore column_Store;
    std::size_t elem_size;
    bool varchar = false;


    for (auto attr : relation) {

        switch (attr.type) {
            case Attribute::TY_Int:
            case Attribute::TY_Float:
            case Attribute::TY_Double:
            case Attribute::TY_Char:
                elem_size = attr.size;

                break;

            case Attribute::TY_Varchar: {
                varchar = true;

                Column<Varchar> *col = new Column<Varchar>();
                ColumnBase *a = static_cast<ColumnBase *>(col);
                column_Store.columns_.push_back(a);

                break;
            }
            default:
                dbms_unreachable("unknown attribute type");
        }

        if(!varchar) {
            GenericColumn *column = new GenericColumn(elem_size);
            ColumnBase *a = static_cast<ColumnBase *>(column);
            column_Store.columns_.push_back(a);
        }

        varchar = false;

    }

    return column_Store;
}


ColumnStore ColumnStore::Create_Explicit(std::initializer_list<ColumnBase *> columns) {
    ColumnStore columnStore;

    for (auto col : columns){
        columnStore.columns_.push_back(col);
    }

    return columnStore;
}

std::size_t ColumnStore::size_in_bytes() const {
    std::size_t sizeBytes = 0;
    for (auto column: columns_) {
        sizeBytes += column->size_in_bytes();
    }

    return sizeBytes;
}

std::size_t ColumnStore::capacity_in_bytes() const {
    std::size_t capBytes = 0;
    for (auto column: columns_) {
        capBytes += column->capacity_in_bytes();
    }

    return capBytes;
}
