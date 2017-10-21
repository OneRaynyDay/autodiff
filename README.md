# Autodiff Docs

# API
We want this following API:

```c++
et::var x(10), y(20), z(5);

std::unordered_map<et::var, double> args {
    {x,0},
    {y,0},
    {z,0}
};

et::eval(x); // returns a number
(x + y); // returns a var
(y + z * 5); // returns a var
final = (3 + x^2 - 1); // returns a var

et::eval(final); // returns a number
et::back(final, args); // fills the std::map<etc::var, etc::var> m{ {x, dx}, {y, dy}, {z, dz} };
```

## `et::var`

Each `et::var` is a lightweight struct containing the following(pretty much):

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

y.getChildren().size(); // outputs 1. 

z = x + y; // z gets assignment operator'd! 

z.getChildren().size(); // outputs 2. We will not implement path compression.
eval(z); // z's original value is overwritte; outputs 30

y.getValue(); // outputs 20. It's already evaluated by eval(z)!
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

This should be handled by the `et::expression` class, which will map the variables to the const-ness.

By default the variables are not const. 
We'll add to an unordered_set of variables that are non-const via bottom-up tree search.

Here's an example of using the optimized version:

```c++
et::var x(10), y(20), z(5);

std::unordered_map<et::var, double> args {
    {x,0},
    {y,0},
    {z,0}
};

et::eval(x); // returns a number
(x + y); // returns a var
(y + z * 5); // returns a var
final = (3 + x^2 - 1); // returns a var

et::eval(final); // returns a number
et::back(final, args, {et::back_flags::const_qualify}); // fills the std::map<etc::var, etc::var> m{ {x, dx}, {y, dy}, {z, dz} };
```
