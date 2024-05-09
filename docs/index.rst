Welcome to |project|'s documentation!
=====================================

This example shows that any file or folder inside a project's root-level `docs` folder
is included in the Sphinx documentation build, and takes prescedence over the default.

This includes the `index.rst` file, and this allows you to include extra toc-trees for
project-level documentation:

.. toctree::
   :maxdepth: 2
   :caption: Project-Level Documentation

   guide.rst


The following example shows how to include the component-level toctree:

.. toctree::
   :maxdepth: 2
   :caption: Components

   @COMPONENT_TOCTREE@


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
