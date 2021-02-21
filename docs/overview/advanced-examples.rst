.. role:: raw-html-m2r(raw)
   :format: html


C++ Examples
=================

The power of Kompute comes in when the interface is used for complex computations. This section contains an outline of the advanced / end-to-end examples available.

Simple examples
^^^^^^^^^^^^^^^


* `Pass shader as raw string <#simple-shader-example>`_
* `Record batch commands with a Kompute Sequence <#record-batch-commands>`_
* `Run Asynchronous Operations <#asynchronous-operations>`_
* `Run Parallel Operations Across Multiple GPU Queues <#parallel-operations>`_
* `Create your custom Kompute Operations <#your-custom-kompute-operation>`_
* `Implementing logistic regression from scratch <#logistic-regression-example>`_

End-to-end examples
^^^^^^^^^^^^^^^^^^^


* `Machine Learning Logistic Regression Implementation <https://towardsdatascience.com/machine-learning-and-data-processing-in-the-gpu-with-vulkan-kompute-c9350e5e5d3a>`_
* `Parallelizing GPU-intensive Workloads via Multi-Queue Operations <https://towardsdatascience.com/parallelizing-heavy-gpu-workloads-via-multi-queue-operations-50a38b15a1dc>`_
* `Android NDK Mobile Kompute ML Application <https://towardsdatascience.com/gpu-accelerated-machine-learning-in-your-mobile-applications-using-the-android-ndk-vulkan-kompute-1e9da37b7617>`_
* `Game Development Kompute ML in Godot Engine <https://towardsdatascience.com/supercharging-game-development-with-gpu-accelerated-ml-using-vulkan-kompute-the-godot-game-engine-4e75a84ea9f0>`_


Simple Shader Example
~~~~~~~~~~~~~~~~~~~~~

Pass compute shader data in glsl/hlsl text or compiled SPIR-V format (or as path to the file). Back to `examples list <#simple-examples>`_.

.. code-block:: cpp
   :linenos:
    int main() {

        // You can allow Kompute to create the Vulkan components, or pass your existing ones
        kp::Manager mgr; // Selects device 0 unless explicitly requested

        // Creates tensor an initializes GPU memory (below we show more granularity)
        auto tensorA = std::make_shared<kp::Tensor>(kp::Tensor({ 3., 4., 5. }));
        auto tensorB = std::make_shared<kp::Tensor>(kp::Tensor({ 0., 0., 0. }));

        // Create tensors data explicitly in GPU with an operation
        mgr.rebuild({ tensorA, tensorB });

        // Define your shader as a string (using string literals for simplicity)
        // (You can also pass the raw compiled bytes, or even path to file)
        std::string shader(R"(
            #version 450

            layout (local_size_x = 1) in;

            layout(set = 0, binding = 0) buffer a { float pa[]; };
            layout(set = 0, binding = 1) buffer b { float pb[]; };

            void main() {
                uint index = gl_GlobalInvocationID.x;
                pb[index] = pa[index];
                pa[index] = index;
            }
        )");

        // Run Kompute operation on the parameters provided with dispatch layout
        mgr.evalOpDefault<kp::OpAlgoBase>(
            { tensorA, tensorB }, 
            kp::Shader::compile_source(shader));

        // Sync the GPU memory back to the local tensor
        mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorA, tensorB });

        // Prints the output which is A: { 0, 1, 2 } B: { 3, 4, 5 }
        std::cout << fmt::format("A: {}, B: {}", 
            tensorA.data(), tensorB.data()) << std::endl;
    }

Record batch commands
~~~~~~~~~~~~~~~~~~~~~

Record commands in a single submit by using a Sequence to send in batch to GPU. Back to `examples list <#simple-examples>`_

.. code-block:: cpp
   :linenos:

   int main() {

       kp::Manager mgr;

       std::shared_ptr<kp::Tensor> tensorLHS{ new kp::Tensor({ 1., 1., 1. }) };
       std::shared_ptr<kp::Tensor> tensorRHS{ new kp::Tensor({ 2., 2., 2. }) };
       std::shared_ptr<kp::Tensor> tensorOutput{ new kp::Tensor({ 0., 0., 0. }) };

       // Create all the tensors in memory
       mgr.evalOpDefault<kp::OpCreateTensor>({tensorLHS, tensorRHS, tensorOutput});

       // Create a new sequence
       std::weak_ptr<kp::Sequence> sqWeakPtr = mgr.sequence();

       if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr.lock())
       {
           // Begin recording commands
           sq->begin();

           // Record batch commands to send to GPU
           sq->record<kp::OpMult>({ tensorLHS, tensorRHS, tensorOutput });
           sq->record<kp::OpTensorCopy>({tensorOutput, tensorLHS, tensorRHS});

           // Stop recording
           sq->end();

           // Submit multiple batch operations to GPU
           size_t ITERATIONS = 5;
           for (size_t i = 0; i < ITERATIONS; i++) {
               sq->eval();
           }

           // Sync GPU memory back to local tensor
           sq->begin();
           sq->record<kp::OpTensorSyncLocal>({tensorOutput});
           sq->end();
           sq->eval();
       }

       // Print the output which iterates through OpMult 5 times
       // in this case the output is {32, 32 , 32}
       std::cout << fmt::format("Output: {}", tensorOutput.data()) << std::endl;
   }

Asynchronous Operations
~~~~~~~~~~~~~~~~~~~~~~~

You can submit operations asynchronously with the async/await commands in the kp::Manager and kp::Sequence, which provides granularity on waiting on the vk::Fence. Back to `examples list <#simple-examples>`_

.. code-block:: cpp
   :linenos:

   int main() {

       // You can allow Kompute to create the Vulkan components, or pass your existing ones
       kp::Manager mgr; // Selects device 0 unless explicitly requested

       // Creates tensor an initializes GPU memory (below we show more granularity)
       auto tensor = std::make_shared<kp::Tensor>(kp::Tensor(std::vector<float>(10, 0.0)));

       // Create tensors data explicitly in GPU with an operation
       mgr.rebuild(tensor)

       // Define your shader as a string (using string literals for simplicity)
       // (You can also pass the raw compiled bytes, or even path to file)
       std::string shader(R"(
           #version 450

           layout (local_size_x = 1) in;

           layout(set = 0, binding = 0) buffer b { float pb[]; };

           shared uint sharedTotal[1];

           void main() {
               uint index = gl_GlobalInvocationID.x;

               sharedTotal[0] = 0;

               // Iterating to simulate longer process
               for (int i = 0; i < 100000000; i++)
               {
                   atomicAdd(sharedTotal[0], 1);
               }

               pb[index] = sharedTotal[0];
           }
       )");

       std::vector<uint32_t> spirv = kp::Shader::compile_source(shader);

       // We can now await for the previous submitted command
       // The first parameter can be the amount of time to wait
       // The time provided is in nanoseconds
       mgr.evalOpAwaitDefault(10000);

       // Run Async Kompute operation on the parameters provided
       mgr.evalOpAsyncDefault<kp::OpAlgoBase>(
           { tensor }, 
           spirv);

       // Here we can do other work

       // When we're ready we can wait 
       // The default wait time is UINT64_MAX
       mgr.evalOpAwaitDefault()

       // Sync the GPU memory back to the local tensor
       // We can still run synchronous jobs in our created sequence
       mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensor });

       // Prints the output: B: { 100000000, ... }
       std::cout << fmt::format("B: {}", 
           tensor.data()) << std::endl;
   }

Parallel Operations
~~~~~~~~~~~~~~~~~~~

Besides being able to submit asynchronous operations, you can also leverage the underlying GPU compute queues to process operations in parallel.

This will depend on your underlying graphics card, but for example in NVIDIA graphics cards the operations submitted across queues in one family are not parallelizable, but operations submitted across queueFamilies can be parallelizable.

Below we show how you can parallelize operations in an `NVIDIA 1650 <http://vulkan.gpuinfo.org/displayreport.php?id=9700#queuefamilies>`_\ , which has a ``GRAPHICS+COMPUTE`` family on ``index 0``\ , and ``COMPUTE`` family on ``index 2``.

Back to `examples list <#simple-examples>`_.

.. code-block:: cpp
   :linenos:

   int main() {

       // In this case we select device 0, and for queues, one queue from familyIndex 0
       // and one queue from familyIndex 2
       uint32_t deviceIndex(0);
       std::vector<uint32_t> familyIndices = {0, 2};

       // We create a manager with device index, and queues by queue family index
       kp::Manager mgr(deviceIndex, familyIndices);

       // We need to create explicit sequences with their respective queues
       // The second parameter is the index in the familyIndex array which is relative
       //      to the vector we created the manager with.
       mgr.sequence("queueOne", 0);
       mgr.sequence("queueTwo", 1);

       // Creates tensor an initializes GPU memory (below we show more granularity)
       auto tensorA = std::make_shared<kp::Tensor>(kp::Tensor(std::vector<float>(10, 0.0)));
       auto tensorB = std::make_shared<kp::Tensor>(kp::Tensor(std::vector<float>(10, 0.0)));

       // We run the first step synchronously on the default sequence
       mgr.rebuild({ tensorA, tensorB });

       // Define your shader as a string (using string literals for simplicity)
       // (You can also pass the raw compiled bytes, or even path to file)
       std::string shader(R"(
           #version 450

           layout (local_size_x = 1) in;

           layout(set = 0, binding = 0) buffer b { float pb[]; };

           shared uint sharedTotal[1];

           void main() {
               uint index = gl_GlobalInvocationID.x;

               sharedTotal[0] = 0;

               // Iterating to simulate longer process
               for (int i = 0; i < 100000000; i++)
               {
                   atomicAdd(sharedTotal[0], 1);
               }

               pb[index] = sharedTotal[0];
           }
       )");

       std::vector<uint32_t> spirv = kp::Shader::compile_source(shader);

       // Run the first parallel operation in the `queueOne` sequence
       mgr.evalOpAsync<kp::OpAlgoBase>(
           { tensorA }, 
           "queueOne",
           spirv);

       // Run the second parallel operation in the `queueTwo` sequence
       mgr.evalOpAsync<kp::OpAlgoBase>(
           { tensorB }, 
           "queueTwo",
           spirv);

       // Here we can do other work

       // We can now wait for the two parallel tasks to finish
       mgr.evalOpAwait("queueOne")
       mgr.evalOpAwait("queueTwo")

       // Sync the GPU memory back to the local tensor
       mgr.evalOp<kp::OpTensorSyncLocal>({ tensorA, tensorB });

       // Prints the output: A: 100000000 B: 100000000
       std::cout << fmt::format("A: {}, B: {}", 
           tensorA.data()[0], tensorB.data()[0]) << std::endl;
   }

Your Custom Kompute Operation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Build your own pre-compiled operations for domain specific workflows. Back to `examples list <#simple-examples>`_

We also provide tools that allow you to `convert shaders into C++ headers <https://github.com/EthicalML/vulkan-kompute/blob/master/scripts/convert_shaders.py#L40>`_.

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
           this->mShaderFilePath = "shaders/glsl/opmult.comp.spv";
       }
   }


   int main() {

       kp::Manager mgr; // Automatically selects Device 0

       // Create 3 tensors of default type float
       auto tensorLhs = std::make_shared<kp::Tensor>(kp::Tensor({ 0., 1., 2. }));
       auto tensorRhs = std::make_shared<kp::Tensor>(kp::Tensor({ 2., 4., 6. }));
       auto tensorOut = std::make_shared<kp::Tensor>(kp::Tensor({ 0., 0., 0. }));

       // Create tensors data explicitly in GPU with an operation
       mgr.rebuild({ tensorLhs, tensorRhs, tensorOut });

       // Run Kompute operation on the parameters provided with dispatch layout
       mgr.evalOpDefault<kp::OpMyCustom<3, 1, 1>>(
           { tensorLhs, tensorRhs, tensorOut });

       // Prints the output which is { 0, 4, 12 }
       std::cout << fmt::format("Output: {}", tensorOutput.data()) << std::endl;
   }


