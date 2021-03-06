#include "stdafx.h"
#include <iostream>


// The rules of three/five have to do with setting up constructors for classes containing pointers / dynamic arrays
// as member variables.  Suppose an object A of class myClass contains a pointer to an integer myIntPtr, with (say)
// *(A->myIntPtr) = 7 and A->myIntPtr = some address in memory.  Now suppose we create a new myClass B = A.
// What we want here is for B to be a copy of A, so that we can do whatever we like to B and A won't be affected.
// What ends up happening, however, is a so-called shallow copy, so that B->myIntPtr = A->myIntPtr, so that making
// changes to B will end up causing changes to A as well.  
// Well, in this situation it's on us to spell out how we want copies to be made --- i.e. replace the 'shallow' copy constructor
// with our own custom copy constructor.  


class myClassWithShallowCopy
{
public:
	int * myIntPtr;
	myClassWithShallowCopy(int x)
	{
		myIntPtr = new int;
		*myIntPtr = x;
	}

	void describeSelf() {
		std::cout << "My pointer is " << myIntPtr << ", and the value contained there is " << *myIntPtr << "." << std::endl;
	}
};

void showProblemWithShallowCopy()
{
	myClassWithShallowCopy x = myClassWithShallowCopy(42);
	x.describeSelf();
	std::cout << "Making a shallow copy of this object... " << std::endl;
	myClassWithShallowCopy y = x;
	y.describeSelf();
	std::cout << "Now, we're going to change the value stored in the copy... " << std::endl;

	*(y.myIntPtr) = 33;
	y.describeSelf();
	x.describeSelf();
	std::cout << "BOTH objects were affected!" << std::endl;
	std::cout << std::endl;
}


class myClassWithCustomCopy
{
public:
	int * myIntPtr;
	myClassWithCustomCopy(int x)
	{
		myIntPtr = new int;
		*myIntPtr = x;
	}

	myClassWithCustomCopy(const myClassWithCustomCopy &A)	
	{
		myIntPtr = new int;
		*myIntPtr = *(A.myIntPtr);
	}

	void describeSelf() {
		std::cout << "My pointer is " << myIntPtr << ", and the value contained there is " << *myIntPtr << "." << std::endl;
	}
};


void showImprovementWithCustomCopy()
{
	myClassWithCustomCopy x = myClassWithCustomCopy(42);
	x.describeSelf();
	std::cout << "Making a copy of this object... " << std::endl;
	myClassWithCustomCopy y = x;
	y.describeSelf();
	std::cout << "Now, we're going to change the value stored in the copy... " << std::endl;

	*(y.myIntPtr) = 33;
	y.describeSelf();
	x.describeSelf();
	std::cout << "Wow!  The objects behave how we want them to!" << std::endl;
	std::cout << std::endl;

}

void showProblemWithCustomCopy() {
	myClassWithCustomCopy x = myClassWithCustomCopy(42);
	x.describeSelf();
	std::cout << "Making a copy of this object... " << std::endl;
	myClassWithCustomCopy y = x;
	y.describeSelf();
	std::cout << "Now using the copy assignment operator (y = x) ... " << std::endl;
	y = x;
	std::cout << "Now, we're going to change the value stored in the copy... " << std::endl;

	*(y.myIntPtr) = 33;
	y.describeSelf();
	x.describeSelf();
	std::cout << "Uh oh!  It looks like our copy is affecting the original again!" << std::endl;
	std::cout << std::endl;
}



class myClassWithCopyConstructorAndCopyAssignment
{
public:
	int * myIntPtr;
	myClassWithCopyConstructorAndCopyAssignment(int x)
	{
		myIntPtr = new int;
		*myIntPtr = x;
	}

	myClassWithCopyConstructorAndCopyAssignment(const myClassWithCopyConstructorAndCopyAssignment &A)
	{
		myIntPtr = new int;
		*myIntPtr = *(A.myIntPtr);
	}

	myClassWithCopyConstructorAndCopyAssignment& operator=(const myClassWithCopyConstructorAndCopyAssignment &A)
	{
		if (this != &A)							// Do nothing in the case of  ' x = x '.
		{
			int * newIntPtr = new int;			// Here, our object already exists and has already been initialized.
			*newIntPtr = *(A.myIntPtr);			// So we're not initializing it here... instead we're copying the relevant fields over.
			delete myIntPtr;							// We need to delete the stuff that was here or else we leak memory.
			myIntPtr = newIntPtr;


			// Question:  Why don't we just say something like *myIntPtr = *A.myIntPtr  ?
			// Maybe this is too simple to see why.  So instead of having a member myIntPtr which is a pointer to an integer, 
			// consider a member which is a pointer to a more complicated structure which might have its own pointers and/or
			// dynamically allocated memory.  If we say, then, *myObjPtr = *A.myObjPtr, then we end up using myObjPtr's copy assignment
			// operator.  Whereas if we do things as above, then we use myObjPtr's copy constructor, and just pass the pointer over.  
			// As long as we've followed the rule of 5 with the myObj class, it shouldn't matter which we do.
		}

		return *this;
	}

	void describeSelf() {
		std::cout << "My pointer is " << myIntPtr << ", and the value contained there is " << *myIntPtr << "." << std::endl;
	}
};


void showImprovementWithCustomCopyAssignment() {
	myClassWithCopyConstructorAndCopyAssignment x = myClassWithCopyConstructorAndCopyAssignment(42);
	x.describeSelf();
	std::cout << "Making a copy of this object... " << std::endl;
	myClassWithCopyConstructorAndCopyAssignment y = x;
	y.describeSelf();			//What the actual fuck... It's making a shallow copy?!?!?  WHY?
	std::cout << "Now using the copy assignment operator (y = x) ... " << std::endl;
	y = x;

	y.describeSelf();	//temp
	x.describeSelf();	//temp
	std::cout << "Now, we're going to change the value stored in the copy... " << std::endl;


	*(y.myIntPtr) = 33;
	y.describeSelf();
	x.describeSelf();
	std::cout << std::endl;
}

// So we have copy constructor and copy assignment. So, myClass A = B;  myClass A(B);  myClass A, then A = B;  all work.
// The last two are the move constructor and the move assignment operator.
// Move constructor:  myObject(myObject&& A)
// Move assignment:   myObject& operator=(myObject&& A)

class showProblemWithNoMoveSemantics
{
	///// The above class works fine but can be wasteful as we'll see:
public:
	int * myPtr;

	showProblemWithNoMoveSemantics(int x = 0)
	{			
		myPtr = new int;
		*myPtr = x;
		std::cout << "Constructing with the usual constructor... with x-value " << x << "." << std::endl;
	}

	showProblemWithNoMoveSemantics(const showProblemWithNoMoveSemantics &other)
	{
		myPtr = new int;
		*myPtr = *(other.myPtr);
		std::cout << "Constructing with the copy constructor..." << std::endl;
	}

	showProblemWithNoMoveSemantics& operator=(const showProblemWithNoMoveSemantics &other)
	{
		if (this != &other) 
		{
			//delete myPtr;
			*myPtr = *other.myPtr;
			std::cout << "Using the copy assignment operator..." << std::endl;
		}
		return *this;
	}

	~showProblemWithNoMoveSemantics()
	{
		std::cout << "Destroying object..." << std::endl;
		delete myPtr;
		myPtr = nullptr;
	}

	void describeSelf()
	{
		std::cout << "My pointer is " << myPtr << " and the value contained there is " << *myPtr << "." << std::endl;
	}
	

};


showProblemWithNoMoveSemantics createObject(int x)
{
	showProblemWithNoMoveSemantics someObject = showProblemWithNoMoveSemantics(x);
	return someObject;
}


void showProblemWithNoMoveSemanticsFunction()
{
	std::cout << std::endl << std::endl;
	std::cout << "Without move semantics, here's what happens when we do myClass x; x = createObjectOfMyClass(some args);" << std::endl << std::endl;
	showProblemWithNoMoveSemantics x;
	x = createObject(42);
	std::cout << std::endl;
	std::cout << "First, in the line 'myClass x;' , we build an object of class myClass.  This uses the default constructor, i.e. constructor with value 0." << std::endl;
	std::cout << "Next, createObjectOfMyClass(some args) needs to create an object of class myClass using 'some args' - this uses the constructor with value 42." << std::endl;
	std::cout << "After this, because of how we wrote our createObjectOfMyClass function, we end up copy constructing our return value from this object." << std::endl;
	std::cout << "Back to now: x = createObjectOfMyClass(some args) ...  well, x is already an object of class myClass, so this is the copy assignment operator." << std::endl;
	std::cout << "So in this relatively innocent few lines of code, we've been forced to create the same object many times." << std::endl;
	std::cout << "This is very wasteful, especially if our class contains a lot of data and is expensive to construct.  This is the main problem with not including move semantics!" << std::endl;

	std::cout << std::endl;
}


class myClass			// This class satisfies the rule of 5
{
public:
	int * myPtr;

	myClass(int x = 0)
	{
		myPtr = new int;
		*myPtr = x;
		std::cout << "Using the usual constructor with value " << x << "." << std::endl;
	}

	~myClass()
	{
		delete myPtr;
		myPtr = nullptr;
		std::cout << "Destroying object..." << std::endl;
	}

	myClass(const myClass& other)
	{
		myPtr = new int;
		*myPtr = *other.myPtr;
		std::cout << "Using the copy constructor." << std::endl;
	}

	myClass& operator=(const myClass& other)
	{
		if (this != &other) 
		{
			*myPtr = *other.myPtr;
			std::cout << "Using the copy assignment operator." << std::endl;
		}
	}

	myClass(myClass&& other)
	{
		myPtr = other.myPtr;
		other.myPtr = nullptr;
		std::cout << "Using the move constructor." << std::endl;
	}

	myClass& operator=(myClass&& other)
	{
		if (&other == this)
			return *this;

		delete myPtr;			// Delete any resources we're holding onto before transferring ownership from 'other' to 'this'.
		myPtr = other.myPtr;
		other.myPtr = nullptr;

		std::cout << "Using the move assignment operator." << std::endl;

		return *this;
	}

	void describeSelf()
	{
		std::cout << "My pointer is " << myPtr << " and the value stored there is " << *myPtr << "." << std::endl;
	}
};


myClass createObj(int x)
{
	myClass someObject = myClass(x);
	return someObject;
}

void showImprovementWithRuleOfFive()
{
	std::cout << std::endl << std::endl;
	std::cout << "We've now followed the rule of five, so this should be a bit more efficient than the last iteration." << std::endl;
	std::cout << "The difference between this iteration and the previous will become more noticable with classes which have a lot of dynamically allocated memory and pointers to objects which are expensive to build." << std::endl;
	
	myClass x;
	x = createObj(42);

	myClass y(createObj(30));
}



int main()
{
	showProblemWithShallowCopy();
	showImprovementWithCustomCopy();
	showProblemWithCustomCopy();
	showImprovementWithCustomCopyAssignment();
	showProblemWithNoMoveSemanticsFunction();

	showImprovementWithRuleOfFive();

    return 0;
}

