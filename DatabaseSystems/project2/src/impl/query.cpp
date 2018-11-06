#include "dbms/query.hpp"
#include "dbms/Store.hpp"
#include "impl/ColumnStore.hpp"
#include "impl/RowStore.hpp"
#include <unordered_map>
#include "impl/Compression.hpp"

namespace dbms {

    namespace query {

        namespace milestone1 {

            uint64_t Q1(const RowStore &store) {
                double sumup = 0;

                //iterator des rowstoreanfangs
                auto it = store.begin();
                for (std::size_t i = 0, size = store.size(); i < size; i++, ++it) {
                    uint32_t idate = it.get<uint32_t>(11);
                    if (idate < date_to_int(1998, 01, 01)) {
                        int64_t extendedprice = it.get<int64_t>(1);
                        int64_t discount = it.get<int64_t>(5);
                        int64_t tax = it.get<int64_t>(3);
                        sumup = sumup + (extendedprice * (100 - discount) * (100 + tax));
                    }
                }

                return sumup / (1000000);
            }

            uint64_t Q1(const ColumnStore &store) {
                double sumup = 0;

                //iterator für jedes argument das beötigt wir
                auto shipdateit = store.get_column<uint32_t>(11).begin();
                auto exprit = store.get_column<int64_t>(1).begin();
                auto discit = store.get_column<int64_t>(5).begin();
                auto taxit = store.get_column<int64_t>(3).begin();

                for (std::size_t i = 0, size = store.size(); i < size; i++) {
                    uint32_t idate = *shipdateit;
                    if (idate < date_to_int(1998, 01, 01)) {
                        int64_t extendedprice = *exprit;
                        int64_t discount = *discit;
                        int64_t tax = *taxit;
                        sumup = sumup + (extendedprice * (100 - discount) * (100 + tax));
                    }
                    shipdateit++;
                    exprit++;
                    discit++;
                    taxit++;
                }
                return sumup / (100 * 100 * 100);
            }


            unsigned Q2(const RowStore &store) {
                std::unordered_map<Char<11>, unsigned int> map;
                auto it = store.begin();

                unsigned int max = 0;

                for (std::size_t i = 0, size = store.size(); i < size; ++i) {
                    Char<11> text = it.get<Char<11>>(13);
                    auto findIter = map.find(text);

                    if (findIter == map.end()) {
                        map.insert(std::make_pair(text, 1));
                        if (i == 0) {
                            max = 1;
                        }
                    } else {
                        findIter->second++;
                        if (findIter->second > max) {
                            max = findIter->second;
                        }
                    }
                    ++it;
                }

                return max;
            }

            unsigned Q2(const ColumnStore &store) {
                std::unordered_map<Char<11>, unsigned int> map;
                auto shipMode = store.get_column<Char<11>>(13).begin();

                unsigned int max = 0;

                for (std::size_t i = 0, size = store.size(); i < size; ++i) {
                    Char<11> text = *shipMode;
                    auto findIter = map.find(text);

                    if (findIter == map.end()) {
                        map.insert(std::make_pair(text, 1));
                        if (i == 0) {
                            max = 1;
                        }
                    } else {
                        findIter->second++;
                        if (findIter->second > max) {
                            max = findIter->second;
                        }
                    }
                    ++shipMode;
                }

                return max;
            }


        }


        namespace milestone2 {

            unsigned Q2(const ColumnStore &store) {
                std::unordered_map<int, unsigned int> map;
                auto shipMode = store.get_column<int>(13).begin();

                unsigned int max = 0;

                for (std::size_t i = 0, size = store.size(); i < size; ++i) {
                    int text = *shipMode;
                    auto findIter = map.find(text);

                    if (findIter == map.end()) {
                        map.insert(std::make_pair(text, 1));
                        if (i == 0) {
                            max = 1;
                        }
                    } else {
                        findIter->second++;
                        if (findIter->second > max) {
                            max = findIter->second;
                        }
                    }
                    ++shipMode;
                }

                return max;
            }

            unsigned Q3(const ColumnStore &store) {
                unsigned int sumup = 0;
                //iterator für jedes argument das beötigt wir
                auto shipdateit = store.get_column<uint32_t>(11).begin();
                auto quantity = store.get_column<int64_t >(15).begin();

                auto date1 = date_to_int(1993, 01, 01);
                auto date2 = date_to_int(1993, 12, 31);

                for (std::size_t i = 0; i < store.size(); i++) {
                    uint32_t idate = *shipdateit;
                    if (date1<= idate && idate <= date2) {
                        int64_t quant = *quantity;
                        sumup = sumup + quant;
                    }
                    shipdateit++;
                    quantity++;

                }
                return sumup;
            }

            unsigned Q4(const ColumnStore &store, uint32_t O, uint32_t L) {
                auto orderkey = store.get_column<RLE<uint32_t>>(4).begin();
                auto lnum = store.get_column<int32_t>(12).begin();
                auto comment = store.get_column<Char<45>>(14).begin();

                unsigned int length = 0;

                for (std::size_t i = 0, size = store.size(); i < size; i++) {
                    uint32_t okey = *orderkey;
                    int32_t  lin = *lnum;
                    if (O == okey && lin == L) {
                        return std::strlen(*comment);
                    }
                    orderkey++;
                    lnum++;
                    comment++;
                }

                return length;

            }

        }

        namespace milestone3 {

            unsigned Q3(const ColumnStore &store, shipdate_index_type &index)
            {
                unsigned int sumup = 0;
                //iterator für jedes argument das beötigt wir
                auto shipdateit = store.get_column<uint32_t>(11).begin();
                auto quantity = store.get_column<int64_t >(15).begin();

                auto date1 = date_to_int(1993, 01, 01);
                auto date2 = date_to_int(1993, 12, 31);

                for (std::size_t i = 0; i < store.size(); i++) {
                    uint32_t idate = *shipdateit;
                    if (date1<= idate && idate <= date2) {
                        int64_t quant = *quantity;
                        sumup = sumup + quant;
                    }
                    shipdateit++;
                    quantity++;

                }
                return sumup;
            }

            unsigned Q4(const ColumnStore &store, uint32_t O, uint32_t L, primary_index_type &index)
            {
                auto orderkey = store.get_column<RLE<uint32_t>>(4).begin();
                auto lnum = store.get_column<int32_t>(12).begin();
                auto comment = store.get_column<Char<45>>(14).begin();

                unsigned int length = 0;

                for (std::size_t i = 0, size = store.size(); i < size; i++) {
                    uint32_t okey = *orderkey;
                    int32_t  lin = *lnum;
                    if (O == okey && lin == L) {
                        return std::strlen(*comment);
                    }
                    orderkey++;
                    lnum++;
                    comment++;
                }

                return length;
            }

            unsigned Q5(const ColumnStore &lineitem, const ColumnStore &orders)
            {
                //first step: brauchen: Lorderkey, extendedprice, tax, shipmode
                //second step: brauchen: Odersorderkey, orderstatus
                //1:
                auto Lorderkey = lineitem.get_column<RLE<uint32_t>>(4).begin();
                auto exprit = lineitem.get_column<int64_t>(1).begin();
                auto taxit = lineitem.get_column<int64_t>(3).begin();
                auto shipMode = lineitem.get_column<Dictionary<Char<11>>>(13).begin();
                auto dict = lineitem.get_column<Dictionary<Char<11>>>(13).get_dictionary();
                //2:
                auto Oorderkey = orders.get_column<uint32_t>(1).begin();
                auto orderstatus = orders.get_column<char>(8).begin();


                std::unordered_map<uint32_t, double> map;
                //std::vector<uint32_t> ord;

                for (std::size_t i = 0, size = lineitem.size(); i < size; ++i) {
                    //Where shipmode = "AIR"
                    if(*shipMode == dict("AIR")){
                        double sumup = 0;
                        int64_t extendedprice = *exprit;
                        int64_t tax = *taxit;
                        sumup = (extendedprice * tax);
                        sumup = sumup / (100*100); //gehen wir davon aus dass wir hier das richtige ergebnis haben

                        uint32_t order = *Lorderkey;
                        auto findIter = map.find(order);

                        if(findIter == map.end()){
                            map.insert(std::make_pair(order,sumup));
                            //ord.push_back(order);
                        }else{
                            findIter->second = findIter->second + sumup;
                        }
                    }
                    Lorderkey++;
                    exprit++;
                    taxit++;
                    shipMode++;
                } //hier hat man jetzt L, also orderkey gegrouped im vector, und für jede gruppe an Orderkey die summe gespeichert

                std::unordered_map<uint32_t, int> join;
                std::size_t ordsize = map.size();

                for(std::size_t a = 0, size = orders.size(); a < size; ++a){
                        char orderstat = *orderstatus;
                        uint32_t Oorder = *Oorderkey;
                        if(orderstat == 'F'){
                            for(auto it = map.begin(); it != map.end(); ++it){
                                if(Oorder == it->first){
                                    auto findIter = join.find(Oorder);
                                    if(findIter == join.end()){
                                        join.insert(std::make_pair(Oorder,1));
                                    }else{
                                        findIter->second++;
                                    }
                                }
                            }
                        }

                    //sollten in der neuen map schon so viele keys wie in der ersten map on lineitems sind, wissen wir,
                    //dass alle treffer aus L schon gesehen wurden und danach keine neuen mehr kommen können => abbruch
                        if(join.size() == ordsize){
                            a = size;
                        }
                    orderstatus++;
                    Oorderkey++;
                }

                uint32_t maxorderkey = 2;
                double maxtax =0;
                //jetzt noch den orderkey mit der größten summe finden
                for(auto it = join.begin(); it != join.end(); ++it){
                    uint32_t orderinjoin = it->first;
                    auto findIter = map.find(orderinjoin);

                    if(findIter == map.end()){
                        //nothin => dann fehler aber ignoriert
                    }else{
                        if(findIter->second > maxtax){
                            maxtax = findIter->second;
                            maxorderkey = orderinjoin;
                        }
                    }
                }

                //finished
                return maxorderkey;
            }

        }

    }





}
