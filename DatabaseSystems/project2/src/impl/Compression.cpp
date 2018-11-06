#include "impl/Compression.hpp"
#include "impl/ColumnStore.hpp"


using namespace dbms;


ColumnStore * dbms::compress_columnstore_lineitem(const Relation &relation, const ColumnStore &store)
{

    Column<RLE<uint32_t>> *orderkeyColumn = new Column<RLE<uint32_t>>();
    //Column<uint32_t> *orderkeyColumn = new Column<uint32_t>();
    Column<uint32_t> *partkeyColumn = new Column<uint32_t>();
    Column<uint32_t> *suppkeyColumn = new Column<uint32_t>();
    Column<int32_t > *linenumberColumn = new Column<int32_t>();
    Column<int64_t > *quantityColumn = new Column<int64_t>();
    Column<int64_t > *extendedPriceColumn = new Column<int64_t>();
    Column<int64_t > *discountColumn = new Column<int64_t>();
    Column<int64_t > *taxColumn = new Column<int64_t>();
    //Column<RLE<char>> *returnflagColumn = new Column<RLE<char>>();
    //Column<RLE<char>> *linestatusColumn = new Column<RLE<char>>();
    Column<char> *returnflagColumn = new Column<char>();
    Column<char> *linestatusColumn = new Column<char>();
    Column<uint32_t> *shipdateColumn = new Column<uint32_t>();
    Column<uint32_t> *commitdateColumn = new Column<uint32_t>();
    Column<uint32_t> *receipdateColumn = new Column<uint32_t>();
    Column<Dictionary<Char<26>>> *shipinstructColumn = new Column<Dictionary<Char<26>>>();
    //Column<RLE<Dictionary<Char<11>>>> *shipmodeColumn = new Column<RLE<Dictionary<Char<11>>>>();
    Column<Dictionary<Char<11>>> *shipmodeColumn = new Column<Dictionary<Char<11>>>();
    Column<Char<45>> *commentColumn = new Column<Char<45>>();


    auto returnflagIterator = store.get_column<char>(0).begin();
    auto extendedPriceIterator = store.get_column<int64_t >(1).begin();
    auto linestatusIterator = store.get_column<char>(2).begin();
    auto taxIterator = store.get_column<int64_t >(3).begin();
    auto orderkeyIterator = store.get_column<uint32_t>(4).begin();
    auto discountIterator = store.get_column<int64_t >(5).begin();
    auto shipinstructIterator = store.get_column<Char<26>>(6).begin();
    auto partkeyIterator = store.get_column<uint32_t>(7).begin();
    auto suppkeyIterator = store.get_column<uint32_t>(8).begin();
    auto commitdateIterator = store.get_column<uint32_t>(9).begin();
    auto receipdateIterator = store.get_column<uint32_t>(10).begin();
    auto shipdateIterator = store.get_column<uint32_t>(11).begin();
    auto linenumberIterator = store.get_column<int32_t >(12).begin();
    auto shipmodeIterator = store.get_column<Char<11>>(13).begin();
    auto commentIterator = store.get_column<Char<45>>(14).begin();
    auto quantityIterator = store.get_column<int64_t >(15).begin();


    for(std::size_t i = 0, size = store.size(); i < size; i++){
        orderkeyColumn->push_back(*orderkeyIterator++);
        partkeyColumn->push_back(*partkeyIterator++);
        suppkeyColumn->push_back(*suppkeyIterator++);
        linenumberColumn->push_back(*linenumberIterator++);
        quantityColumn->push_back(*quantityIterator++);
        extendedPriceColumn->push_back(*extendedPriceIterator++);
        discountColumn->push_back(*discountIterator++);
        taxColumn->push_back(*taxIterator++);
        returnflagColumn->push_back(*returnflagIterator++);
        linestatusColumn->push_back(*linestatusIterator++);
        shipdateColumn->push_back(*shipdateIterator++);
        commitdateColumn->push_back(*commitdateIterator++);
        receipdateColumn->push_back(*receipdateIterator++);
        shipinstructColumn->push_back(*shipinstructIterator++);
        shipmodeColumn->push_back(*shipmodeIterator++);
        commentColumn->push_back(*commentIterator++);

    }



    std::initializer_list<ColumnBase *> list = {static_cast<ColumnBase* >(returnflagColumn),
                                                static_cast<ColumnBase* >(extendedPriceColumn),
                                                static_cast<ColumnBase* >(linestatusColumn),
                                                static_cast<ColumnBase* >(taxColumn),
                                                static_cast<ColumnBase* >(orderkeyColumn),
                                                static_cast<ColumnBase* >(discountColumn),
                                                static_cast<ColumnBase* >(shipinstructColumn),
                                                static_cast<ColumnBase* >(partkeyColumn),
                                                static_cast<ColumnBase* >(suppkeyColumn),
                                                static_cast<ColumnBase* >(commitdateColumn),
                                                static_cast<ColumnBase* >(receipdateColumn),
                                                static_cast<ColumnBase* >(shipdateColumn),
                                                static_cast<ColumnBase* >(linenumberColumn),
                                                static_cast<ColumnBase* >(shipmodeColumn),
                                                static_cast<ColumnBase* >(commentColumn),
                                                static_cast<ColumnBase* >(quantityColumn)};


    ColumnStore *columnstore = new ColumnStore(ColumnStore::Create_Explicit(list));

    return columnstore;


}


