.. role:: raw-html-m2r(raw)
   :format: html


Examples
========

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
* `Parallelizing GPU-intensive Workloads via Multi-Queue Operations [https://towardsdatascience.com/parallelizing-heavy-gpu-workloads-via-multi-queue-operations-50a38b15a1dc>`_
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
        mgr.evalOpDefault<kp::OpTensorCreate>({ tensorA, tensorB });

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
        mgr.evalOpDefault<kp::OpAlgoBase<3, 1, 1>>(
            { tensorA, tensorB }, 
            std::vector<char>(shader.begin(), shader.end()));

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
       std::weak_ptr<kp::Sequence> sqWeakPtr = mgr.getOrCreateManagedSequence();

       if (std::shared_ptr<kp::Sequence> sq = sqWeakPtr.lock())
       {
           // Begin recording commands
           sq->begin();

           // Record batch commands to send to GPU
           sq->record<kp::OpMult<>>({ tensorLHS, tensorRHS, tensorOutput });
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
       mgr.evalOpAsyncDefault<kp::OpTensorCreate>({ tensor });

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

       // We can now await for the previous submitted command
       // The first parameter can be the amount of time to wait
       // The time provided is in nanoseconds
       mgr.evalOpAwaitDefault(10000);

       // Run Async Kompute operation on the parameters provided
       mgr.evalOpAsyncDefault<kp::OpAlgoBase<>>(
           { tensor }, 
           std::vector<char>(shader.begin(), shader.end()));

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
       mgr.createManagedSequence("queueOne", 0);
       mgr.createManagedSequence("queueTwo", 1);

       // Creates tensor an initializes GPU memory (below we show more granularity)
       auto tensorA = std::make_shared<kp::Tensor>(kp::Tensor(std::vector<float>(10, 0.0)));
       auto tensorB = std::make_shared<kp::Tensor>(kp::Tensor(std::vector<float>(10, 0.0)));

       // We run the first step synchronously on the default sequence
       mgr.evalOpDefault<kp::OpTensorCreate>({ tensorA, tensorB });

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

       // Run the first parallel operation in the `queueOne` sequence
       mgr.evalOpAsync<kp::OpAlgoBase<>>(
           { tensorA }, 
           "queueOne",
           std::vector<char>(shader.begin(), shader.end()));

       // Run the second parallel operation in the `queueTwo` sequence
       mgr.evalOpAsync<kp::OpAlgoBase<>>(
           { tensorB }, 
           "queueTwo",
           std::vector<char>(shader.begin(), shader.end()));

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

   template<uint32_t tX = 0, uint32_t tY = 0, uint32_t tZ = 0>
   class OpMyCustom : public OpAlgoBase<tX, tY, tZ>
   {
     public:
       OpMyCustom(std::shared_ptr<vk::PhysicalDevice> physicalDevice,
              std::shared_ptr<vk::Device> device,
              std::shared_ptr<vk::CommandBuffer> commandBuffer,
              std::vector<std::shared_ptr<Tensor>> tensors)
         : OpAlgoBase<tX, tY, tZ>(physicalDevice, device, commandBuffer, tensors, "")
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
       mgr.evalOpDefault<kp::OpMyCustom<3, 1, 1>>(
           { tensorLhs, tensorRhs, tensorOut });

       // Prints the output which is { 0, 4, 12 }
       std::cout << fmt::format("Output: {}", tensorOutput.data()) << std::endl;
   }


Logistic Regression Example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Logistic regression is oftens seen as the hello world in machine learning so we will be using it for our examples. Back to `examples list <#simple-examples>`_.

.. image:: ../images/logistic-regression.jpg
   :width: 300px


In summary, we have:


* Vector ``X`` with input data (with a pair of inputs ``Xi`` and ``Xj``\ )
* Output ``Y`` with expected predictions

With this we will:


* Optimize the function simplified as ``Y = WX + b``
* We'll want our program to learn the parameters ``W`` and ``b``

Converting to Kompute Terminology

.. code-block::


   We will have to convert this into Kompute terminology.

   First specifically around the inputs, we will be using the following:

   * Two vertors for the variable `X`, vector `Xi` and `Xj`
   * One vector `Y` for the true predictions
   * A vector `W` containing the two input weight values to use for inference
   * A vector `B` containing a single input parameter for `b`

   .. code-block:: cpp
       :linenos:

       std::vector<float> wInVec = { 0.001, 0.001 };
       std::vector<float> bInVec = { 0 };

       std::shared_ptr<kp::Tensor> xI{ new kp::Tensor({ 0, 1, 1, 1, 1 })};
       std::shared_ptr<kp::Tensor> xJ{ new kp::Tensor({ 0, 0, 0, 1, 1 })};

       std::shared_ptr<kp::Tensor> y{ new kp::Tensor({ 0, 0, 0, 1, 1 })};

       std::shared_ptr<kp::Tensor> wIn{ 
           new kp::Tensor(wInVec, kp::Tensor::TensorTypes::eStaging)};

       std::shared_ptr<kp::Tensor> bIn{ 
           new kp::Tensor(bInVec, kp::Tensor::TensorTypes::eStaging)};


   We will have the following output vectors:

   * Two output vectors `Wi` and `Wj` to store all the deltas to perform gradient descent on W
   * One output vector `Bout` to store all the deltas to perform gradient descent on B

   .. code-block:: cpp
       :linenos:

       std::shared_ptr<kp::Tensor> wOutI{ new kp::Tensor({ 0, 0, 0, 0, 0 })};
       std::shared_ptr<kp::Tensor> wOutJ{ new kp::Tensor({ 0, 0, 0, 0, 0 })};

       std::shared_ptr<kp::Tensor> bOut{ new kp::Tensor({ 0, 0, 0, 0, 0 })};


   For simplicity we will store all the tensors inside a params variable:

   .. code-block:: cpp
       :linenos:

       std::vector<std::shared_ptr<kp::Tensor>> params = 
           {xI, xJ, y, wIn, wOutI, wOutJ, bIn, bOut};


   Now that we have the inputs and outputs we will be able to use them in the processing. The workflow we will be using is the following:

   1. Create a Sequence to record and submit GPU commands
   2. Submit OpCreateTensor to create all the tensors 
   3. Record the OpAlgo with the Logistic Regression shader
   4. Loop across number of iterations:
      4-a. Submit algo operation on LR shader
      4-b. Re-calculate weights from loss
   5. Print output weights and bias

   1. Create a sequence to record and submit GPU commands

.. code-block:: cpp
    :linenos:

    kp::Manager mgr;

    if (std::shared_ptr<kp::Sequence> sq = 
            mgr.getOrCreateManagedSequence("createTensors").lock()) 
    {
        // ...



#. Submit OpCreateTensor to create all the tensors
   :raw-html-m2r:`<del>~</del>`\ :raw-html-m2r:`<del>~</del>`\ :raw-html-m2r:`<del>~</del>`\ :raw-html-m2r:`<del>~</del>`\ ~~

.. code-block:: cpp
    :linenos:

    {
        // ... continuing from codeblock above

        sq->begin();

        sq->record<kp::OpCreateTensor>(params);

        sq->end();
        sq->eval();




#. Record the OpAlgo with the Logistic Regression shader
   :raw-html-m2r:`<del>~</del>`\ :raw-html-m2r:`<del>~</del>`\ :raw-html-m2r:`<del>~</del>`\ :raw-html-m2r:`<del>~</del>`\ ~~

Once we re-record, all the instructions that were recorded previously are cleared.

Because of this we can record now the new commands which will consist of the following:


#. Copy the tensor data from local to device
#. Run the logistic regression shader
#. Copy the output data 

.. code-block:: cpp
    :linenos:

    {
        // ... continuing from codeblock above

        sq->begin();

        sq->record<kp::OpTensorSyncDevice>({wIn, bIn});

        sq->record<kp::OpAlgoBase<>>(
                params, 
                false, // Whether to copy output from device
                "test/shaders/glsl/test_logistic_regression.comp");

        sq->record<kp::OpTensorSyncLocal>({wOutI, wOutJ, bOut});

        sq->end();



#. Loop across number of iterations + 4-a. Submit algo operation on LR shader
   :raw-html-m2r:`<del>~</del>`\ :raw-html-m2r:`<del>~</del>`\ :raw-html-m2r:`<del>~</del>`\ :raw-html-m2r:`<del>~</del>`\ ~~

.. code-block:: cpp
    :linenos:

    {
        // ... continuing from codeblock above

        uint32_t ITERATIONS = 100;

        for (size_t i = 0; i < ITERATIONS; i++) 
        {
            // Run evaluation which passes data through shader once
            sq->eval();



4-b. Re-calculate weights from loss

.. code-block::


   Once the shader code is executed, we are able to use the outputs from the shader calculation.

   In this case we want to basically add all the calculated weights and bias from the back-prop step.

   .. code-block:: cpp
       :linenos:

       {
           // ... 
           for (size_t i = 0; i < ITERATIONS; i++) 
           {
               // ... continuing from codeblock above

               // Run evaluation which passes data through shader once
               sq->eval();

               // Subtract the resulting weights and biases
               for(size_t j = 0; j < bOut->size(); j++) {
                   wInVec[0] -= wOutI->data()[j];
                   wInVec[1] -= wOutJ->data()[j];
                   bInVec[0] -= bOut->data()[j];
               }
               // Set the data for the GPU to use in the next iteration
               wIn->mapDataIntoHostMemory();
               bIn->mapDataIntoHostMemory();
           }

   5. Print output weights and bias

.. code-block:: cpp
    :linenos:

    std::cout << "Weight i: " << wIn->data()[0] << std::endl;
    std::cout << "Weight j: " << wIn->data()[1] << std::endl;
    std::cout << "Bias: " << bIn->data()[0] << std::endl;



Logistic Regression Compute Shader
----------------------------------

Finally you can see the shader used for the logistic regression usecase below:

.. code-block:: cpp
   :linenos:

   #version 450

   layout (constant_id = 0) const uint M = 0;

   layout (local_size_x = 1) in;

   layout(set = 0, binding = 0) buffer bxi { float xi[]; };
   layout(set = 0, binding = 1) buffer bxj { float xj[]; };
   layout(set = 0, binding = 2) buffer by { float y[]; };
   layout(set = 0, binding = 3) buffer bwin { float win[]; };
   layout(set = 0, binding = 4) buffer bwouti { float wouti[]; };
   layout(set = 0, binding = 5) buffer bwoutj { float woutj[]; };
   layout(set = 0, binding = 6) buffer bbin { float bin[]; };
   layout(set = 0, binding = 7) buffer bbout { float bout[]; };

   float learningRate = 0.1;
   float m = float(M);

   float sigmoid(float z) {
       return 1.0 / (1.0 + exp(-z));
   }

   float inference(vec2 x, vec2 w, float b) {
       float z = dot(w, x) + b;
       float yHat = sigmoid(z);
       return yHat;
   }

   float calculateLoss(float yHat, float y) {
       return -(y * log(yHat)  +  (1.0 - y) * log(1.0 - yHat));
   }

   void main() {
       uint idx = gl_GlobalInvocationID.x;

       vec2 wCurr = vec2(win[0], win[1]);
       float bCurr = bin[0];

       vec2 xCurr = vec2(xi[idx], xj[idx]);
       float yCurr = y[idx];

       float yHat = inference(xCurr, wCurr, bCurr);
       float loss = calculateLoss(yHat, yCurr);

       float dZ = yHat - yCurr;
       vec2 dW = (1. / m) * xCurr * dZ;
       float dB = (1. / m) * dZ;
       wouti[idx] = learningRate * dW.x;
       woutj[idx] = learningRate * dW.y;
       bout[idx] = learningRate * dB;
   }
