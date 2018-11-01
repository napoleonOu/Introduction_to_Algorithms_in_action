# 堆排序实现
"""
def MaxHeapity(arr,index,max_arr_size):
    if 2*index+1 > max_arr_size:
        max_index=index
    elif 2*index+1 == max_arr_size:
        if arr[2*index+1] > arr[index]:
            max_index=2*index+1
        else:
            max_index=index
    else:
        if arr[2*index+1] <= arr[index] and arr[2*index+2] <= arr[index]:
            max_index=index
        elif arr[2*index+2] > arr[2*index+1]:
            max_index=2*index+2
        else:
            max_index=2*index+1
    if max_index != index:
        MaxHeapity(arr,max_index,max_arr_size)
    arr[index], arr[max_index] = max_index[max_index], arr[index]
"""
def MaxHeapity(arr,index,max_arr_size):
    if max_arr_size <= 1:
        return
    l=2*index+1
    r=2*index+2
    largest=index
    if l <= max_arr_size-1 and arr[l] > arr[index]:
        largest =l
    if r <= max_arr_size-1 and arr[r] > arr[largest]:
        largest =r
    if index != largest:
        arr[index],arr[largest]=arr[largest],arr[index]
        MaxHeapity(arr,largest,max_arr_size)
# 为什么要从后往前
def BuildMaxHeap(arr,max_arr_size):
    if max_arr_size==1:
        return
    i = max_arr_size//2-1
    print("i=",i)
    while i>=0:
        MaxHeapity(arr, i, max_arr_size)
        print("arr======",arr)
        i-=1

def HeapSort(arr):
    if len(arr) ==1:
        return
    max_arr_size=len(arr)
    print("max_arr_size =",max_arr_size)
    BuildMaxHeap(arr, max_arr_size)
    print("after buildmaxheap:",arr)  #[16,14,10,8,7,9,3,2,4,1]
    i=max_arr_size-1
    while i>=0:
        arr[0],arr[i]=arr[i],arr[0]
        max_arr_size-=1
        MaxHeapity(arr,0,max_arr_size)
        i-=1

if __name__ == "__main__":
    AA=[4,1,3,2,16,9,10,14,8,7]
    print("paixuqian:",AA)
    HeapSort(AA)
    print("paixuhou:",AA)