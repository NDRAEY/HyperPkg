import setuptools

setuptools.setup(
    name='hyperpkg',
    version='0.0.2',
    author="NDRAEY",
    description="HyperPkg utilities",
    scripts=["hyperpkg/hyperpkg-packer"],
    packages=setuptools.find_packages()
)
