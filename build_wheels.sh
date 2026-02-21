#!/bin/bash
set -e -u -x

function repair_wheel {
    wheel="$1"
    if ! auditwheel show "$wheel"; then
        echo "Skipping non-platform wheel $wheel"
    else
        auditwheel repair "$wheel" --plat "$PLAT" -w /io/dist/
    fi
}

# System package required for library
yum install -y vulkan
yum install -y vulkan-devel

# Create folder
mkdir -p /io/dist_tmp/
mkdir -p /io/dist/


# Compile wheels
for PYBIN in /opt/python/*/bin; do
    "${PYBIN}/pip" wheel --verbose /io/ --no-deps -w /io/dist_tmp/
done

# Bundle external shared libraries into the wheels
for whl in /io/dist_tmp/*.whl; do
    repair_wheel "$whl" && echo "Continuing"
done

# Install packages and test
for PYBIN in /opt/python/*/bin/; do
    "${PYBIN}/pip" install -r /io/python/test/requirements-dev.txt
    "${PYBIN}/pip" install kp -f /io/dist
    make -C /io/ test_python PYTHON_BIN="${PYBIN}/python"
done

