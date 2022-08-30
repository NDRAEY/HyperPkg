#!/usr/bin/python3

import hyperpkg as pack
import argparse
import os

if __name__=="__main__":
    parser = argparse.ArgumentParser(
        prog='hyperpkg-pack',
        description='Pack executable and it\'s resources'
    )
    parser.add_argument('--executable', '-e', required=True)
    parser.add_argument('--resources', '-r', required=True)
    parser.add_argument('--name', '-n', required=True)
    parser.add_argument('--author', '-a', const="Unknown Author", nargs='?')
    args = parser.parse_args()

    print("Starting...")
    fo = open(f"{args.name}.hyperpkg", "wb")

    package = pack.CarolWriter(fo, args.name, args.author)
    package.make_hdr()

    print("Executable:", args.executable)
    package.write_something(package.MAIN, open(args.executable, "rb"))

    for a, b, c in os.walk("test"):
        for k in c:
            print("Packing resource:", a+"/"+k)
            package.write_something(package.RESOURCE, open(a+"/"+k, "rb"))
    package.finalize()
    
    fo.close()
