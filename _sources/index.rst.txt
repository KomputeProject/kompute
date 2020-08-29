.. Vulkan Kompute documentation master file, created by
   sphinx-quickstart on Fri Aug 28 06:42:00 2020.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to Vulkan Kompute's documentation!
==========================================

.. mdinclude:: ../README.md

.. toctree::
   :maxdepth: 2
   :caption: Contents:


Reference
========

You can also go directoy to the `raw doxygen docs <doxygen/annotated.html>`_

Overview of entire dependencies and their relationship with vulkan component ownership.

.. image:: images/kompute-vulkan-architecture.jpg
   :width: 100%

Manager
-------

.. doxygenclass:: kp::Manager
   :members:

.. image:: images/kompute-vulkan-architecture-manager.jpg
   :width: 100%

Sequence
-------

.. doxygenclass:: kp::Sequence
   :members:

.. image:: images/kompute-vulkan-architecture-sequence.jpg
   :width: 100%

Tensor
-------

.. doxygenclass:: kp::Tensor
   :members:

.. image:: images/kompute-vulkan-architecture-tensor.jpg
   :width: 100%

Algorithm
-------

.. doxygenclass:: kp::Algorithm
   :members:

.. image:: images/kompute-vulkan-architecture-algorithm.jpg
   :width: 100%

OpBase
-------

.. doxygenclass:: kp::OpBase
   :members:

.. image:: images/kompute-vulkan-architecture-operations.jpg
   :width: 100%

OpAlgoBase
-------

.. doxygenclass:: kp::OpAlgoBase
   :members:

.. image:: images/kompute-vulkan-architecture-opmult.jpg
   :width: 100%

OpMult
-------

.. doxygenclass:: kp::OpMult
   :members:

.. image:: images/kompute-vulkan-architecture-opmult.jpg
   :width: 100%

OpCreateTensor
-------

.. doxygenclass:: kp::OpCreateTensor
   :members:

.. image:: images/kompute-vulkan-architecture-opcreatetensor.jpg
   :width: 100%



Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
