/*stl源码剖析
 * 学习笔记*/
#include<iterator>
using std::forward_iterator_tag;
template <class Value>
struct __hashtable_node{
    __hashtable_node* next;
    Value val;
};
template<class Value,class Key,class HashFcn,
         class ExtractKey,class EqualKey,class Alloc>
struct __hashtable_iterator{
    typedef hashtable<Value,Key,HashFcn,ExtractKey,EqualKey,Alloc>
        hashtable;
    typedef __hashtable_iterator<Value,Key,HashFcn,ExtractKey,EqualKey,Alloc>
        iterator;
    typedef __hashtable_node<Value> node;
    typedef forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t differrence_type;
    typedef size_t size_type;
    typedef Value& referance;
    typedef Value* pointer;
    node* cur;
    hashtable* ht;

    __hashtable_iterator(node* n,hashtable* tab):cur(n),ht(tab){}
    __hashtable_iterator(){}

    referance operator*()const {return cur->val;}
    pointer operator->()const {return &(operator *());}
    iterator& operator++();
    iterator& operator++(int);
    bool operator ==(const iterator& iter)const{return iter.cur==cur;}
    bool operator !=(const iterator& iter)const{return iter.cur!=cur;}
};


