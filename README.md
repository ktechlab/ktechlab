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


Building and Running
--------------------

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
CMkake is not a run-time dependency of KTechLab.


1. By using a terminal, navigate to the top-level source-directory

2. Run the shell script `simple-setup.sh`.
    This command compiles KTechLab into the directory `simple-build`,
    installs it into the directory `simple-install` and
    sets up the user-specific settings (syscoca, mime database) for the
    current user.

         sh simple-setup.sh

3. Launch KTechLab by running the script `simple-launch.sh`

         sh simple-launch.sh

KTechLab should start running at this point.
If this simple method of launching KTechLab does not work,
please contact the developers, because you found is a bug.

## Running by multiple users with same build/install directory

It is possible to run an already compiled and installed version of KTechLab
by multiple users. In order to do this, run the script
`ktechlab-user-setup.sh` from the `simple-install/bin/` directory

         sh simple/install/bin/ktechlab-user-setup.sh

Then you can launch KTechLab as usual

        sh simple-launch.sh

## Running a build when the source/build/install directory has been moved

It the source directory has been moved, then the setup procedure has
to be rerun. This is due to the fact that in the launch scripts of
KTechLab, absolute directory paths are generated.
The compilation step should be a lot faster than previously, because
the source files have not been changed, so the existing build is
just verified, not recreated.

         sh simple-setup.sh
         sh simple-launch.sh

Have fun with KTechLab!
