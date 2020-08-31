
Advanced Examples
==================

The power of Kompute comes in when the interface is used for complex computations. In this section we cover a set of advanced examples including machine learning and data processing applications that showcase the more advanced capabilities of Kompute.


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

    sq->begin();

    sq->record<kp::OpCreateTensor>(params);

    sq->end();
    sq->eval();


3. Record the OpAlgo with the Logistic Regresion shader
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp
    :linenos:

    sq->begin();

    sq->record<kp::OpAlgoBase<>>(
            params, 
            true, // Whether to copy output from device
            "test/shaders/glsl/test_logistic_regression.comp");

    sq->end();

4. Loop across number of iterations + 4-a. Submit algo operation on LR shader
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp
    :linenos:

    // Iterate across all expected iterations
    for (size_t i = 0; i < ITERATIONS; i++) 
    {
        sq->eval();


   4-b. Re-calculate weights from loss

.. code-block:: cpp
    :linenos:

    for(size_t j = 0; j < bOut->size(); j++) {
        wInVec[0] -= wOutI->data()[j];
        wInVec[1] -= wOutJ->data()[j];
        bInVec[0] -= bOut->data()[j];
    }
    wIn->setData(wInVec);
    bIn->setData(bInVec);

5. Print output weights and bias
~~~~~~~~~~~~~~~~~~~~~~


.. code-block:: cpp
    :linenos:

    REQUIRE(wIn->data()[0] < 0.01);
    REQUIRE(wIn->data()[1] > 1.0);
    REQUIRE(bIn->data()[0] < 0.0);

    SPDLOG_DEBUG("Result wIn: {}, bIn: {}", 
            wIn->data(), bIn->data());

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




