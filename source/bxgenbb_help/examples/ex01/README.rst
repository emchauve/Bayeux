===========================
geomtools ``examples/ex01``
===========================

Introduction
============

 * Description:

   This  example illustrates  how to configure and use the event
   generator manager.
   Also it is shown how to use the ``bxgenbb_inspector`` utility.

 * Source file(s) :

   * ``ex01.cxx`` : the main program for using the genbb::manager class.
   * ``ex01_plain_reader.cxx`` : the program to read primary event data
     from a file (plain Boost/Serialization-based datatools I/O format).

 * Configuration files :

   * ``config/manager.conf`` : the main configuration file of the geometry
     manager.
   * Event generators' configuration files :

     * ``config/generators/backgrounds.conf`` : some event generators
     * ``config/generators/calibrations.conf`` : some event generators
     * ``config/generators/dbd.conf`` : some event generators
     * ``config/generators/misc.conf.conf`` : some event generators
     * ``config/generators/electron_energy_spectrum_0.data`` : data file
       which contains the tabulated energy spectrum used by
       a specific electron generator

 * Built object(s) :

     * ``ex01`` : the example executable linked to the ``Bayeux`` library.
     * ``ex01_plain_reader`` : the reader executable linked to the ``Bayeux`` library.

 * Build method: CMake.

Quick start
===========

1. Build, install and setup the Bayeux library
2. Make a copy of the example directory:

.. code:: sh

   shell> cp -a $(bxquery --exampledir)/genbb_help/ex01 /tmp/genbb_help_ex01
   shell> cd /tmp/genbb_help_ex01
..

3. Build and install the example::

.. code:: sh

   shell> mkdir _build.d
   shell> cd _build.d
   shell> cmake \
	  -DCMAKE_INSTALL_PREFIX=./_install.d \
          -DBayeux_DIR:PATH=$(bxquery --cmakedir) \
          ..
   shell> make
   shell> make install
   shell> cd ..
..

4. Run the example::

.. code:: sh

   shell> ./ex01
..

5. Run the ``bxgenbb_inspector``

     Print the list of available generators: ::

.. code:: sh

   shell> bxgenbb_inspector \
	  --configuration config/manager.conf \
          --action list
..

     Generate 10000 Co60 decay events, build and save histograms
     in a ROOT file: ::

.. code:: sh

   shell> bxgenbb_inspector \
	  --configuration config/manager.conf \
	  --action shoot \
          --generator "Co60" \
          --prng-seed 314159 \
          --number-of-events 10000 \
          --modulo 1000 \
          --prompt \
          --delayed \
          --prompt-time-limit 1 \
          --histo-def "@genbb_help:inspector/config/le_nuphy-1.0/inspector_histos_prompt.conf" \
          --histo-def "@genbb_help:inspector/config/le_nuphy-1.0/inspector_histos_delayed.conf" \
          --output-file "histos_Co60.root"
..

     Generate 10000 Co60 decay events, save them in a data file using
     the Boost serialization portable binary format: ::

.. code:: sh

   shell> bxgenbb_inspector \
	  --configuration config/manager.conf \
          --action shoot \
          --generator "Co60" \
          --prng-seed 314159 \
          --number-of-events 100 \
          --modulo 1000 \
	  --output-mode "plain" \
          --output-file "Co60_10000.xml"
..

     Read the primary event from the generated file: ::

.. code:: sh

      shell> ./ex01_plain_reader Co60_10000.xml
..


6. Check the output file:

   Output histograms browsable from ROOT via the ``histos_Co60.root`` file ::

.. code:: sh

      shell> root histos_Co60.root
      root [1] TBrowser b;
      root [2] .q
..


7. Clean::

.. code:: sh

      shell> rm -fr ./_build.d
      shell> rm -fr ./_install.d
..

8. Note:

   Run an automated test script: ::

.. code:: sh

      shell> ./process.sh
..
