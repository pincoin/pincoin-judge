# Python3
* compile: `python3 -c "import py_compile; py_compile.compile(r'Main.py')"`
* run: `python3 Main.py`
* version: python 3.6.8
* time constraint: x3+2 sec
* memory constraint: x2+32MB

```python
a, b = map(int, input().split())
print(a+b)
```

```
apt-get install python3
```

# C11
* compile: `gcc Main.c -o Mainc.out -O2 -Wall -lm -static -std=c11`
* run: `./Mainc.out`
* version: gcc 7.4.0

```c
#include <stdio.h>
int main() {
    int a, b;
    scanf("%d %d",&a,&b);
    printf("%d\n",a+b);
    return 0;
}
```

```
apt-get install build-essential
```

# C++17
* compile: `g++ Main.cc -o Maincpp.out -O2 -Wall -lm -static -std=gnu++17`
* run: `./Maincpp.out`
* version: g++ 7.4.0

```cpp
#include <iostream>
using namespace std;
int main() {
    auto a=0, b=0;
    cin >> a >> b;
    cout << a+b << endl;
    return 0;
}
```

```
apt-get install build-essential
```

# Java11 OpenJDK
* compile: `javac -J-Xms1024m -J-Xmx1024m -J-Xss512m -encoding UTF-8 Main.java`
* run: `java -Xms1024m -Xmx1024m -Xss512m -Dfile.encoding=UTF-8 -Djava.security.manager -Djava.security.policy=java.policy Main`
* version: openjdk 11.0.3
* time constraint: x2+1 sec
* memory constraint: x2+16MB

```java
import java.util.*;
public class Main{
    public static void main(String args[]){
        Scanner sc = new Scanner(System.in);
        int a, b;
        a = sc.nextInt();
        b = sc.nextInt();
        System.out.println(a + b);
    }
}
```

```
apt-get install openjdk-11-jdk
```

# C#
* compile: `mcs -codepage:utf8 -warn:0 -optimize+ -checked+ -clscheck- -reference:System.Numerics.dll -out:Main.exe Main.cs`
* run: `mono --optimize=all Main.exe`
* version: mcs 4.6.2.0
* time constraint: +5 sec
* memory constraint: +512MB

```c#
using System;

public class Program {
    public static void Main() {
        string s = Console.ReadLine();
        string[] ss = s.Split();
        int a = int.Parse(ss[0]);
        int b = int.Parse(ss[1]);
        Console.WriteLine(a+b);
    }
}
```

```
apt-get install mono-mcs libmono-system-numerics4.0-cil
```

# Node.js
* compile: LINT check!
* run: `node Main.js`
* version: v8.10.0
* time constraint: x3+2 sec
* memory constraint: x2MB

```javascript
var fs = require('fs');
var input = fs.readFileSync('/dev/stdin').toString().split(' ');
var a = parseInt(input[0]);
var b = parseInt(input[1]);
console.log(a+b);
```

```
apt-get install nodejs npm
npm init
npm install file-system --save
```

# PHP
* compile: `php -l Main.php`
* run: `php Main.php`
* version: 7.2.19
* time constraint: +5 sec
* memory constraint: +512MB

```php
<?php
fscanf(STDIN,"%d %d",$a,$b);
fprintf(STDOUT,"%d",$a+$b);

```

```
apt-get install php-cli
```
