"""
CarolPkg(HyperPkg) packaging system by NDRAEY (c) 2021

It can create simple runnable packages with it's resources! (NOT ELF FILE FORMAT!)
"""
# pylint: disable=C0103

import struct
import os
import sys

class CarolWriter:
    """
    Carol's main writer class.
    """
    def __init__(self, fobj, packagename, author=None, version='0.0.1'):
        # Initializer function
        self.MAGIC = b'CAROL'
        self.VERSION = b'1'

        self.MAIN = b'\x01'
        self.DIRECTORY = b'\x02'
        self.RESOURCE = b'\x03'

        self.pkg = packagename
        self.author = author
        self.version = version
        
        self.author_spec = int(author is not None)
        self.flags = self.author_spec
        self.fobj = fobj

    def make_hdr(self):
        """
        Writes header of package.
        """
        self.fobj.write(self.MAGIC)
        self.fobj.write(self.VERSION)
        self.fobj.write(struct.pack("<H", self.flags))
        print("Flags written:", self.flags)
        self.fobj.write(struct.pack("<H", len(self.pkg)))
        self.fobj.write(self.pkg.encode("utf-8"))
        if self.author_spec:
            self.fobj.write(struct.pack("<H", len(self.author)))
            self.fobj.write(self.author.encode("utf-8"))
        self.fobj.write(struct.pack("<H", len(self.version)))
        self.fobj.write(self.version.encode("utf-8"))


    def write_something(self, typ, file):
        """
        Writes file to package
        """
        '''
        if os.path.isdir(file.name):
            print("Directory insertion not currently available, sorry...")
            sys.exit(1)
        '''
        if typ!=self.MAIN:
            trk = file.name.split("/")
            for index, _ in enumerate(trk):
                if index==0:
                    continue
                wdir = '/'.join(trk[:index])
                # print(file.name, ": Directory should be created:", wdir)
                self.fobj.write(self.DIRECTORY)
                self.fobj.write(struct.pack("<H", len(wdir.encode("utf-8"))))
                self.fobj.write(wdir.encode("utf-8"))
        file.seek(0, 2)
        fsize = file.tell()
        file.seek(0)

        self.fobj.write(struct.pack("<cI", typ, fsize))
        if typ==self.RESOURCE:
            self.fobj.write(struct.pack("<H", len(file.name)))
            self.fobj.write(file.name.encode("utf-8"))
        self.fobj.write(file.read())
        file.close()

    def finalize(self):
        """
        Finalizes the package (closes file stream)
        """
        self.fobj.truncate()
        self.fobj.close()

if __name__=="__main__":
    fo = open("Nya.hyperpkg", "wb")
    package = CarolWriter(fo, "First package", "NDRAEY")
    package.make_hdr()
    package.write_something(package.MAIN, open("hello","rb"))
    package.write_something(package.RESOURCE, open("tags/run/main.txt", "rb"))
    fo.close()
