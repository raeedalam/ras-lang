# Functions

* Function is a block of code that is run when it is called
* Here's an example in javascript:

```javascript
function my_function(){
  console.log("Hi")
}
```

# Functions in RAS Lang
To do this in RAS Lang, look below
```
func my_function: print Hi

my_function
```

* `func` means that you want to create a function
* `my_function` is the name of you function
* `:` when you put that you are gonna put code after that
* `my_function` is calling that function and running the code in it
* You can also use variables in functions:

```
var name = Bob
func my_function: print $name
my_function
```

KABOOM, now you can do functions
