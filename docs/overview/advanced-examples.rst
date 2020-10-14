
Advanced Examples
==================

The power of Kompute comes in when the interface is used for complex computations. This section contains an outline of the advanced / end-to-end examples available.

Currently the advanced examples available include:

* `Machine Learning Logistic Regression Implementation <https://towardsdatascience.com/machine-learning-and-data-processing-in-the-gpu-with-vulkan-kompute-c9350e5e5d3a`_
* `Android NDK Mobile Kompute ML Application <https://towardsdatascience.com/gpu-accelerated-machine-learning-in-your-mobile-applications-using-the-android-ndk-vulkan-kompute-1e9da37b7617`_
* `Game Development Kompute ML in Godot Engine <https://towardsdatascience.com/supercharging-game-development-with-gpu-accelerated-ml-using-vulkan-kompute-the-godot-game-engine-4e75a84ea9f0`_

Below there is also a simple implementation of the logistic regression algorithm using Vulkan Kompute.

Logistic Regression Example
------------------

Logistic regression is oftens seen as the hello world in machine learning so we will be using it for our examples. 

.. image:: ../images/logistic-regression.jpg
   :width: 300px

In summary, we have:

* Vector `X` with input data (with a pair of inputs `Xi` and `Xj`)
* Output `Y` with expected predictions

With this we will:

* Optimize the function simplified as `Y = WX + b`
* We'll want our program to learn the parameters `W` and `b`

Converting to Kompute Terminology
~~~~~~~~~~~~~~~~~~~~~~

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
3. Record the OpAlgo with the Logistic Regresion shader
4. Loop across number of iterations:
   4-a. Submit algo operation on LR shader
   4-b. Re-calculate weights from loss
5. Print output weights and bias

1. Create a sequence to record and submit GPU commands
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp
    :linenos:

    kp::Manager mgr;

    if (std::shared_ptr<kp::Sequence> sq = 
            mgr.getOrCreateManagedSequence("createTensors").lock()) 
    {
        // ...

2. Submit OpCreateTensor to create all the tensors
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp
    :linenos:

    {
        // ... continuing from codeblock above

        sq->begin();

        sq->record<kp::OpCreateTensor>(params);

        sq->end();
        sq->eval();


3. Record the OpAlgo with the Logistic Regresion shader
~~~~~~~~~~~~~~~~~~~~~~

Once we re-record, all the instructions that were recorded previosuly are cleared.

Because of this we can record now the new commands which will consist of the following:

1. Copy the tensor data from local to device
2. Run the logistic regression shader
3. Copy the output data 

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

4. Loop across number of iterations + 4-a. Submit algo operation on LR shader
~~~~~~~~~~~~~~~~~~~~~~

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
~~~~~~~~~~~~~~~~~~~~~~

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

            // Substract the resulting weights and biases 
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
~~~~~~~~~~~~~~~~~~~~~~


.. code-block:: cpp
    :linenos:

    std::cout << "Weight i: " << wIn->data()[0] << std::endl;
    std::cout << "Weight j: " << wIn->data()[1] << std::endl;
    std::cout << "Bias: " << bIn->data()[0] << std::endl;


Logistic Regression Compute Shader
------------------------

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




