# Brief intro to the Rule of Five

## Shallow Copies

When we define our own classes in C++, the compiler comes up with all sorts of 'default' functionality for us.  In the following snippet of code, for example, we define a class, instantiate some members, and perform a typical operation:

```C++
class BasicClass
{
  int m_myInt;
  BasicClass(int someInt = 0) : m_myInt(someInt) {}
  void saySomething() { std::cout << "My integer is " << m_myInt << std::endl; }
};

int main() 
{
  BasicClass x;  
  x = BasicClass(5);
 
  BasicClass y = x;  
  y.saySomething();
  
  return 1;
}
```

Although this seems pretty straightforward, there's actually a lot going on here.  In the first line of ```main()```, we create a ```BasicClass``` object ```x``` with ```m_myInt``` value 0.  In the second line we create a temporary object via ```BasicClass(5)``` and then copy all the data from this temporary object to ```x```.  This is done behind the scenes, and is an example of a 'shallow copy'.  A shallow copy is achieved by copying *exactly* the values contained in the fields of one object into the corresponding fields of another object.  Next (and quite invisibly to us!), our temporary object is destroyed before moving on.  

In the third line of ```main()```, we create a new ```BasicClass``` object y and initialize its values to those of ```x```.  This is similar but not exactly the same as what happened when we created ```x```:  Here, we did not (read: the compiler did not) first create ```y``` with a temporary ```m_myInt``` value of 0.  Rather, we (the compiler) created ```y``` by directly copying the fields of ```x``` into ```y``` (this is another shallow copy).  The final line does exactly what one would think: it prints the line "My integer is 5" to the console.  

We're still not done!  Once we leave main(), ```x``` and ```y``` go out of scope, so they get automatically destroyed.  

In total, there were three compiler-generated functions at work here.  We only ever defined a constructor for our class --- we never told it how to copy the contents of one object into another, or how to initialize an object using another object.  We also never told the compiler how to destroy our objects.  This is actually fine.  There's no danger in using these compiler-generated functions for this class.


So, then, for what sorts of classes could this cause problems?

## Deep Copies

Consider a class with a member variable which is a pointer to some other object, or a class which dynamically allocates memory.  For example, 

```C++
class ShallowBad
{
  int * m_myIntPtr;
  ShallowBad(int someInt = 0) 
  {
    m_myIntPtr = new int;
    *m_myIntPtr = someInt;
  }
  void saySomething() { std::cout << "My pointer location is " << m_myIntPtr << " and the integer it points at is " << *m_myIntPtr << std::endl; }
};

int main() 
{
  ShallowBad x;  
  x = ShallowBad(5);
 
  ShallowBad y = x;  
  x.saySomething();
  y.saySomething();
  
  *y.m_myIntPtr = 7;
  x.saySomething();
  y.saySomething();
  
  return 1;
}
```

When we create `y` we're again performing a shallow copy, so each field of `x` is copied over to `y` exactly.  Thus `y` actually has a copy of the integer pointer contained in `x`, and consequently any changes to `*x.m_myIntPtr` will also affect `*y.m_myIntPtr`.  If we run the example above, 4 lines will be output to the console.  On my machine they were:

My pointer location is 00A40568 and the integer it points at is 5

My pointer location is 00A40568 and the integer it points at is 5

My pointer location is 00A40568 and the integer it points at is 7

My pointer location is 00A40568 and the integer it points at is 7

(The pointer location will likely be different each time you run this).  So both objects `x` and `y` share the variable value `*m_myIntPtr` and this will be the case as long as both objects exist.  We can not change the values of `*x.m_myIntPtr` and `*y.m_myIntPtr` independently of each other.  This might sometimes be a desirable thing - but let's assume that this is not the functionality we were hoping for, and that we want to be able to change the integer `y` points to without affecting the one `x` points to.  To pull this off, we'll need to replace the compiler's default copy constructor and copy assignment operators to make *deep* copies.  The following code pulls this off:


```C++
class BetterCopies
{
  int * m_myIntPtr;
  BetterCopies(int someInt = 0) // Constructor
  {
    m_myIntPtr = new int;
    *m_myIntPtr = someInt;
    
    std::cout << "Using the constructor to create an object... "
			<< "The new object's pointer location is " << m_myIntPtr
			<< " and the value it points to is " << *m_myIntPtr << "." << std::endl;
  }
  
  BetterCopies(const BetterCopies& original)    // Copy Constructor
  {
    m_myIntPtr = new int;
    *m_myIntPtr = *original.m_myIntPtr;
    
    std::cout << "Using the copy constructor to create an object... " 
            << "The new object's pointer location is " << m_myIntPtr 
            << " and the value it points at is " << *m_myIntPtr << "." << std::endl;
            
    std::cout << "The original object had location " << original.m_myIntPtr 
            << " and value " << *original.m_myIntPtr << "." << std::endl;
  }
  
  BetterCopies& operator=(const BetterCopies& original)   // Copy Assignment
  {
    if (this != &original)
    {
        m_myIntPtr = new int;
        *m_myIntPtr = *original.m_myIntPtr;
	
        std::cout << "Using the copy assignment operator to assign values to an object... " 
            << "The new object's pointer location is " << m_myIntPtr 
            << " and the value it points at is " << *m_myIntPtr << "." << std::endl;
            
        std::cout << "The original object had location " << original.m_myIntPtr 
            << " and value " << *original.m_myIntPtr << "." << std::endl; 
    }
  return *this;
  }
  
  ~BetterCopies()         // Destructor
  {
  std::cout << "Destroying object with pointer location " << m_myIntPtr << std::endl;
  
  delete m_myIntPtr;
  m_myIntPtr = nullptr;
  }
  
  void saySomething() { std::cout << "My pointer location is " << m_myIntPtr << " and the integer it points at is " << *m_myIntPtr << std::endl; }
};

int main() 
{
  BetterCopies x;  
  x = BetterCopies(5);
 
  BetterCopies y = x;  
  x.saySomething();
  y.saySomething();
  
  *y.m_myIntPtr = 7;
  x.saySomething();
  y.saySomething();
  
  return 1;
}
```


On my machine this program produced the following console output:


Using the constructor to create an object... The new object's pointer location is 016F97E0 and the value it points to is 0.

Using the constructor to create an object... The new object's pointer location is 016F9BB0 and the value it points to is 5.

Using the copy assignment operator to assign values to an object... The new object's pointer location is 016F6728 and the value it points at is 5.

The original object had location 016F9BB0 and value 5.

Destroying object with pointer location 016F9BB0

Using the copy constructor to create an object... The new object's pointer location is 016F6758 and the value it points at is 5.

The original object had location 016F6728 and value 5.

My pointer location is 016F6728 and the integer it points at is 5

My pointer location is 016F6758 and the integer it points at is 5

My pointer location is 016F6728 and the integer it points at is 5

My pointer location is 016F6758 and the integer it points at is 7

Destroying object with pointer location 016F6758

Destroying object with pointer location 016F6728




We see that the first line `BetterCopies x` creates an object with the default constructor.  The second line `x = BetterCopies(5)` first creates an object via `BetterCopies(5)` and then uses the copy assignment operator to copy this new object's fields to `x`.  At the end of this line, the temporary object we just created is destroyed via the destructor.

The next line `BetterCopies y = x` uses the copy constructor to create `y` and initialize its values to be (deep) copies of `x`'s values.  The rest of the program is self-explanatory, and of course at the end of `main()` we destroy `x` and `y` via the destructor.



For quick reference, the important syntax is:

```C++
class myClass
{
  myClass(some params) {   .....   }    // Constructor
  myClass& operator=(const myClass& someObject) {  .....  }   // Copy assignment
  myClass(const myClass& someObject) {  .....  }  // Copy constructor
  ~myClass() {  .....  }  // Destructor
}
```










```C++
int main() {}
```








