![HyperPkg](res/hyperpkg.png)

# About Project
HyperPkg (CarolPkg) - is a packaging system that allows run programs that needs resources from packages (in short, an archive with a program and resources)

# Installation
## Linux

Install compiler, GNU Make and Python 3: ```apt update && apt install gcc make python3```

Clone this repository: ```git clone https://github.com/NDRAEY/HyperPkg```

Enter: ```cd HyperPkg```

Compile runtime: ```cd runtime && make && make install && cd ..```

Install utilities: ```cd packer && python3 setup.py install && cd ..```

Now, you can create and run HyperPkg packages!
