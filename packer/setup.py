import setuptools

setuptools.setup(
    name='hyperpkg',
    version='0.0.1',
    author="NDRAEY",
    description="HyperPkg utilities",
    scripts=["packer/hyperpkg-packer"],
    packages=setuptools.find_packages()
)