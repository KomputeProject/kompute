# Python Bindings for Vulkan Kompute

## Publishing to pypi

Build source distribution:

```
python setup.py sdist bdist_wheel
```

Push to test pypi registry:

```
python -m twine upload --repository testpypi dist/*
```

Install from test pypi:

```
python -m pip install --index-url https://test.pypi.org/simple/ --no-deps kp
```

Run tests in python/test directory:

```
python -m pytest
```

Push to official pypi registry:

```
python -m twine upload dist/*
```

