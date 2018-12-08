#include <vector>
#include <assert.h>
#include<string>
using std::pair;
using std::string;
using std::vector;

template<typename K, typename V>
struct HashNode
{
    pair<K, V> _kv;
    HashNode<K,V>* _next;

    HashNode(pair<K,V> p)
        :_next(NULL)
        , _kv(p)
    {}
};

template<class K, class V, class HashFunc> //声明
class HashTable;

//stl库里边是没有使用Ref和Ptr来达到代码复用的目的。而是将const和非const迭代器分开来写。
//具体原因在于新加的那个哈希表指针，我自己又将其该为const的，暂时没有发现问题
template<typename K, typename V, typename HashFunc, typename Ref, typename Ptr>
struct HashTableIterator
{
    typedef HashTableIterator<K, V, HashFunc, Ref, Ptr> Self;
    typedef HashTable<K, V, HashFunc> HashTable;
    typedef HashNode<K, V> Node;


    HashTableIterator(){}
    HashTableIterator(Node* ptr, const HashTable* table)
        : _ptr(ptr)
        , _hashtable(table)
    {}

    Ref operator*()
    {
        return _ptr->_kv;
    }

    Ptr operator->()
    {
        return &(operator*());
    }

    Self& operator++()
    {
        _ptr = _Next(_ptr);
        return *this;
    }

    Self operator++(int)
    {
        Self tmp = *this;
        ++*this;

        return tmp;
    }

    bool operator==(const Self& s)
    {
        return _ptr == s._ptr;
    }

    bool operator!=(const Self& s)
    {
        return _ptr != s._ptr;
    }

    Node* _ptr;
    const HashTable* _hashtable;

protected:
/*	Node* _Next(Node* cur)
    {
        assert(cur);

        Node* old = cur;
        cur = cur->_next;

        if (!cur)
        {
            size_t index = _hashtable->GetIndex(old->_kv.first);
            while (!cur && ++index < _hashtable->_table.size())   //需要友元
                cur = _hashtable->_table[index];
        }
        return cur;
    }*/

    Node* _Next(Node* cur)
    {
        assert(cur);

        Node* old = cur;
        cur = cur->_next;

        if (!cur)
        {
            size_t index = _hashtable->GetIndex(old->_kv.first);
            while (!cur && ++index < _hashtable->GetTable()->size())
                cur = _hashtable->GetTable()->operator[](index);
        }
        return cur;
    }
};


template<typename K>
struct _HashFunc
{
    size_t operator()(const K& key)
    {
        return key;
    }
};

template<>
struct _HashFunc<string>
{
    size_t operator()(const string& key)
    {
        return BKDRHash(key.c_str());
    }

    size_t BKDRHash(const char* str)  //字符串哈希算法
    {
        register size_t hash = 0;
        while (*str)
        {
            hash = hash * 131 + *str;
            str++;
        }
        return hash;
    }
};

template<typename K, typename V, class HashFunc = _HashFunc<K>>
class HashTable
{
    typedef HashNode<K, V> Node;
public:
    typedef HashTableIterator<K, V, HashFunc, pair<K, V>&, pair<K, V>*> Iterator;
    typedef HashTableIterator<K, V, HashFunc, const pair<K, V>&, const pair<K, V>*> ConstIterator;
public:
    HashTable()
        :_size(0)
    {}

    HashTable(size_t size)
        :_size(0)
    {
        _table.resize(GetNextSize(size));
    }

    pair<Node*, bool> Insert(const pair<K,V>& p)   //防冗余
    {
        if (_size == _table.size())//控制负载因子为1,当负载因子超过1时,进行扩容
        {
            CheckCapacity();
        }

        size_t index = GetIndex(p.first);

        Node* cur = _table[index];
        while (cur)
        {
            if (cur->_kv.first == p.first)
            {
                return std::make_pair(cur, false);
            }

            cur = cur->_next;
        }

        Node* tmp = new Node(p);

        tmp->_next = _table[index];
        _table[index] = tmp;

        ++_size;
        return std::make_pair(_table[index], true);
    }

    Node* Find(const K& key)
    {
        size_t index = GetIndex(key);

        Node* cur = _table[index];
        while (cur)
        {
            if (cur->_kv.first == key)
            {
                return cur;
            }

            cur = cur->_next;
        }

        return NULL;
    }

    bool Erase(const K& key)
    {
        size_t index = GetIndex(key);
        Node* cur = _table[index];
        Node* prev = NULL;

        while (cur)
        {
            if (cur->_kv.first == key)
            {
                if (prev == NULL)
                {
                    _table[index] = cur->_next;
                }
                else
                {
                    prev->_next = cur->_next;
                }
                delete cur;
                --_size;
                return true;
            }

            prev = cur;
            cur = cur->_next;
        }
        return false;
    }

    V& operator[](const K& key)
    {
        pair<Node*, bool> ret;
        ret = Insert(make_pair(key,V()));

        return ret.first->_kv.second;
    }

    Iterator Begin()
    {
        for (size_t i = 0; i < _table.size(); ++i)
            if (_table[i])
                return Iterator(_table[i], this);
        return End();
    }

    Iterator End()
    {
        return Iterator(NULL,this);
    }

    ConstIterator Begin() const
    {
        for (size_t i = 0; i < _table.size(); ++i)
            if (_table[i])
                return ConstIterator(_table[i], this);
        return End();
    }

    ConstIterator End() const
    {
        return ConstIterator(NULL, this);
    }

    ~HashTable()
    {
        Clear();
    }

    void Clear()
    {
        for (size_t i = 0; i < _table.size(); ++i)
        {
            Node* cur = _table[i];
            Node* del = NULL;
            while (cur)
            {
                del = cur;
                cur = cur->_next;
                delete del;
            }
            _table[i] = NULL;
        }
    }

    const size_t GetIndex(const K& key) const
    {
        HashFunc hf;
        size_t hash = hf(key);
        return hash % _table.size();
    }

    const vector<Node*>* GetTable() const
    {
        return &_table;
    }
protected:

    void CheckCapacity()
    {
        HashTable<K, V,HashFunc> ht(_table.size());

        for (size_t i = 0; i < _table.size(); ++i)
        {
            Node* cur = _table[i];
            while (cur)
            {
                ht.Insert(cur->_kv);
                cur = cur->_next;
            }
        }

        _table.swap(ht._table);
    }

    unsigned long GetNextSize(unsigned long size) //使用素数作为哈希表的大小可以减少哈希冲突
    {
        const int _PrimeSize = 28;
        static const unsigned long _PrimeList[_PrimeSize] =
        {
            53ul, 97ul, 193ul, 389ul, 769ul,
            1543ul, 3079ul, 6151ul, 12289ul, 24593ul,
            49157ul, 98317ul, 196613ul, 393241ul, 786433ul,
            1572869ul, 3145739ul, 6291469ul, 12582917ul, 25165843ul,
            50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul,
            1610612741ul, 3221225473ul, 4294967291ul
        };

        for (int i = 0; i < _PrimeSize; ++i)
        {
            if (_PrimeList[i] > size)
            {
                return _PrimeList[i];
            }
        }
        return 0;
    }
protected:
    vector<Node*> _table;
    size_t _size;
};


/*stl源码剖析
 * 学习笔记*/
#if 0
#include<iterator>
#include <algorithm>
#include<vector>
#include<memory>
using std::forward_iterator_tag;
using std::vector;

template <class KeyType>
struct hash{};
//字符串要进行映射
inline size_t __stl_hash_string(const char *s)
{
    unsigned long h = 0;
    for (;*s;++s)
    {
        h = 5 * h + *s;
    }

    return size_t(h);
}
//类模板显示特化定义
template <>
struct hash<int>{
    size_t operator()(int x) const {return x;}
};

template <>
struct hash<char *>{
    size_t operator()(const char *s) const {return __stl_hash_string(s);}
};

template <>
struct hash<const char *>{
    size_t operator()(const char *s) const {return __stl_hash_string(s);}
};

template <>
struct hash<char>{
    size_t operator()(char s) const {return s;}
};

template <>
struct hash<unsigned char>{
    size_t operator()(unsigned char s) const {return s;}
};

template <>
struct hash<signed char>{
    size_t operator()(signed char s) const {return s;}
};


template <class T1,class T2>
void construct(T1 *p,const T2 &value)
{
    new (p) T1(value);
}
template <class T1,class T2>
void destory(T1 *p)
{
    if(nullptr!=p)
    {
        delete p;
        p=nullptr;
    }
}

// C++ Standard 规定，每一个 Adaptable Unary Function 都必须继承此类别
template <class Arg,class Result>
struct unary_function{
    typedef Arg argument_type;
    typedef Result result_type;
};

// C++ Standard 规定，每一个 Adaptable Binary Function 都必须继承此类别
template <class Arg1,class Arg2,class Result>
struct binary_function{
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Result result_type;
};

//从节点中取出键值的仿函数定义
//identity function；任何数值通过此函数式后，不会发生任何改变
template <class T>
struct identity:public unary_function<T,T>
{
    const T& operator()(const T& x) const{return x;}
};

//判断键值是否相等的仿函数定义
template <class T>
struct equal_to:public binary_function<T,T,bool>{
    bool operator()(const T& x,const T& y) const{return x == y;}
};

template<class T,class Alloc>
class simple_alloc
{
public:

         static T* allocate(size_t n)
         {
            return 0 == n? 0:(T*)Alloc::allocate(n*sizeof(T));
         }

         static T* allocate(void)
         {
            return (T*) Alloc::allocate(sizeof (T));
         }

         static void deallocate(T* p,size_t n)
         {
            if(0 != n) Alloc::deallocate(p,n*sizeof(T));
         }
         static void deallocate(T* p)
         {
            Alloc::deallocate(p,sizeof(T));
         }

};
template <class Value>
struct __hashtable_node{
    __hashtable_node* next;
    Value val;
};
template<class Value,class Key,class HashFcn,
         class ExtractKey,class EqualKey,class Alloc=std::allocator<__hashtable_node<Value> > >
class hashtable;

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
    iterator operator++(int);
    bool operator ==(const iterator& iter)const{return iter.cur==cur;}
    bool operator !=(const iterator& iter)const{return iter.cur!=cur;}
};
template<class Value,class Key,class HashFcn,
         class ExtractKey,class EqualKey,class Alloc>
__hashtable_iterator<Value,Key,HashFcn,ExtractKey,EqualKey,Alloc>&
__hashtable_iterator<Value,Key,HashFcn,ExtractKey,EqualKey,Alloc>::operator++()
{
    const node* p_old=cur;
    cur=cur->next;
    if(!cur){
        size_type bucket=ht->bkt_num(p_old->val);
        while(!cur&& ++bucket < ht->buckets.size()){//非空，且++bucket，必须小于它buckets的大小
            cur=ht->buckets[bucket];
        }
    }
    return *this;
}
template<class Value,class Key,class HashFcn,
         class ExtractKey,class EqualKey,class Alloc>
inline __hashtable_iterator<Value,Key,HashFcn,ExtractKey,EqualKey,Alloc>
__hashtable_iterator<Value,Key,HashFcn,ExtractKey,EqualKey,Alloc>::operator++(int)
{
    iterator tmp= *this;
    ++*this;//
    return tmp;
}

template<class Value,class Key,class HashFcn,
         class ExtractKey,class EqualKey,class Alloc>
class hashtable{
public:
    typedef Value value_type;
    typedef HashFcn hasher;
    typedef EqualKey key_equal;
    typedef size_t size_type;
private:
    hasher hash;
    key_equal equals;
    ExtractKey get_key;

    typedef __hashtable_node<Value> node;
    typedef simple_alloc<node,Alloc> node_allocator;

    vector<node*,Alloc> buckets;
    size_type num_elements;
public:
    hashtable(size_type n,
              const HashFcn& hf,
              const EqualKey& eql)
        :hash(hf),equals(eql),get_key(ExtractKey()),num_elements(0)
    {
        initialize_buckets(n);
    }
    void initialize_buckets(size_type n)
    {
        const unsigned long  n_buckets = next_size(n);
        buckets.reserve(n_buckets);
        buckets.insert(buckets.end(),n_buckets,node*(0));
        num_elements=0;
    }
    size_type next_size(size_type n)
    {
        return __stl_next_prime(n);
    }
    size_type bucket_count()const{return buckets.size();}
    typedef simple_alloc<node,Alloc> node_allocator;
    static const int __stl_num_primes=28;
    static const unsigned long __stl_prime_list[__stl_num_primes]={53,97,193,389,769,1543,3079,6151,12289,24593,49157,98317,196613,393241,786433,1572869,3145739,6291469,12582917,25165843,50331653,100663319,201326611,402653189,805306457,1610612741,3221225473,4294967291};
    inline unsigned long __stl_next_prime(unsigned long n)
    {
        const unsigned long* first = __stl_prime_list;
        const unsigned long* last = __stl_prime_list + __stl_num_primes;
        const unsigned long* pos = std::lower_bound(first,last,n);
        return pos == last?*(last-1) : *pos;
    }
    size_type max_bucket_size() const
    {
        return __stl_prime_list[__stl_num_primes-1];
    }
    node* new_node(const value_type& vt)
    {
        node* n = node_allocator::allocate();
        n->val = 0;
        construct(&n->val,vt);
    }
    void delete_node(node* n)
    {
        destory(&n->val);
        node_allocator::deallocate(n);
    }

};

#endif

#if 0
//参考 http://lib.csdn.net/article/datastructure/11710
//助我理解模板特化，和书本中不同的是没有迭代器的实现

//《STL源码剖析》5.7 hashtable编程实现
//Author：江南烟雨
//E-Mail：xiajunhust@gmail.com

#include <iostream>
#include <algorithm>
#include <vector>
using std::vector;//hash table中链表节点数据结构定义
using std::cout;
using std::endl;

template <class ValueType>
struct __hashtable_node{
    __hashtable_node *next;
    ValueType val;
};

static const int __stl_num_primes = 28;//表格大小(28个质数)
//28个质数
static const unsigned long __stl_prime_list[__stl_num_primes] = {
    53,         97,           193,         389,       769,
    1543,       3079,         6151,        12289,     24593,
    49157,      98317,        196613,      393241,    786433,
    1572869,    3145739,      6291469,     12582917,  25165843,
    50331653,   100663319,    201326611,   402653189, 805306457,
    1610612741, 3221225473ul, 4294967291ul
};

//得出28个质数中不小于n的那个质数
inline unsigned long __get_next_prime(unsigned long n)
{
    const unsigned long *first = __stl_prime_list;
    const unsigned long *last = __stl_prime_list + __stl_num_primes;
    const unsigned long *pos = std::lower_bound(first,last,n);

    return pos == last ? *(last - 1) : *pos;
}

template <class T1,class T2>
void construct(T1 *p,const T2 &value)
{
    new (p) T1(value);
}

template <class T>
void destroy(T* pointer)
{
    pointer->~T();//调用析构函数
}

//hash函数定义
//都是仿函数
template <class KeyType>
struct hash{};

//字符串要进行映射
inline size_t __stl_hash_string(const char *s)
{
    unsigned long h = 0;
    for (;*s;++s)
    {
        h = 5 * h + *s;
    }

    return size_t(h);
}

//类模板显示特化定义
template <>
struct hash<int>{
    size_t operator()(int x) const {return x;}
};

template <>
struct hash<char *>{
    size_t operator()(const char *s) const {return __stl_hash_string(s);}
};

template <>
struct hash<const char *>{
    size_t operator()(const char *s) const {return __stl_hash_string(s);}
};

template <>
struct hash<char>{
    size_t operator()(char s) const {return s;}
};

template <>
struct hash<unsigned char>{
    size_t operator()(unsigned char s) const {return s;}
};

template <>
struct hash<signed char>{
    size_t operator()(signed char s) const {return s;}
};

// C++ Standard 规定，每一个 Adaptable Unary Function 都必须继承此类别
template <class Arg,class Result>
struct unary_function{
    typedef Arg argument_type;
    typedef Result result_type;
};

// C++ Standard 规定，每一个 Adaptable Binary Function 都必须继承此类别
template <class Arg1,class Arg2,class Result>
struct binary_function{
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Result result_type;
};

//从节点中取出键值的仿函数定义
//identity function；任何数值通过此函数式后，不会发生任何改变
template <class T>
struct identity:public unary_function<T,T>
{
    const T& operator()(const T& x) const{return x;}
};

//判断键值是否相等的仿函数定义
template <class T>
struct equal_to:public binary_function<T,T,bool>{
    bool operator()(const T& x,const T& y) const{return x == y;}
};

//比较字符串是否相等的仿函数
struct eqstr{
    bool operator()(const char *s1,const char *s2)const
    {
        return strcmp(s1,s2) == 0;
    }
};

//hash table数据结构定义
//模板参数：
//ValueType：节点的实值型别
//KeyType：节点的键值型别
//HashFcn：hash function的函数型别
//ExtractKey：从节点中取出键值的方法
//EqualKey：判断键值是否相同
template <class ValueType,class KeyType,class HashFcn,class ExtractKey,class EqualKey>
class HashTableClass{
public:
    typedef struct __hashtable_node<ValueType> node;//hash table内部链表节点定义
    //hash table没有提供默认构造函数
    HashTableClass(size_t n,
        const HashFcn &hf,
        const EqualKey &eql,
        const ExtractKey &ext)
        :hasher(hf),equals(eql),get_key(ext),num_elements(0)
    {
        initialize_buckets(n);
    }
    HashTableClass(size_t n,
        const HashFcn &hf,
        const EqualKey &eql)
        :hasher(hf),equals(eql),get_key(ExtractKey()),num_elements(0)
    {
        initialize_buckets(n);
    }
    HashTableClass(const HashTableClass &ht)
        :hasher(ht.hasher),equals(ht.equals),get_key(ht.get_key),num_elements(0)
    {
        copy_from(&ht);
    }
    ~HashTableClass(){
        clear();
    }
    //赋值操作符
    HashTableClass& operator= (const HashTableClass &ht)
    {
        if (&ht != this)
        {
            clear();
            hasher = ht.hasher;
            equals = ht.equals;
            get_key = ht.get_key;
            copy_from(&ht);
        }
    }
    //返回元素个数
    size_t size(){return num_elements;}
    //最大元素数目
    size_t max_size(){return size_t(-1);}
    //返回bucket vector大小
    size_t bucket_count(){return buckets.size();}
    //返回bucket vector可能的最大值
    size_t max_bucket_count(){return __stl_prime_list[__stl_num_primes - 1];}
    //插入元素，不允许重复
    std::pair<node *,bool> insert_unique(const ValueType &obj);
    //插入元素，允许重复
    node* insert_equal(const ValueType &obj);
    //打印所有节点
    void printAllNodes();
    //遍历所有buckets
    void printAllBuckets();
    //查找某一键值的节点
    std::pair<node*,bool> find(const KeyType &key);
    //判断某一值出现的次数
    size_t count(const KeyType &key);
    //整体删除
    void clear();
    //复制hash表
    void copy_from(const HashTableClass *ht);

private:
    std::vector<node*> buckets;//由桶组成的vector
    size_t num_elements;//总的元素个数
    HashFcn hasher;
    ExtractKey get_key;
    EqualKey equals;

    //节点配置和释放函数
    node* new_node(const ValueType &obj)
    {
        node *tempNode = new node;
        tempNode->next = NULL;
        try
        {
            construct(&tempNode->val,obj);
        }
        catch (...)
        {
            delete tempNode;
            throw;
            return NULL;
        }
        return tempNode;
    }

    void delete_node(node *n)
    {
        destroy(&n->val);
        delete n;
    }

    //初始化buckets vector
    void initialize_buckets(size_t n);

    //返回最接近n并大于等于n的质数
    size_t next_size(size_t n)const{return __get_next_prime(n);}

    //判断是否需要扩充buckets vector，如有需要则进行扩充
    void resize(size_t num_elements_hint);

    //判断元素落在哪个bucket
    //提供两个版本
    //版本一：只接受实值
    size_t bkt_num(const ValueType &obj) const{return bkt_num_key(get_key(obj));}
    //版本二：接受实值和buckets个数
    size_t bkt_num(const ValueType &obj,size_t n) const {return bkt_num_key(get_key(obj),n);}
    //版本一：只接受键值
    size_t bkt_num_key(const KeyType &Key) const{return hasher(Key) % (buckets.size());}
    //版本二：接受键值和buckets个数
    size_t bkt_num_key(const KeyType &Key,size_t n) const {return hasher(Key) % n;}

    //在不需要重新分配bucket vector的情况下插入元素，元素不允许重复
    std::pair<node *,bool> insert_unique_noresize(const ValueType &obj);
    //在不需要重新分配bucket vector的情况下插入元素，元素不允许重复
    node* insert_equal_noresize(const ValueType &obj);
};

template <class ValueType,class KeyType,class HashFcn,class ExtractKey,class EqualKey>
std::pair<typename HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::node *,bool>
HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::insert_unique(const ValueType &obj)
{
    resize(num_elements + 1);

    return insert_unique_noresize(obj);
}

template <class ValueType,class KeyType,class HashFcn,class ExtractKey,class EqualKey>
typename HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::node *
HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::insert_equal(const ValueType &obj)
{
    resize(num_elements + 1);

    return insert_equal_noresize(obj);
}

template <class ValueType,class KeyType,class HashFcn,class ExtractKey,class EqualKey>
void HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::printAllNodes()
{
    cout << endl;
    cout << "Current node in hash table : " << endl;
    for (size_t i = 0;i < buckets.size();++i)
    {
        typename HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::node* curNode = buckets[i];
        while(curNode)
        {
            cout << curNode->val << " ";
            curNode = curNode->next;
        }
    }
    cout << endl;
}

template <class ValueType,class KeyType,class HashFcn,class ExtractKey,class EqualKey>
void HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::printAllBuckets()
{
    cout << endl;
    cout << "Current buckets in hash table : " << endl;
    for (size_t i = 0;i < buckets.size();++i)
    {
        typename HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::node* curNode = buckets[i];
        if(NULL == curNode)
            cout << "buckets[" << i << "] is empty!" << endl;
        else
        {
            size_t count = 0;
            while(curNode)
            {
                ++count;
                curNode = curNode->next;
            }
            cout << "buckets[" << i << "] has " << count << " elements : ";
            curNode = buckets[i];
            while(curNode)
            {
                cout << curNode->val << " ";
                curNode = curNode->next;
            }
            cout << endl;
        }
    }
}

template <class ValueType,class KeyType,class HashFcn,class ExtractKey,class EqualKey>
std::pair<typename HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::node *,bool>
HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::find(const KeyType &key)
{
    size_t bucket_index = bkt_num_key(key);
    node*first = buckets[bucket_index];
    while(first)
    {
        if (equals(key,get_key(first->val)))
        {
            cout << "find the element " << key << " success" << endl;
            return std::pair<typename HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::node *,bool>(first,true);
        }
        first = first->next;
    }

    cout << "cannot find the element " << key << endl;

    return std::pair<typename HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::node *,bool>(NULL,false);
}

template <class ValueType,class KeyType,class HashFcn,class ExtractKey,class EqualKey>
size_t HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::count(const KeyType &key)
{
    size_t bucket_index = bkt_num_key(key);
    node*first = buckets[bucket_index];
    size_t num = 0;
    while(first)
    {
        if (equals(key,get_key(first->val)))
        {
            ++num;
        }
        first = first->next;
    }

    cout << "The element " << key << " appears " << num << " times" << endl;
    return num;
}

template <class ValueType,class KeyType,class HashFcn,class ExtractKey,class EqualKey>
void HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::clear()
{
    for (size_t i = 0;i < buckets.size();++i)
    {
        node* first = buckets[i];
        //删除bucket list中的每个节点
        while(first)
        {
            node *next = first->next;
            delete_node(first);
            first = next;
        }
        buckets[i] = 0;
    }
    //总元素个数置0
    num_elements = 0;

    //vector并未释放掉空间（自动回收）
}

template <class ValueType,class KeyType,class HashFcn,class ExtractKey,class EqualKey>
void HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::copy_from(const HashTableClass *ht)
{
    buckets.clear();//清除已有vector
    //使得bucket vector空间和对方相同
    buckets.reserve(ht->buckets.size());
    //插入n个元素，null
    buckets.insert(buckets.end(),ht->buckets.size(),(typename HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::node *)0);

    for (size_t i = 0;i < ht->buckets.size();++i)
    {
        if (const node *cur = ht->buckets[i])
        {
            node *tempNode = new_node(cur->val);
            buckets[i] = tempNode;
            for (node *next = cur->next;next;next = next->next)
            {
                tempNode->next = new_node(next->val);
                tempNode = tempNode->next;
            }
        }
    }

    num_elements = ht->num_elements;
}

template <class ValueType,class KeyType,class HashFcn,class ExtractKey,class EqualKey>
void HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::initialize_buckets(size_t n)
{
    const size_t n_buckets = next_size(n);
    buckets.reserve(n_buckets);
    buckets.insert(buckets.end(),n_buckets,(node*)0);
}

template <class ValueType,class KeyType,class HashFcn,class ExtractKey,class EqualKey>
void HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::resize(size_t num_elements_hint)
{
    //buckets vector重建与否的标准：
    //比较新的总元素个数和原buckets vector大小
    const size_t old_n_vector = buckets.size();
    //需要重新配置
    if (num_elements_hint > old_n_vector)
    {
        const size_t n = next_size(num_elements_hint);
        if (n > old_n_vector)//有可能vector已到达最大
        {
            vector<node*> tempVec(n,(node *)0);
            for (size_t bucketIndex = 0;bucketIndex < old_n_vector;++bucketIndex)
            {
                node *first = buckets[bucketIndex];//指向节点对应之串行的起始节点
                while(first)
                {
                    //计算节点落在哪一个新的bucket内
                    size_t new_bucket_index = bkt_num_key(first->val,n);
                    buckets[bucketIndex] = first->next;
                    first->next = tempVec[new_bucket_index];
                    tempVec[new_bucket_index] = first;
                    first = buckets[bucketIndex];
                }
            }
            //交换新旧两个bucket vector
            buckets.swap(tempVec);
        }
    }
}

template <class ValueType,class KeyType,class HashFcn,class ExtractKey,class EqualKey>
std::pair<typename HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::node *,bool>
HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::insert_unique_noresize(const ValueType &obj)
{
    size_t bucket_index = bkt_num(obj);
    node *first = buckets[bucket_index];
    //搜索当前链表
    for (node *curNode = first;curNode;curNode = curNode->next)
    {
        if(equals(get_key(obj),get_key(curNode->val)))
            return std::pair<typename HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::node*,bool>(curNode,false);
    }

    node *tempNode = new_node(obj);
    tempNode->next = first;
    buckets[bucket_index] = tempNode;

    ++num_elements;

    return std::pair<typename HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::node*,bool>(tempNode,true);
}

template <class ValueType,class KeyType,class HashFcn,class ExtractKey,class EqualKey>
typename HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::node*
HashTableClass<ValueType,KeyType,HashFcn,ExtractKey,EqualKey>::insert_equal_noresize(const ValueType &obj)
{
    size_t bucket_index = bkt_num(obj);
    node *first = buckets[bucket_index];
    for (node *curNode = first;curNode;curNode = curNode->next)
    {
        //发现与链表中的某键值相等，马上插入，然后返回
        if (equals(get_key(obj),get_key(curNode->val)))
        {
            node *tempNode = new_node(obj);
            tempNode->next = curNode->next;
            curNode->next = tempNode;
            ++num_elements;
            return tempNode;
        }
    }

    //如果没发现键值相等的元素
    node *tempNode = new_node(obj);
    //将其插入链表头部
    tempNode->next = first;
    buckets[bucket_index] = tempNode;
    ++num_elements;
    return tempNode;
}
#endif
