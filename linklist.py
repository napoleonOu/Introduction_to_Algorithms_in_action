class Node(object):
    def __init__(self,val,p=0):
        self.data=val
        self.next=p
class LinkLists(object):
    def __init__(self):
        self.head=0

    def __getitem__(self, key):
        if self.is_empty():
            print("linklist is empty")
            return
        elif key < 0 or key > self.getlength():
            print("the key gived is error")
            return
        else:
            self.getitem(key)
    def __setitem__(self, key, value):
        if self.is_empty():
            print("linklist is empty")
            return
        elif key < 0 or key > self.getlength():
            print("the key gived is error")
            return
        else:
            self.delete(key)
            return self.insert(key)
    def initlist(self,data):
        self.head = Node(data[0])
        p=self.head

        for i in data[1:]:
            node = Node(i)
            p.next = node
            p=p.next #这 里没有指针的概念的话 python是怎么表达指针的
    def getlength(self):
        p =self.head
        length=0
        while p!=0:
            length+=1
            p=p.next
        return length
    def is_empty(self):
        if self.getlength()==0:
            return True
        else:
            return False
    def clear(self):
        self.head=0

    def append(self,item):
        q=Node(item)
        if self.head==0:
            self.head=q
        else:
            p=self.head
            while p.next != 0:
                p=p.next
            p.next = q

    def getitem(self,index):
        if self.is_empty():
            print("is empty")
            return
        j=0
        p=self.head
        while p.next!=0 and j<index:
            p=p.next
            j+=1
        if j==index:
            return p.data
        else:
            print("index nont exist")
    def insert(self,index,item):
        if self.is_empty() or index <0 or index > self.getlength():
            print("is empty or index invalid")
            return
        if index==0:
            q=Node(item,self.head)
            self.head=q
            return
        p=self.head
        post=self.head
        j=0
        while p.next!=0 and j< index:
            post = p
            p=p.next
            j+=1
        if j==index:
            q=Node(item,p)
            post.next=q
            q.next=p #不能忘记
    def delete(self,index):
        if self.is_empty() or index <0 or index > self.getlength():
            print("is empty or index invalid")
            return
        if index == 0:
            p=self.head
            p=p.next
            self.head=p
            return
        p=self.head
        post=self.head
        j=0
        while p.next!=0 and j< index:
            post = p
            p=p.next
            j+=1
        if j==index:
            post.next=p.next
    def index(self,value):
        if self.is_empty():
            print("is empty")
            return
        p=self.head
        j=0
        while p.next!=0 and  not value == p.data:
            p=p.next
            j+=1
        if p.data == value:
            return j
        else:
            return -1
l = LinkLists()
l.initlist([1,2,3,4,5])
print (l.getitem(4))
l.append(6)
print (l.getitem(5))

l.insert(4,40)
print (l.getitem(3))
print (l.getitem(4))
print (l.getitem(5))

l.delete(5)
print (l.getitem(5))

l.index(5)


