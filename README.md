# Expression Tree Docs

# API
We want this following API:

```c++
et::var x(10), y(20), z(5);

std::vector<et::var> args {x,y,z};

et::eval(x); // returns a number
(x + y); // returns a var
(y + z * 5); // returns a var
final = (3 + x^2 - 1); // returns a var

et::eval(final); // returns a number
auto dx = et::back(final, x); // returns the gradient with respect to x, is a et::var
auto grad = et::back(final, args); // returns a std::map<etc::var, etc::var> m{ {x, dx}, {y, dy}, {z, dz} };
```

## `et::var`

Each `et::var` is a lightweight struct containing the following:

```c++
struct var{
    ...
    int val;
    operator op;
    vector<var*> children; // children
}
```

If we evaluate the expression `y + z * 5`, we will get a tree like:

```
    (+)
    / \
  (*) (y)
  / \
(z) (5)
```

when we apply, the plus function overloaded, we will retrieve the following:

```c++
et::var x(10);
et::var y = x + 10;
et::var z(0); 

eval(z); // outputs 0. 

y.children.size(); // outputs 1. 

z = x + y; // z gets assignment operator'd! 

z.children.size(); // outputs 2. We will not implement path compression.
eval(z); // z's original value is overwritte; outputs 30

y.val; // outputs 20. It's already evaluated by eval(z)!
```

## `et::eval()`

`et::eval()` works by first performing a search for the leaves, and then performing a topological sort.

## `et::back()`

`et::back()` works by setting the terminal node as having derivative 1. At every single operation, it will perform the proper derivative expression. It goes backwards, so it's really a reversed topological sort, with the initial leaf being a single node, the evaluated node.

# Optimizations

## `const`-ness Induced Restricted BFS

We know that some variables are constant so that no gradient flow returns from it. 
The variables that are constant are usually rvalues, aka directly defined and resides in registers.

However, we can't directly restrict rvalues, or else gradient may not flow back at all.

Therefore, we will define a specific operator overload on variables, so that we can restrict BFS:

```c++
var x(10);

// The operation below creates 2 rvalue variables:
// 1. 10 becomes var(10).
//    This can be const-qualified, since we don't care about its value.
// 2. (x+10) becomes var(_, {x, var(10)}). 
//    This needs to be kept because x is an lvalue.
// 
// And creates 2 lvalue variables:
// 1. x is previously defined.
// 2. (x+10)/2 is the result, and is assigned z. 

var z = (x + 10)/2;
```

In the above example, we should not calculate the derivative of `var(10)`, 
but we should calculate `var(x+10)`.

The heuristic is thus:

1. Any rvalue `var` input into an `f()` operation will be considered `const`-qualified.
2. as long as `f()` is an operation on purely `const`-qualified inputs, 
it will output a `const`-qualified variable.

