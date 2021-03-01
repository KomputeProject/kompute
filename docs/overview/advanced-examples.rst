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


Asynchronous Operations
~~~~~~~~~~~~~~~~~~~~~~~

You can submit operations asynchronously with the async/await commands in the kp::Manager and kp::Sequence, which provides granularity on waiting on the vk::Fence. Back to `examples list <#simple-examples>`_

.. code-block:: cpp
   :linenos:

   int main() {

       // You can allow Kompute to create the Vulkan components, or pass your existing ones
       kp::Manager mgr; // Selects device 0 unless explicitly requested

       // Creates tensor an initializes GPU memory (below we show more granularity)
       auto tensor = mgr.tensor(10, 0.0);

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

       auto sq = mgr.sequence();

       sq.eval<kp::OpTensorSyncDevice>({tensor});

       sq.evalAsync<kp::OpAlgoDispatch>(mgr.algorithm({tensor}, spirv));

       // When we're ready we can wait 
       // The default wait time is UINT64_MAX
       sq.evalAwait(10000)

       // Sync the GPU memory back to the local tensor
       // We can still run synchronous jobs in our created sequence
       sq.eval<kp::OpTensorSyncLocal>({ tensor });

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

       // Creates tensor an initializes GPU memory (below we show more granularity)
       auto tensorA = mgr.tensor({ 10, 0.0 });
       auto tensorB = mgr.tensor({ 10, 0.0 });

       // Copies the data into GPU memory
       mgr.sequence().eval<kp::OpTensorSyncDevice>({tensorA tensorB});

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

       std::shared_ptr<kp::Algorithm> algo = mgr.algorithm({tensorA, tenssorB}, spirv);

       // We need to create explicit sequences with their respective queues
       // The second parameter is the index in the familyIndex array which is relative
       //      to the vector we created the manager with.
       sqOne = mgr.sequence(0);
       sqTwo = mgr.sequence(1);

       // Run the first parallel operation in the `queueOne` sequence
       sqOne->evalAsync<kp::OpAlgoDispatch>(algo);

       // Run the second parallel operation in the `queueTwo` sequence
       sqTwo->evalAsync<kp::OpAlgoDispatch>(algo);

       // Here we can do other work

       // We can now wait for the two parallel tasks to finish
       sqOne.evalOpAwait()
       sqTwo.evalOpAwait()

       // Sync the GPU memory back to the local tensor
       mgr.sequence()->eval<kp::OpTensorSyncLocal>({ tensorA, tensorB });

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

   class OpMyCustom : public OpAlgoDispatch
   {
     public:
       OpMyCustom(std::vector<std::shared_ptr<Tensor>> tensors,
            std::shared_ptr<kp::Algorithm> algorithm)
         : OpAlgoBase(algorithm)
       {
            if (tensors.size() != 3) {
                throw std::runtime_error("Kompute OpMult expected 3 tensors but got " + tensors.size());
            }

            std::vector<uint32_t> spirv = kp::Shader::compile_source(R"(
                #version 450

                layout(set = 0, binding = 0) buffer tensorLhs {
                   float valuesLhs[ ];
                };

                layout(set = 0, binding = 1) buffer tensorRhs {
                   float valuesRhs[ ];
                };

                layout(set = 0, binding = 2) buffer tensorOutput {
                   float valuesOutput[ ];
                };

                layout (constant_id = 0) const uint LEN_LHS = 0;
                layout (constant_id = 1) const uint LEN_RHS = 0;
                layout (constant_id = 2) const uint LEN_OUT = 0;

                layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

                void main() 
                {
                    uint index = gl_GlobalInvocationID.x;

                    valuesOutput[index] = valuesLhs[index] * valuesRhs[index];
                }
            )");

            algorithm->rebuild(tensors, spirv);
       }
   }


   int main() {

       kp::Manager mgr; // Automatically selects Device 0

       // Create 3 tensors of default type float
       auto tensorLhs = mgr.tensor({ 0., 1., 2. });
       auto tensorRhs = mgr.tensor({ 2., 4., 6. });
       auto tensorOut = mgr.tensor({ 0., 0., 0. });

       mgr.sequence()
            ->record<kp::OpTensorSyncDevice>({tensorLhs, tensorRhs, tensorOut})
            ->record<kp::OpMyCustom>({tensorLhs, tensorRhs, tensorOut}, mgr.algorithm())
            ->record<kp::OpTensorSyncLocal>({tensorLhs, tensorRhs, tensorOut})
            ->eval();

       // Prints the output which is { 0, 4, 12 }
       std::cout << fmt::format("Output: {}", tensorOutput.data()) << std::endl;
   }


