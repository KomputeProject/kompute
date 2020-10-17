
Asynchronous and Parallel Operations
=============

In GPU computing it is possible to have multiple levels of asynchronous and parallel processing of GPU tasks.

It is important to understand the conceptual distinctions of the diffent terminology when using each of these components.

In this section we will cover the following points:

* Asynchronous operation submission
* Parallel processing of operations

Asynchronous operation submission
---------------------------------

As the name implies, this refers to the asynchronous submission of operations. This means that operations can be submitted to the GPU, and the C++ / host CPU can continue performing tasks, until when the user desires to run `await` to wait until the operation finishes.

This basically provides further granularity on Vulkan Fences, which is its means to enable the CPU host to know when GPU commands have finished executing. 

It is important that submitting tasks asynchronously, does not mean that these will be executed in parallel. Parallel execution of operations will be covered in the following section.

Asynchronous operation submission can be achieved through the kp::Manager, or directly through the kp::Sequence. Below is an example using the Kompute manager.

Async/Await Example
^^^^^^^^^^^^^^^^^^^^^

Asynchronous job submission is done using `evalOpAsync` and `evalOpAwait` functions.

For simplicity the `evalOpAsyncDefault` and `evalOpAwaitDefault` functions are provided, which can be used similar to the synchronous counterparts (which basically use the default named sequence).

A simple example of asynchronous submission can be found below.

One important thing to bare in mind when using asynchronous submissions, is that you should make sure that any overlapping asynchronous functions are run in separate sequences.

The reason why this is important is that the Await function not only waits for the fence, but also runs the `postEval` functions across all operations, which is required for several operations.

.. code-block:: cpp
    :linenos:

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


Parallel Operation Submission
-----------

In order to work with parallel execution of tasks, it is important that you understand some of the core GPU processing limitations, as these can be quite broad and hardware dependent, which means they will vary across NVIDIA / AMD / ETC video cards.

GPUs by default will optimize towards GPU 


