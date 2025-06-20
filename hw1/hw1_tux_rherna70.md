# hw1 - Readiness Exercise
## Your name (UID)

### 1.
Explain why the output of the following code snippet is 0.
``` 
unsigned int i = 0;
printf("%u", i--);
```


[your answer]

Because we use a postfix decrement operator here so the value is printed first before the decrement.

### 2.

Explain why the output of the following code snippet is "char=1, int=4, long=8" in x86 (64-bit).

```
printf("char=%d, int=%d, long=%d", \
  sizeof(char), sizeof(int), sizeof(long));
```

[your answer]

Data type sizes are determined by the architecture, which are fixed in hardware.  In x86-64 we have:
1 byte => 8 bits
word=2 bytes
int=4 bytes
long=8 bytes

### 3.
Explain why the output of the following code snippet is "st0 = 8, st1 = 8".

```
struct st0 {
  int x;
  char y;
};
struct st1 {
  int x;
  char y;
  char z;
};
int main()
{
  printf("st0 = %d, st1 = %d\n",
  sizeof(struct st0), sizeof(struct st1));
}
```

[your answer]

```
robertdavidhernandez@MacBook-Pro:~/code/cs_594 (main) > gcc hello.c
hello.c:17:5: warning: format specifies type 'int' but the argument has type 'unsigned long' [-Wformat]
    sizeof(struct st0), sizeof(struct st1));
    ^~~~~~~~~~~~~~~~~~
hello.c:17:25: warning: format specifies type 'int' but the argument has type 'unsigned long' [-Wformat]
    sizeof(struct st0), sizeof(struct st1));
                        ^~~~~~~~~~~~~~~~~~
2 warnings generated.
```

It would seem to be based on the compiler output that the reason the sizes of the structs are the same is because the underlying data structures are "unsigned long", it would seem that one of (compiler, hardware, OS kernel) assign an unsigned long as the minimun size for a struct.

### 4.

Explain why the output of the following code snippet is 0xEF.

```
int64_t v = 0xdeadbeef;
printf("%02x", ((char *)&v)[0]);
```

[your answer]


### 5.
 Explain why the output of the following code snippet is "i=5, j=10".
```
int main ()
{
  int i, j, *p, *q;
  p = &i;
  q = &j;
  *p = 5;
  *q = *p + i;
  printf("i = %d, j = %d\n", i, j);
  return 0;
}
````

[your answer]


We declare four variables: i and j are integers, and p and q are pointers to integers.

Then the addresses of variables i and j are assigned to pointers p and q respectively.

Then we assign value 5 to the variable i through the pointer p.

The value of i (5) is added to the value pointed to by p (also 5 p points to i). The resultant value (10) is then assigned to the variable j through the pointer q.


Thus the output will be "i=5, j=10" 


### 6.

Explain why the value of NULL (64-bit) is 0x0000000000000000.

[your answer]

NULL being 0x0000000000000000 is a convenient representation of a null pointer, and it follows the convention of representing zero in hexadecimal notation.


### 7.

Explain why the output of the following code is 0x124000.
#define PGSIZE 4096
#define CONVERT(sz) (((sz)+PGSIZE-1) & ~(PGSIZE-1))
printf("0x%x", CONVERT(0x123456));

[your answer]

First we define the page size as 4096

Then we define a "CONVERT" macro that uses an arguement "sz" and does:
```
(((0x123456)+4096-1) & ~(4096-1))
```


which in english is:

(0x123456) + 4096 - 1 evaluates to 0x124551.

This step ensures that if sz is not already a multiple of PGSIZE, it is increased to the next multiple of PGSIZE.

```
& ~(4096-1) 
```
performs a bitwise AND with the bitwise negation of `(4096-1)`. This step ensures that the result is rounded down to the nearest multiple of PGSIZE.

The final result is 0x124000, which is the rounded-up multiple of PGSIZE for the input 0x123456.


So, the printf statement prints "0x124000". This type of operation is used in memory management, where it ensures that memory allocations are aligned to the page size.


### 8.

 Assuming the first printf results "1 = 0x7fffdfbf7f00", explain why the rest of the output is as follows:

2 = 0x7fffdfbf7f04

3 = 0x7fffdfbf7f00

4 = 0x7fffdfbf7f14

```
main() {
  int x[5];
  printf("1 = %p\n", x);
  printf("2 = %p\n", x+1);
  printf("3 = %p\n", &x);
  printf("4 = %p\n", &x+1);
  return 0;
}
```

[your answer]

The first print is given to be "1 = 0x7fffdfbf7f00" which from the print statement is the address of x

The second print prints the address of the next element in the array (index 1). This is because x+1 points to the memory location immediately after the end of the first element.  "2 = 0x7fffdfbf7f04": This is the address of the second element in the array (x+1), which is 4 bytes (size of an integer) after the address of the first element.

The third print prints the address of the array x which is the same as the address of the first element "3 = 0x7fffdfbf7f00": This is the address of the entire array, same as the address of its first element.

The fourth print prints the address immediately after the end of the array "4 = 0x7fffdfbf7f14": This is the address immediately after the end of the entire array (&x+1), which is 20 bytes (size of the entire array 5 * sizeof(int)) after the address of the first element.

### 9.
![](hw1_tux_1234567.png)
![image](git_ubuntu.jpg)