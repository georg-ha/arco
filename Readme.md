# Arco

## Setup

- `cd build`
- `cmake ..`
- `make`
- `./arco [filename]`

## Language Overview

### Hello World

A program consits of statements and expressions, which are terminated by newlines or `;`. 
To extend expressions beyond one line, use `(` expr `)`

```
fun main() of int = {
    let name = "Georg"
    printf("Hello, %s!\n", name)
    0
}
```

printf accepts the format specifiers `%i`, `%d`, `%f`, `%s`, `%c`.

### Variables

Variables can be defined using `let` or `var`.
Type annotations are optional.
Supported types are `int`, `float`, `char`, `bool` and `string`

```

let x = 20
var y of float = 20.5 

y = 21.5 # Ok
x = 400 # Error

```

### Functions

Type annotations for parameters and the return type are required. The actual function body can be any expression.
To define bigger functions, use block expressions using `{}`.
No Return statement is needed to return a value

```
fun square(x of int) of int = x * x
```

Functions can be used before they are declared:

```
fun main() of int = {
    printf("%d\n", double(25))
    0
}

fun double(x of int) = x * 2
 
```
