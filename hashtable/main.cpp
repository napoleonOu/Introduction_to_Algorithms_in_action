#include <iostream>
#include "hashtable.h"

using std::cout;
using std::endl;

int main()
{

    hashtable<int,int,hash<int>,identity<int>,equal_to<int>>
            iht(50,hash<int>(),equal_to<int>());


#if 0
    HashTable<int,int> ht(50);
    for(int i=0;i!=50;i++){
        std::string tmp="aaa";
        auto p1=std::make_pair(i,i+1000);
        ht.Insert(p1);
    }
    std::cout<<(ht.Find(8))->_kv.second;
    ;
#endif


    //cout<<"bucket count = "<<iht.bucket_count()<<endl;
    return 0;
}
