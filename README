KTechLab
=============

KTechLab is an IDE for microcontrollers and electronics.
It supports circuit simulation,
program development for microcontrollers and
simulating the programmed microcontroller together with its application circuit.

Introduction
-------------

Resources
-------------
- KTechLab on github:
    https://github.com/ktechlab

- KTechLab on sourceforge
    - wiki:
        http://ktechlab.sourceforge.net
    - project page:
        http://sourceforge.net/projects/ktechlab
    - project page for developers:
        http://sourceforge.net/projects/ktechlab/develop

- the official communucation channel is hosted on sourceforge,
        it is called ktechlab-devel
    - link to archives of the list:
        http://sourceforge.net/mailarchive/forum.php?forum_name=ktechlab-devel

- on IRC, KTechLab developers regularly join the channel
    #ktechlab, on FreeNode

- source code repositories: see Getting the source section, below


Getting the source
------------------

KTechLab's source code is located in GIT repositories.

The official repository is located on github:

- https://github.com/ktechlab/ktechlab

- the master branch contains the latest tested development code,
    while other branches might contain untested/unstable code

Other notable GIT repositories are located on sourceforge:

- main KTechLab repository:
    - `git://ktechlab.git.sourceforge.net/gitroot/ktechlab/ktechlab`
    - contains the contents of the previously used SVN repository

- developers' own repositories:
    - git://ktechlab.git.sourceforge.net/gitroot/ktechlab/ktl-alonzotg
    - git://ktechlab.git.sourceforge.net/gitroot/ktechlab/ktl-j_ohny_b
    - git://ktechlab.git.sourceforge.net/gitroot/ktechlab/ktl-zoltan_p
        - the master branch should have the same content as the
            official master branch from github
    - these repositories contain work-in-progress code,
        which is supposed to be merged in the master branch on github

Checking out from GIT can be performed with the following commands:

    git clone git://github.com/ktechlab/ktechlab.git
    cd ktechlab
    git checkout -b master --track origin/master


Building and Installing
-----------------------

Required dependencies

- Qt4 -dev
- cmake
- glib -dev

Optional dependencies

- KDE4 libraries -dev
- KDevPlatform libraries -dev
    - NOTE: without the following two components, only the unit tests
        will be built; those are not useful for end-users
- GPSim -dev, for microcontroller support
- Eigen2 -dev, for unit tests; generally it is not needed for builds
    targeted to end-users, but it is useful for development



Since KDE4, all Makefiles in KDE projects are generated using CMake.
After checking out the source (see Getting the source),
here is how to compile and install KTechLab.

1. By using a terminal, navigate to the top-level source-directory

2. Create and enter a build directory:

        mkdir build && cd build

3. Run cmake to generate the Makefiles:

        cmake .. -DCMAKE_BUILD_TYPE=debugfull -DCMAKE_INSTALL_PREFIX=~/usr/

    This command enables debug-symbols to be built
    and installs all files into a `usr` sub-directory in your home-directory.
    If cmake complains about any missing libraries,
    you need to install development versions for those.
    Make sure cmake finishes with no error until you proceed.

3. Compile and install the source:

        make install

4. If you installed into a non-standard prefix
    (like suggested in the cmake-command-line above),
    you need to make sure that some environment variables are set:

        export PATH=~/usr/bin:$PATH

        export LD_LIBRARY_PATH=~/usr/lib/kde4:~/usr/lib:$LD_LIBRARY_PATH

        export XDG_DATA_DIRS=~/usr/share:$XDG_DATA_DIRS

        export QT_PLUGIN_PATH=~/usr/lib/kde4:$QT_PLUGIN_PATH

        export KDEDIRS=~/usr:$KDEDIRS

5. After installing new versions of .desktop files or other
    freedesktop.org-related files, you need to run:

        kbuildsycoca4

    to update all caches to use these files.
    This is especially important when creating new plugins.
    They won't be loadable by (or even visible to) the KDE's plugin-system,
    until you ran kbuildsycoca4.
    Note that you need to have the environment variables from step 5 set.

6. On fresh builds, it's necessary to update the mime database.
    This will be done by the following command:

        update-mime-database /path/to/mime

    Where /path/to/mime is the path, where the ktechlab.xml gets installed to.
    For example:

        update-mime-database ~/usr/share/mime

    (there should be ~/usr/share/mime/packages/ktechlab.xml in this case)


Running/Testing
-----------------------

If building and installing has been successfully completed, the command

    ktechlab

Should launch KTechLab. Note that in some cases the KTechLab plugins
might be disabled, so they need to be enabled from the menu:
Setting -> Configure KTechLab -> Plugins,
and enable all the KTechLab-related plugins.

Sometimes it's easier to launch KTechLab from a script,
mostly for debugging purposes.
The script presented below sets the environment-variables and
installs all the mime-types before launching the KTechLab
executable located in the current directory:

    #!/bin/bash

    export PATH=~/usr/bin:$PATH
    export LD_LIBRARY_PATH=~/usr/lib/kde4:~/usr/lib:$LD_LIBRARY_PATH
    # export XDG_DATA_DIRS=~/usr/share:$XDG_DATA_DIRS
    export XDG_DATA_DIRS=$HOME/usr/share:$XDG_DATA_DIRS
    export QT_PLUGIN_PATH=~/usr/lib/kde4:$QT_PLUGIN_PATH
    export KDEDIRS=~/usr:$KDEDIRS

    kbuildsycoca4  &> /dev/null

    update-mime-database $HOME/usr/share/mime

    ./ktechlab
