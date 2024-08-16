#!/bin/bash

# Run this from the src directory

filepath="/path/to/file.txt"
filename=$(basename "$filepath")

# Get the class names from each .hpp file.
# Assumes that the name of the header file is also the class name
# and that the class name is a top-level class that belongs in the
# kp namespace.
for file in $(find . -name "*.hpp"); do
    class_name=$(basename "$file" .hpp)
    echo "$class_name"

    # Replace anything where the class name is used as a template argument
    find . \( -name "*.cpp" -o -name "*.hpp" \) -exec sed -i s/\<"$class_name"/\<kp::$class_name/g {} +

    # Replace anything where a member of the class is accessed using ::
    find . \( -name "*.cpp" -o -name "*.hpp" \) -exec sed -i "s/\(^\|[^:]\)\($class_name::\)/\1kp::\2/g" {} +
done

# Special cases
find \( -name "*.cpp" -o -name "*.hpp" \) -exec sed -i 's/\(^\|[^:]\)\(TensorT<\)/\1kp::\2/g' {} +
find \( -name "*.cpp" -o -name "*.hpp" \) -exec sed -i 's/\(^\|[^:]\)\(TensorTypes::\)/\1kp::Tensor::\2/g' {} +
find \( -name "*.cpp" -o -name "*.hpp" \) -exec sed -i 's/\(^\|[^:]\)\(TensorDataTypes::\)/\1kp::Tensor::\2/g' {} +