MaxSize =256
class Stack:
    def __init__(self):
        self.array = [None]* MaxSize
        self.top = -1
    def empty(self):
        if self.top == -1:
            return True
        else:
            return False
    def push(self,x):
        if self.top == MaxSize-1:
            print("have no space.")
            return
        self.top+=1
        self.array[self.top] = x
    def pop(self):
        if self.empty():
            print("empty error:")
            return -1
        self.top-=1
        return self.array[self.top+1]
    def top(self):
        if self.empty():
            print("empty error:")
            return -1
        else:
            return self.array[self.top]
    def showStack(self):
        if self.empty():
            print("empty error:")
        ptop=self.top
        while ptop>=0:
            print(" ",self.array[ptop])
            ptop-=1
if __name__ == "__main__":
    mystack=Stack()
    mystack.push(3)
    mystack.push(4)
    mystack.push(7)
    mystack.push(5)
    mystack.push(1)
    if mystack.empty():
        print("kong")
    else:
        print("not kong")
    mystack.showStack()





