# Flexible Efficient Language
FELan is an Object-Oriented Programming Language made by Ahmad Mahmoudi. This language is focused on Flexibility of its compiler to add more features easily and fast. like to add a new operator to this language you only have to add about 10 lines of code.

**This language only can be run under unix systems.**

## Sample code
```
from felan.lang.system          import *
from felan.lang.primitives      import *

fun main(){
    var variable:Int = Int.__constructor__(3);
    println(variable + 7);
}
```