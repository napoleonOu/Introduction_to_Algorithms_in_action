#include <iostream>
#include "hashtable.h"

using std::cout;
using std::endl;

int main()
{

    hashtable<int, int, hash<int>, identity<int>, equal_to<int>>
        iht(50, hash<int>(), equal_to<int>());
    cout << "bucket count =" << iht.bucket_count() << endl;
    iht.insert_unique(59);
    iht.insert_unique(63);
    iht.insert_unique(108);
    iht.insert_equal(108);
    hashtable<int, int, hash<int>, identity<int>, equal_to<int>>::iterator iter = iht.begin();
    //cout << *iter;

    for (int i = 0; i<iht.size();i++,++iter)
    {
        cout << " " << *iter;
    }
    //system("pause");
    return 0;
}
