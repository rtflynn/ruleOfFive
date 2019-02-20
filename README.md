# Brief intro to the Rule of Five

When we define our own classes in C++, the compiler comes up with all sorts of 'default' functionality for us.  In the following snippet of code, for example, we define a class, instantiate some members, and perform a typical operation:

```C++
class BasicClass
{
  int m_myInt;
  BasicClass(int someInt = 0) : m_myInt(someInt) {}
  void saySomething() { std::cout << "My integer is " << m_myInt << std::endl; }
}

int main() 
{
  BasicClass x;  
  x = BasicClass(5);
 
  BasicClass y = x;  
  y.saySomething();
}
```

Although this seems pretty straightforward, there's actually a lot going on here.  In the first line of ```main()```, we create a ```BasicClass``` object ```x``` with ```m_myInt``` value 0.  In the second line we create a temporary object via ```BasicClass(5)``` and then copy all the data from this temporary object to ```x```.  This is done behind the scenes, and is an example of a 'shallow copy'.  A shallow copy is achieved by copying *exactly* the values contained in the fields of one object into the corresponding fields of another object.  Next (and quite invisibly to us!), our temporary object is destroyed before moving on.  

In the third line of ```main()```, we create a new ```BasicClass``` object y and initialize its values to those of ```x```.  This is similar but not exactly the same as what happened when we created ```x```:  Here, we did not (read: the compiler did not) first create ```y``` with a temporary ```m_myInt``` value of 0.  Rather, we (the compiler) created ```y``` by directly copying the fields of ```x``` into ```y``` (this is another shallow copy).  The final line does exactly what one would think: it prints the line "My integer is 5" to the console.  

We're still not done!  Once we leave main(), ```x``` and ```y``` go out of scope, so they get automatically destroyed.  

In total, there were three compiler-generated functions at work here.  We only ever defined a constructor for our class --- we never told it how to copy the contents of one object into another, or how to initialize an object using another object.  We also never told the compiler how to destroy our objects.  This is actually fine.  There's no danger in using these compiler-generated functions for this class.


So, then, for what sorts of classes could this cause problems?






```C++
int main() {}
```








