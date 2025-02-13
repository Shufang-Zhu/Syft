###Compilation Instructions using CMake

==== Install CUDD ====

0.1 Make sure CUDD is installed. CUDD can be found at: 

    https://github.com/KavrakiLab/cudd.git

0.2 Install CUDD:

    ./configure --enable-silent-rules --enable-obj --enable-dddmp --prefix=[install location]
    sudo make install

    If you get an error about aclocal, this might be due to either
    a. Not having automake:
        sudo apt-get install automake
    b. Needing to reconfigure, do this before configuring:
        autoreconf -i


==== Install FLEX, BISON ====

0.3 Install flex and bison:

    sudo apt-get install flex bison



==== Install SPOT ====

0.5 Spot can be found at:

    https://spot.lrde.epita.fr/


==== Install LYDIA ====

    cd submodules
    cd lydia

Follow the instructions to complete the installation of lydia.


==== Build SYFTMAX ====

1. Make build folder so your directory is not flooded with build files:

    ```mkdir build && cd build```

3. Run CMake to generate the makefile:

    ```cmake -DCMAKE_BUILD_TYPE=Release ..```

4. Compile using the generated makefile:

    ```make```

==== Run SYFTMAX ====

1. Reach executable file Syftmax

   ```cd bin```

2. Run example:

    ---- Synthesize one single strategy ----
        Agn first: ```./Syftmax -f ../../example/env.ltlf -p ../../example/test.part```

    ---- Synthesize the maximally permissive strategy ----
        Agn first: ```./Syftmax -f ../../example/env.ltlf -p ../../example/test.part -m```
   


