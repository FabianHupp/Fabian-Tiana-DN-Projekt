#pragma once

#include "dbms/assert.hpp"
#include "dbms/macros.hpp"
#include "dbms/util.hpp"
#include <algorithm>
#include <utility>
#include <vector>


namespace dbms {

template<
    typename Key,
    typename Value,
    typename Compare = std::less<Key>>
struct BPlusTree
{
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<const Key, Value>;
    using size_type = std::size_t;
    using key_compare = Compare;

    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    /*--- Tree Node Data Types ---------------------------------------------------------------------------------------*/


    struct leaf_node
    {
        leaf_node(int leafsiz, int leafnumber){
            leaf_size = leafsiz;
            number = leafnumber;
        }
        int leaf_size;
        std::vector<value_type> values;
        int number;
    };


    struct inner_node
    {
        inner_node(int leafsize,bool follow){
            leaf_size = leafsize;
            innerfollow = follow;
        }
        std::vector<Key> Keys;
        //inner_node* innersuccessors[6];
        //leaf_node* leafsuc[6];
        std::vector<inner_node*> innersuccessors;
        std::vector<leaf_node*> leafsuc;
        int filled;
        int leaf_size;
        bool innerfollow;

    };

    /*--- Iterator ---------------------------------------------------------------------------------------------------*/
    private:
    template<bool C>
    struct the_iterator
    {
        friend struct BPlusTree;
        static constexpr bool is_const = C;
        using pointer_type = std::conditional_t<is_const, const_pointer, pointer>;
        using reference_type = std::conditional_t<is_const, const_reference, reference>;

        the_iterator(std::vector<leaf_node*> leafs) {
            leaf = leafs;
            actstand = 0;
            aleaf = 0;
            ainner = 0;
        }

        /** Compare this iterator with an other iterator for equality. */
        bool operator==(the_iterator other) const {
            if(actstand == other.actstand){return true;}
            return false;
        }
        bool operator!=(the_iterator other) const {
            if(actstand != other.actstand){
                return true;
            }
            return false;
        }

        /** Advance the iterator to the next element. */
        the_iterator & operator++() {
            actstand++;
            if(ainner == 4){
                aleaf++;
                ainner = 0;
            }else{
                ainner++;
            }

            return *this;
        }

        /** Return a pointer to the designated element. */
        pointer_type operator->() const {
            value_type* actvalue = &(leaf[aleaf]->values[ainner]);
            return actvalue;}
        /** Return a reference to the designated element */
        reference_type operator*() const {
            value_type& actvalue = leaf[aleaf]->values[ainner];
            return actvalue;
        }

    public:
        std::vector<leaf_node*> leaf;
        int actstand,aleaf,ainner;

    };
    public:
    using iterator = the_iterator<false>;
    using const_iterator = the_iterator<true>;

    /*--- Range Type -------------------------------------------------------------------------------------------------*/
    private:
    template<bool C>
    struct the_range
    {
        friend struct BPlusTree;

        the_range(the_iterator<C> begin, the_iterator<C> end) : begin_(begin), end_(end) {
            key_compare lt;
            assert(not lt(end_->first, begin_->first)); // begin <= end
        }

        the_iterator<C> begin() const { return begin_; }
        the_iterator<C> end() const { return end_; }

        bool empty() const { return begin_ == end_; }

        friend std::ostream & operator<<(std::ostream &out, the_range range) {
            return out << "the_range<" << (C ? "true" : "false") << "> from "
                       << range.begin_->first << " to " << range.end_->first;
        }
        DECLARE_DUMP

        private:
        the_iterator<C> begin_;
        the_iterator<C> end_;
    };
    public:
    using range = the_range<false>;
    using const_range = the_range<true>;






    /*--- Factory methods --------------------------------------------------------------------------------------------*/
    template<typename It>
    static BPlusTree Bulkload(It begin, It end) {

        std::vector<leaf_node*> alleafs;         //index 0 = root at the moment
        std::vector<inner_node*> allinnernodes;
        int root = 0;

        int leafnumber = 0;
        leaf_node* actleaf = new leaf_node(5,leafnumber);
        int fill = 0;

        while(begin != end){
            if(fill == 5){
                alleafs.push_back(actleaf);
                leafnumber++;
                actleaf = new leaf_node(5,leafnumber);
                fill = 0;
            }
            actleaf->values.push_back(*begin);
            ++begin;
            fill++;

        }
        //hier hat man auf jedenfall immernoch das letzte leaf entweder voll oder noch nicht ganz voll
        // => commit
        alleafs.push_back(actleaf);
        //jetzt haben wir alle leaves im BPlusTree aber noch ohne parents und es gibt keine innernodes
        int numleafs = alleafs.size();
        int amountinners = numleafs/6;    //fanout = 6
        if(numleafs%6 != 0){amountinners++;}
        int saveinners = amountinners;      //neede for later tree creation
        int overhead = numleafs%6;

        inner_node* inner = new inner_node(5,false);
        int leafkey = 0; //actual leafstate
        while(amountinners != 0){
            int innerkey = 0;
            if(overhead != 0 && amountinners == 1){
                    inner->filled = overhead;
                    for(int a = 0; a < overhead;a++){
                        if(a == 0){//nur succesor
                            if(overhead == 1){
                                key_type savekey = (std::get<0>(alleafs[leafkey]->values[0]));
                                inner->Keys.push_back(++savekey);
                            }
                            inner->leafsuc.push_back(alleafs[leafkey]);
                            leafkey++;
                        }else{
                            //inner->leafsuc[a] = alleafs[leafkey];
                            inner->leafsuc.push_back(alleafs[leafkey]);
                            inner->Keys.push_back(std::get<0>(alleafs[leafkey]->values[0]));
                            leafkey++;
                            innerkey++;
                        }
                    }
            }else{
                inner->filled = 6;
                for(int i = 0; i < 6; i++) {
                    if (i == 0) {//nur succesor
                        inner->leafsuc.push_back(alleafs[leafkey]);
                        leafkey++;
                    } else {
                        inner->leafsuc.push_back(alleafs[leafkey]);
                        inner->Keys.push_back(std::get<0>(alleafs[leafkey]->values[0]));
                        leafkey++;
                        innerkey++;
                    }
                }
            }//hier hat man alle keys und pointer für eine innernode gesetzt=> commit
            allinnernodes.push_back(inner);
            inner = new inner_node(5,false);
            amountinners--;
        }
        //hier hat man jetzt leafnodes + erste reihe an
        inner_node* newin = new inner_node(5,true);
        int innerstate = 0;
        while(saveinners != 1){
            int amountnewrow = saveinners/6;
            if(saveinners%6 != 0){amountnewrow++;}
            if(amountnewrow == 1){      //we found the root
                if(saveinners/6 == 1){
                    inner->filled = 6;
                    for (int l = 0; l < 6; l++) {
                        newin->innersuccessors.push_back(allinnernodes[innerstate]);
                        innerstate++;
                    }
                    for (int u = 0; u < 5; u++) {
                        newin->Keys.push_back((newin->innersuccessors[u+1])->Keys[0]);
                    }
                }else{
                    int overhe = saveinners%6;
                    inner->filled = overhe;
                    for(int l = 0; l < overhe; l++) {            //set successors and parents
                        newin->innersuccessors.push_back(allinnernodes[innerstate]);
                        innerstate++;
                    }
                    //set the keys
                    if(overhe == 1){
                        key_type sakey = allinnernodes[innerstate-1]->Keys[0];
                        newin->Keys.push_back(++sakey);
                    }
                    for(int u = 0; u < (overhe-1); u++){
//<<<<<<< HEAD
                        newin->Keys.push_back((newin->innersuccessors[u+1])->Keys[0]);
//=======
                        //inner_node tim = *newin;
                       // newin->Keys.push_back((tim.innersuccessors[u+1])->Keys[0]);
//>>>>>>> c56014b4a9c1733d33985e06d2d60453630eef2c
                    }

                }
                allinnernodes.push_back(newin);
               // newin = new inner_node(5,true);
                root = innerstate; //evtl hier ändern
            }else {
                for (int k = 0; k < amountnewrow; k++) {         //create the new row
                    int overh = saveinners % 6;
                    if (k == (amountnewrow - 1) && overh != 0) {
                        inner->filled = overh;
                        for (int l = 0; l < overh; l++) {
                            newin->innersuccessors.push_back(allinnernodes[innerstate]);
                            innerstate++;
                        }//set the keys
//<<<<<<< HEAD
                        if(overh == 1){
                            key_type skey = newin->innersuccessors[0]->Keys[0];
                            newin->Keys.push_back(++skey);
                        }else {
                            for (int u = 0; u < (overh - 1); u++) {
                                newin->Keys.push_back((newin->innersuccessors[u + 1])->Keys[0]);
                            }
/*
=======

                        //for (int u = 0; u < (overh - 1); u++) {
                           // printf("letzter u: %i\n\n",u);
                            //inner_node tim = *newin;
                            //newin->Keys.push_back((tim.innersuccessors[u+1])->Keys[0]);
//>>>>>>> c56014b4a9c1733d33985e06d2d60453630eef2c*/
                       }
                    } else {
                        inner->filled = 6;
                        for (int l = 0; l < 6; l++) {            //set successors and parents
                            newin->innersuccessors.push_back(allinnernodes[innerstate]);
                            innerstate++;
                        }                                       //set the keys
                        for (int u = 0; u < 5; u++) {
//<<<<<<< HEAD
                            newin->Keys.push_back((newin->innersuccessors[u+1])->Keys[0]);
//=======
                           // printf("erste u: %i\n\n",u);
                            //inner_node tim = *newin;
                            //newin->Keys.push_back((tim.innersuccessors[u+1])->Keys[0]);
//>>>>>>> c56014b4a9c1733d33985e06d2d60453630eef2c
                        }
                    }
                    allinnernodes.push_back(newin);
                    newin = new inner_node(5,true);

                }
            }
            /*allinnernodes.push_back(newin);
            newin = new inner_node(5,true);*/
            saveinners = amountnewrow;
        }

        // hier sind jetzt alle parents gesetzt, und root gesetzt: return tree*/
        return BPlusTree(5,alleafs,allinnernodes,root);
    }


    /*--- Start of B+-Tree code --------------------------------------------------------------------------------------*/
    private:
    BPlusTree(int leavesize, std::vector<leaf_node*> leafs,std::vector<inner_node*> inner,int root){
        treeleafsize = leavesize;
        treealleafs = leafs;
        treeallinnernodes = inner;
        treeroot = root;
    }

    public:
    BPlusTree(const BPlusTree&) = delete;
    BPlusTree(BPlusTree&&) = default;

    ~BPlusTree() {}

    iterator begin() { iterator a = the_iterator<false>(treealleafs); return a;}
    iterator end()   {
        iterator a = the_iterator<false>(treealleafs);
        int leaf = treealleafs.size();
        int elements = (leaf-1)*5;
        int extra = treealleafs[leaf-1]->values.size();
        elements = elements+extra;
        a.actstand = elements;
        a.aleaf = (leaf-1);
        a.ainner = (extra-1);
        return a;
    }
    const_iterator begin() const { const_iterator a = the_iterator<true>(treealleafs); return a;}
    const_iterator end()   const {
        const_iterator a = the_iterator<true>(treealleafs);
        int leaf = treealleafs.size();
        int elements = (leaf-1)*5;
        int extra = treealleafs[leaf-1]->values.size();
        elements = elements+extra;
        a.actstand = elements;
        a.aleaf = (leaf-1);
        a.ainner = (extra-1);

        return a;
    }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend()   const { return end(); }

    const_iterator find(const key_type key) const {

        int begin = treeroot;
        inner_node* node = treeallinnernodes[begin];
        leaf_node* lnode;

        bool found = false;
        while(!found){
            if(!(node->innerfollow)){  //wenn nurnoch leafs folgen, dann abbrechen
                found = true;
            }else{
                int savekeyslot = node->Keys.size();
                bool first = false;
                for(int i = 0; i < node->Keys.size(); i++){
                    if(key < node->Keys[i] && !first){
                        savekeyslot = i;
                        first = true;
                    }
                }
                node = node->innersuccessors[savekeyslot];
            }
        }
        //hier hat man jetzt die letzt innernode vor den leafnodes
        //also: key in der richtigen leafnode suchen
        int saveslot = node->Keys.size();
        bool fir = false;
        for(int i = 0; i < node->Keys.size(); i++){
            if(key < node->Keys[i] && !fir){
                saveslot = i;
                fir = true;
            }
        }
        lnode = node->leafsuc[saveslot];
        bool found2 = false;
        const_iterator a = the_iterator<true>(treealleafs);

        for(int k = 0; k < lnode->values.size() && !found2; k++){
            if(std::get<0>(lnode->values[k]) == key){
                found2 = true;
                a.actstand = ((lnode->number)*5) +(k);
                a.aleaf = lnode->number;
                a.ainner = k;
            }
        }
        if(found2){
            return a;
        }
        return end();


    }
    iterator find(const key_type &key) {

        int begin = treeroot;
        inner_node* node = treeallinnernodes[begin];
        leaf_node* lnode;

        bool found = false;
        while(!found){
            if(!(node->innerfollow)){  //wenn nurnoch leafs folgen, dann abbrechen
                found = true;
            }else{
                int savekeyslot = node->Keys.size();
                bool first = false;
                for(int i = 0; i < node->Keys.size(); i++){
                    if(key < node->Keys[i] && !first){
                        savekeyslot = i;
                        first = true;
                    }
                }
                node = node->innersuccessors[savekeyslot];
            }
        }
        //hier hat man jetzt die letzt innernode vor den leafnodes
        //also: key in der richtigen leafnode suchen
        int saveslot = node->Keys.size();
        bool fir = false;
        for(int i = 0; i < node->Keys.size(); i++){
            if(key < node->Keys[i] && !fir){
                saveslot = i;
                fir = true;
            }
        }
        lnode = node->leafsuc[saveslot];
        bool found2 = false;
        iterator a = the_iterator<false>(treealleafs);

        for(int k = 0; k < lnode->values.size() && !found2; k++){
            if(std::get<0>(lnode->values[k]) == key){
                found2 = true;
                a.actstand = ((lnode->number)*5) + (k);
                a.aleaf = lnode->number;
                a.ainner = k;
            }
        }
        if(found2){
            return a;
        }
        return end();
    }

    const_range in_range(const key_type &lower, const key_type &upper) const {
        auto savelower = lower;
        auto saveupper = upper;
        bool foundlower, foundupper = false;
        const_iterator a = the_iterator<false>(treealleafs);
        while(savelower < upper && !foundlower){
            a = find(savelower);
            if(a != end()){
                foundlower = true;
            }else{
                ++savelower;
            }
        }
        auto lowerfound = savelower;
        const_iterator b = find(savelower);
        while(b != end() && !foundupper){
            ++b;
            if(b->first >= saveupper){
                foundupper = true;
            }
        }
        const_range ret = the_range<false>(a,b);
        return ret;
    }
    range in_range(const key_type &lower, const key_type &upper) {
        auto savelower = lower;
        auto saveupper = upper;
        bool foundlower, foundupper = false;
        while(savelower < upper && !foundlower){
            iterator a = find(savelower);
            if(a != end()){
                foundlower = true;
            }else{
            ++savelower;
            }
        }
        iterator finala = find(savelower);
        auto lowerfound = savelower;
        iterator b = find(savelower);
        while(b != end() && !foundupper){
            ++b;
            if(b->first >= saveupper){
                foundupper = true;
            }
        }
        range ret = the_range<false>(finala,b);
        return ret;
    }

public:
    std::vector<leaf_node*> treealleafs;
    std::vector<inner_node*> treeallinnernodes;
    int treeleafsize;
    int treeroot = 0;

};

}
