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


