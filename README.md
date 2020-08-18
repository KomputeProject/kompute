# Vulkan Kompute

## Principles

* Non-vulkan naming convention to disambiguate Vulkan vs Kompute components
* Extends the existing vulkan API with a simpler compute-specific interface
* BYOV: Play nice with existing Vulkan applications with a bring-your-own-Vulkan design
* TODO

## Getting Started

Use default equations

```c++
int main() {
    kp::Manager kManager(); // Chooses device 0 

    kp::Tensor inputOne = kp::Tensor({0, 1, 2, 3});

    kp::Tensor inputTwo;
    inputTwo = kManager.eval<kp::OpCreateTensor>(&inputTwo);

    kp::Tensor output = kManager.eval<kp::OpMult>(inputOne, inputTwo);

    std::cout << output << std::endl;
}
```

```c++
int main() {
    kp::Manager kManager(); // Chooses device 0 

    kp::Tensor inputOne = kManager.eval<kp::OpCreateTensor>({0, 1, 2, 3}); // Mounts to device and binds to 0
    kp::Tensor inputTwo = kManager.eval<kp::OpCreateTensor>({0, 1, 2, 3}); // Mounts to device and binds to 1


    kp::Tensor inputOne({0, 1, 2, 3}); 
    kManager.eval<kp::OpCreateTensor>(&inputOne); // Mounts to device and binds to 0

    kp::Tensor inputOne({0, 1, 2, 3}); 
    kManager.eval<kp::OpCreateTensor>(&inputTwo); // Mounts to device and binds to 0

    kp::Tensor output = kManager.eval<kp::OpMult>(inputOne, inputTwo);

    std::cout << output << std::endl;
}
```

Create your own operation

```c++
class CustomOp : kp::BaseOperator {
    CusomOp() {
        this->mAlgorithm = kp::Algorithm("path/to/your/shader.compute.spv")
    }

    kp::Tensor init(kp::Tensor* rhs, kp::Tensor* lhs, kp::Tensor* result) override {
        this->appendParameter(kp::Parameter(rhs)); // Binding 0
        this->appendParameter(kp::Parameter(lhs)); // Binding 1
        this->appendParameter(kp::Parameter(result)); // Binding 2
    }
}

int main() {
    kp::Manager kManager(); // Chooses device 0 

    kp::Tensor inputOne({0, 1, 2, 3}); 

    kp::Tensor inputTwo({0, 1, 2, 3});

    kp::Tensor output;
    kManager.eval<kp::CustomOp>(&inputOne, &inputTwo, &output);

    std::cout << output << std::endl;
}
```

Use equations to group operations on memory and execution step

```c++
int main() {
    kp::Manager kManager(); // Chooses device 0 

    kp::Sequence sq;
    kManager.createSequence(&sq);

    sq.begin();

    kp::Tensor inputOne; 
    sq.record<kp::OpCreateTensor>(&inputOne, {0, 1, 2, 3}); // Mounts to device and binds to 0

    kp::Tensor inputTwo;
    sq.record<kp::OpCreateTensor>(&inputTwo, {0, 1, 2, 3}); // Mounts to device and binds to 1

    kp::Tensor output;
    sq.record<kp::OpMult>(&inputOne, &inputTwo, &output);

    sq.end();
    sq.eval();

    std::cout << output << std::endl;
}
```


## Development

Follows Mozilla C++ Style Guide https://www-archive.mozilla.org/hacking/mozilla-style-guide.html
