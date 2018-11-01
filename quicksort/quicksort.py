def quick_sort(array, l, r):
    if l < r:
        ret = partition(array, l, r)
        quick_sort(array, l, ret-1)
        quick_sort(array, ret + 1, r)

def partition(array, l, r):
    solder = array[r]
    i=l
    j=l
    while j< r:
        if array[j] >= solder:
            j+=1
        else:
            array[j],array[i]=array[i],array[j]  #[9, 4, 3, 2, 11, 7, 6, 5, 8]
            j+=1
            i+=1
    if j == r and i != j:
        array[j], array[i] = array[i], array[j]
    return i

"""
def quick_sort(array, l, r):
    if l < r:
        q = partition(array, l, r)
        quick_sort(array, l, q - 1)
        quick_sort(array, q + 1, r)

def partition(array, l, r):
    x = array[r]
    i = l - 1
    for j in range(l, r):
        if array[j] <= x:
            i += 1
            array[i], array[j] = array[j], array[i]
    array[i + 1], array[r] = array[r], array[i+1]
    return i + 1

"""

if __name__ == "__main__":
    arr = [9, 4, 3, 2, 8, 7, 6, 5, 1, 0]
    quick_sort(arr, 0, len(arr) - 1)
    print("快速排序：", arr)

    #arr=[1,2,5,6,8,9]
    #quicksort(arr,int(0),int(5))
    #print("paixuhou:",arr)
