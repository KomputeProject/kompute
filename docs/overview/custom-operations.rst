
Extending Kompute with Custom C++ Operations
=================

Kompute provides an extenisble architecture which allows for the core components to be extended by building custom operations.

Building operations is intuitive however it requires knowing some nuances around the order in which each of the class functions across the operation are called as a sequence is executed.

These nuances are important for more advanced users of Kompute, as this will provide further intuition in what are the specific functions and components that the native functions (like OpTensorCreate, OpAlgoBase, etc) contain which define their specific behaviour.

Flow of Function Calls
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The top level operation which all operations inherit from is the :class:`kp::OpBase` class. Some of the "Core Native Operations" like :class:`kp::OpTensorCopy`, :class:`kp::OpTensorCreate`, etc all inherit from the base operation class.

The `kp::OpAlgoBase` is another base operation that is specifically built to enable users to create their own operations that contain custom shader logic (i.e. requiring Vulkan Compute Pipelines, DescriptorSets, etc). The next section contains an example which shows how to extend the OpAlgoBase class.

Below you 

.. list-table::
   :header-rows: 1

   * - Function
     - Description
   * - OpBase(..., tensors, freeTensors)
     - Constructor for class where you can load/define resources such as shaders, etc.
   * - ~OpBase()
     - Destructor that frees vulkan resources (if owned) which should be used to manage any memory allocations created through the operation.
   * - init()
     - Init function gets called in the Sequence / Manager inside the record step. This function allows for relevant objects to be initialised within the operation.
   * - record()
     - Record function that gets called in the Sequence / Manager inside the record step after init(). In this function you can directly record to the Vulkan command buffer.
   * - preEval()
     - When the Sequence is Evaluated this preEval is called across all operations before dispatching the batch of recorded commands to the GPU. This is useful for example if you need to copy data from local to host memory.
   * - postEval()
     - After the sequence is Evaluated this postEval is called across all operations. When running asynchronously the postEval is called when you call `evalAwait()`, which is why it's important to always run evalAwait() to ensure the process doesn't go into inconsistent state.


Simple Operation Extending OpAlgoBase
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Below we show a very simple example that enables you to create an operation with a pre-specified shader. In this case it is the multiplication shader.

.. code-block:: cpp
   :linenos:

   class OpMyCustom : public OpAlgoBase
   {
     public:
       OpMyCustom(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
              std::shared_ptr<vk::Device> device,
              std::shared_ptr<vk::CommandBuffer> commandBuffer,
              std::vector<std::shared_ptr<Tensor>> tensors)
         : OpAlgoBase(physicalDevice, device, commandBuffer, tensors, "")
       {
           // Perform your custom steps such as reading from a shader file
           this->mShaderFilePath = "shaders/glsl/opmult.comp";
       }
   }


   int main() {

       kp::Manager mgr; // Automatically selects Device 0

       // Create 3 tensors of default type float
       auto tensorLhs = std::make_shared<kp::Tensor>(kp::Tensor({ 0., 1., 2. }));
       auto tensorRhs = std::make_shared<kp::Tensor>(kp::Tensor({ 2., 4., 6. }));
       auto tensorOut = std::make_shared<kp::Tensor>(kp::Tensor({ 0., 0., 0. }));

       // Create tensors data explicitly in GPU with an operation
       mgr.evalOpDefault<kp::OpTensorCreate>({ tensorLhs, tensorRhs, tensorOut });

       // Run Kompute operation on the parameters provided with dispatch layout
       mgr.evalOpDefault<kp::OpMyCustom>(
           { tensorLhs, tensorRhs, tensorOut });

       // Prints the output which is { 0, 4, 12 }
       std::cout << fmt::format("Output: {}", tensorOutput.data()) << std::endl;
   }


More Complex Operation Extending OpAlgoBase
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Below we show a more complex operation that performs the following:

* Expects three tensors for an operation, two inputs and one output
* Expects the tensors to be initialised
* Checks that the tensors are of the same size
* Expects output tensor to be of type TensorTypes::eDevice (and creates staging tensor)
* Has functionality to read shader from file or directly from spirv bytes
* Records relevant bufferMemoryBarriers
* Records dispatch command
* Records copy command from device tensor to staging output tensor
* In postEval it maps data from staging tensor to output tensor's data


For starters, the header file contains the functions that will be overriden:


.. literalinclude:: ../../src/include/kompute/operations/OpAlgoLhsRhsOut.hpp
   :language: cpp


Then the implementation outlines all the implementations that perform the actions above:
~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../src/OpAlgoLhsRhsOut.cpp
   :language: cpp


