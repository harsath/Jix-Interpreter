# References and Mutability

Mutable vs. immutable objects are handled similar to Python.

## Immutable objects:
When immutable objects (int, bool, string, function) is mutated or re-binded, it will not modify the originally referred object. Instead it will create a new object.
```
// Re-binding
let x = 10;
let y = x;
y = 20;
print(x); // 10
print(y); // 20
// Mutated
let z = x;
z = z + 1;
print(x); // 10
print(z); // 11
```
## Mutable objects:
When mutable objects (array) is mutated, it will alter the underlying object.
```
let x = [1, 2, 3];
let y = x;
y.add(4, 5);
print(x); // [1, 2, 3, 4, 5]
print(y); // [1, 2, 3, 4, 5]
```
