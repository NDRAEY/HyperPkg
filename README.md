![HyperPkg](res/hyperpkg.png)

# About Project
HyperPkg (CarolPkg) - is a packaging system that allows run programs that needs resources from packages (in short, an archive with a program and resources)

# Installation
## Linux

Install compiler, GNU Make and Python 3:

```
apt update && apt install gcc make python3
```

Clone this repository:

```
git clone https://github.com/NDRAEY/HyperPkg
```

Enter:

```
cd HyperPkg
```

Compile the runtime:

```
cd runtime && make && make install && cd ..
```

Install the utilities:

```
cd packer && python3 setup.py install && cd ..
```

Now, you can create and run HyperPkg packages!

# How to Use (Package Creation Example)

For example, let's create the 'my-hyper-package' folder in the home directory:

```
cd ~ && mkdir my-hyper-package && cd my-hyper-package
```

Create the folder for resources:

```
mkdir res
```

Create the program.c file with contents:

```C
#include <stdio.h>

int main() {
	FILE* fp = fopen("res/hi.txt", "r");
	if(!fp) {
		printf("fopen() failed!\n");
		exit(1);
	}

	char* buf = malloc(128);
	if(!buf) {
		printf("malloc() failed!\n");
		exit(1);
	}
	fread(buf, sizeof(char), 128, fp);

	printf("%s\n", buf);
	free(buf);
	
	return 0;
}
```

Compile it: 

```
gcc program.c -o program
```

And create package:

```
hyperpkg-packer -e program -n "My First Package" -r res/ -a "Your Name"
```

- `-e` flag - path to main executable
- `-n` flag - package name
- `-r` flag - path to folder that contains resources
- `-a` flag - author name

Now, you will get new file named: ```My First Package.hyperpkg```

And now, you can run it:

```
hyperpkg-run ./My\ First\ Package.hyperpkg
```

Enjoy!
