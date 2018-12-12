/*stl源码剖析
* 学习笔记*/
//#if 0
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
    for (; *s; ++s)
    {
        h = 5 * h + *s;
    }

    return size_t(h);
}
//类模板显示特化定义
template <>
struct hash<int>{
    size_t operator()(int x) const { return x; }
};

template <>
struct hash<char *>{
    size_t operator()(const char *s) const { return __stl_hash_string(s); }
};

template <>
struct hash<const char *>{
    size_t operator()(const char *s) const { return __stl_hash_string(s); }
};

template <>
struct hash<char>{
    size_t operator()(char s) const { return s; }
};

template <>
struct hash<unsigned char>{
    size_t operator()(unsigned char s) const { return s; }
};

template <>
struct hash<signed char>{
    size_t operator()(signed char s) const { return s; }
};


template <class T1, class T2>
void construct(T1 *p, const T2 &value)
{
    new (p)T1(value);
}
template <class T1, class T2>
void destory(T1 *p)
{
    if (nullptr != p)
    {
        delete p;
        p = nullptr;
    }
}

// C++ Standard 规定，每一个 Adaptable Unary Function 都必须继承此类别
template <class Arg, class Result>
struct unary_function{
    typedef Arg argument_type;
    typedef Result result_type;
};

// C++ Standard 规定，每一个  Adaptable Binary Function 都必须继承此类别
template <class Arg1, class Arg2, class Result>
struct binary_function{
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Result result_type;
};

//从节点中取出键值的仿函数定义
//identity function；任何数值通过此函数式后，不会发生任何改变
template <class T>
struct identity :public unary_function<T, T>
{
    const T& operator()(const T& x) const{ return x; }
};

//判断键值是否相等的仿函数定义
template <class T>
struct equal_to :public binary_function<T, T, bool>{
    bool operator()(const T& x, const T& y) const{ return x == y; }
};

template<class T, class Alloc>
class simple_alloc
{
public:

    T* allocate(size_t n)
    {
        return 0 == n ? 0 : (T*)alloc.allocate(n*sizeof(T));
    }

    T* allocate(void)
    {
        return (T*)alloc.allocate(sizeof(T));
    }

    void deallocate(T* p, size_t n)
    {
        if (0 != n) alloc.deallocate(p, n*sizeof(T));
    }
    void deallocate(T* p)
    {
        alloc.deallocate(p, sizeof(T));
    }
    Alloc alloc;

};
template <class Value>
struct __hashtable_node{
    __hashtable_node* next;
    Value val;
};
template<class Value, class Key, class HashFcn,
class ExtractKey, class EqualKey, class Alloc = std::allocator<__hashtable_node<Value> > >
class hashtable;

template<class Value, class Key, class HashFcn,
class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_iterator{
    typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
        hashtable;
    typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
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

    __hashtable_iterator(node* n, hashtable* tab) :cur(n), ht(tab){}
    __hashtable_iterator(){}

    referance operator*()const { return cur->val; }
    pointer operator->()const { return &(operator *()); }
    iterator& operator++();
    iterator operator++(int);
    bool operator ==(const iterator& iter)const{ return iter.cur == cur; }
    bool operator !=(const iterator& iter)const{ return iter.cur != cur; }
};
template<class Value, class Key, class HashFcn,
class ExtractKey, class EqualKey, class Alloc>
    __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>&
    __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::operator++()
{
    const node* p_old = cur;
    cur = cur->next;
    if (!cur){
        size_type bucket = ht->btk_num(p_old->val);
        while (!cur&& ++bucket < ht->bucket_count()){//非空，且++bucket，必须小于它buckets的大小
            cur = ht->buckets[bucket];
        }
    }
    return *this;
}
template<class Value, class Key, class HashFcn,
class ExtractKey, class EqualKey, class Alloc>
    inline __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
    __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::operator++(int)
{
    iterator tmp = *this;
    ++*this;//
    return tmp;
}
static const int __stl_num_primes = 28;
static const unsigned long __stl_prime_list[__stl_num_primes] =
{ 53, 97, 193, 389, 769, 1543, 3079, 6151, 12289,
24593, 49157, 98317, 196613, 393241, 786433,
1572869, 3145739, 6291469, 12582917, 25165843,
50331653, 100663319, 201326611, 402653189, 805306457,
1610612741, 3221225473, 4294967291 };

template<class Value, class Key, class HashFcn,
class ExtractKey, class EqualKey, class Alloc>
class hashtable{
public:
    friend class __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
    typedef Value value_type;
    typedef HashFcn hasher;
    typedef EqualKey key_equal;
    typedef size_t size_type;
    typedef typename __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
        iterator;

private:
    hasher hash;
    key_equal equals;
    ExtractKey get_key;


    typedef __hashtable_node<Value> node;
    typedef simple_alloc<node, Alloc> node_allocator;

    vector<node*, Alloc> buckets;
    size_type num_elements;
    node_allocator alloc;
public:
    hashtable(size_type n,
        const HashFcn& hf,
        const EqualKey& eql)
        :hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0)
    {
        initialize_buckets(n);
    }
    void initialize_buckets(size_type n)
    {
        const unsigned long  n_buckets = next_size(n);
        buckets.reserve(n_buckets);
        buckets.insert(buckets.end(), n_buckets, (node*)0);
        num_elements = 0;
    }
    void resize(size_type st);
    size_type next_size(size_type n)
    {
        return __stl_next_prime(n);
    }

    size_type size()const{ return num_elements; }
    size_type bucket_count()const{ return buckets.size(); }
    typedef typename simple_alloc<node, Alloc> node_allocator;
    inline unsigned long __stl_next_prime(unsigned long n)
    {
        const unsigned long* first = __stl_prime_list;
        const unsigned long* last = __stl_prime_list + __stl_num_primes;
        const unsigned long* pos = std::lower_bound(first, last, n);
        return pos == last ? *(last - 1) : *pos;
    }
    size_type btk_num(const value_type& obj, size_type st)
    {
        return hash(get_key(obj)) % st;
    }
    size_type btk_num(const value_type& obj)
    {
        //cout << "get_key(obj)==" << get_key(obj) << endl;
        //cout << "hash(get_key(obj)) ==" << hash(get_key(obj));
        //cout << "buckets.size()==" << buckets.size()<<endl;
        return hash(get_key(obj)) % buckets.size();
    }
    size_type max_bucket_size() const
    {
        return __stl_prime_list[__stl_num_primes - 1];
    }
    node* new_node(const value_type& vt)
    {
        node* n = alloc.allocate();
        n->val = 0;
        construct(&n->val, vt);
        return n;
    }
    void delete_node(node* n)
    {
        destory(&n->val);
        node_allocator::deallocate(n);
    }
    std::pair<iterator, bool> insert_unique(const value_type& obj)
    {
        resize(num_elements + 1);
        return insert_unique_noresize(obj);
    }
    std::pair<iterator, bool> insert_unique_noresize(const value_type& obj);
    iterator insert_equal(const value_type& obj)
    {
        resize(num_elements + 1);
        return insert_equal_noresize(obj);
    }
    iterator insert_equal_noresize(const value_type& obj);
    iterator begin()
    {
        if (num_elements != 0)
        {
            for (int i = 0; i < buckets.size(); i++)
            {
                if (buckets[i] != nullptr)
                    return iterator(buckets[i], this);
            }
        }

        else
            return iterator();
    }
    node* end()
    {
        if (num_elements != 0)
        {
            iterator beg = begin();
            for (int i = 0; i<num_elements; i++)
            {
                beg++
            }
            return beg;
        }
        else
            return nullptr;

    }

};
//重建表格，是以现有元素+1 和vector的大小相比，
//这个原则 比较 有意思
template<class Value, class Key, class HashFcn,
class ExtractKey, class EqualKey, class Alloc>
    void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::resize(size_type num_ele)
{
    const size_type n_old = buckets.size();
    if (num_ele > n_old)
    {
        const size_type n = next_size(num_ele);
        if (n > n_old)//重新分配
        {
            std::vector<node*, Alloc> tmp(n, (node*)0);
            for (size_type bucket = 0; bucket != n_old; ++bucket)//遍历所有节点
            {
                node* first = buckets[bucket];
                while (first){
                    size_type new_buckets = btk_num(first->val, n);
                    buckets[bucket] = first->next;//把首元素 指向first的下一个元素
                    first->next = tmp[new_buckets];//把新链表的头 接到first元素的后面
                    tmp[new_buckets] = first;//把first当初新链表的头
                    first = buckets[bucket];
                }
            }
            buckets.swap(tmp);
        }
    }
}
template<class Value, class Key, class HashFcn,
class ExtractKey, class EqualKey, class Alloc>
    std::pair<typename hashtable<Value, Key, HashFcn,
    ExtractKey, EqualKey, Alloc>::iterator, bool>
    hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::insert_unique_noresize(const value_type& obj)
{
    const size_type n = btk_num(obj);
    node* first = buckets[n];
    for (node* cur = first; cur; cur = cur->next)
    {
        if (equals(get_key(cur->val), get_key(obj)))
            return std::pair<iterator, bool>(iterator(cur, this), false);//???
    }
    node *tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    ++num_elements;
    return std::pair<iterator, bool>(iterator(tmp, this), false);
}

template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
__hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::insert_equal_noresize(const value_type& obj)
{
    const size_type n = btk_num(obj);
    node* first = buckets[n];
    for (node* cur = first; cur; cur = cur->next) //找相同值，找到则插入
    {
        if (equals(get_key(obj), get_key(cur->val)))
        {
            node* nn = new_node(obj);
            nn->next = cur->next;
            cur->next = nn;
            cur = cur->next;
            num_elements++;
            return iterator(nn, this);
        }
    }
    //没找到相同的值，直接插入到头部
    node* tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    num_elements++;
    return iterator(tmp, this);
}

