
KTechLab 0.50.1
=============

KTechLab is an IDE for microcontrollers and electronics.
It supports circuit simulation,
program development for microcontrollers and
simulating the programmed microcontroller together with its application circuit.

Introduction
-------------

Resources
-------------
- KTechLab is joining KDE, all of its assets will be available there.

    - https://community.kde.org/Incubator/Projects/KTechLab

    - current webpage:
        https://userbase.kde.org/KTechlab
        TODO better webpage at KDE

    - repository browser at KDE (web based):
        https://commits.kde.org/ktechlab
        https://invent.kde.org/sdk/ktechlab

    - source code repository at KDE (to be used with git):
        git@invent.kde.org:sdk/ktechlab.git

    - bugtracker at KDE:
        https://bugs.kde.org/describecomponents.cgi?product=KTechLab

    - mailing list at KDE:
        ktechlab-devel at KDE
        https://mail.kde.org/mailman/listinfo/ktechlab-devel
        Archives contain the messages from the old list, too

- on IRC, KTechLab developers regularly join the channel
    `#ktechlab`, on FreeNode

- source code repositories: see Getting the source section, below

- KTechLab on github (will become deprecated)
    
    - https://github.com/ktechlab

    - https://github.com/ktechlab/ktechlab/wiki

    - https://github.com/ktechlab/ktechlab
    - https://github.com/ktechlab/ktechlab-0.3 (archive only; merged into the above repoistory)
    - new commits will be always pushed to KDE Git, github will be a mirror only

- KTechLab on sourceforge (mostly deprecated; source code downloads are available)
    - wiki:
        http://ktechlab.sourceforge.net
    - project page:
        http://sourceforge.net/projects/ktechlab
    - project page for developers:
        http://sourceforge.net/projects/ktechlab/develop

- the official communucation channel is hosted at KDE; the mailing list on sourceforge will become deprecated;
        it is called ktechlab-devel
    - link to archives of the list:
        http://sourceforge.net/mailarchive/forum.php?forum_name=ktechlab-devel



Getting the source
------------------

KTechLab's source code is located in a GIT repository.

Official repository is hosted at KDE; its location is at

    git@invent.kde.org:sdk/ktechlab.git

  - repository browser
      https://invent.kde.org/sdk/ktechlab

  - for write access, see section Write access to KTechLab repository at KDE, below.


Deprecated official repositories located on github; they mirror code from KDE,
and also have branches named github/* for historical reference.
For new code use the KDE Git.

- https://github.com/ktechlab/ktechlab
    - the master branch contains the latest tested development code,
        while other branches might contain untested/unstable code

- https://github.com/ktechlab/ktechlab-0.3
    - the port-0.3.8-kde4-v1 branch contains currently the latest code
    (at the time of origina writing; this branch has become master branch in KDE repositories)


Other notable deprecated GIT repositories are located on sourceforge:

- main KTechLab repository (deprecated, use KDE Git):
    - `git://ktechlab.git.sourceforge.net/gitroot/ktechlab/ktechlab`
    - contains the contents of the previously used SVN repository

- developers' own repositories (deprecated, use KDE Git):
    - `git://ktechlab.git.sourceforge.net/gitroot/ktechlab/ktl-alonzotg`
    - `git://ktechlab.git.sourceforge.net/gitroot/ktechlab/ktl-j_ohny_b`
    - `git://ktechlab.git.sourceforge.net/gitroot/ktechlab/ktl-zoltan_p`
        - the master branch should have the same content as the
            official master branch from github
    - these repositories contain work-in-progress code,
        which is supposed to be merged in the master branch on github

Checking out from GIT can be performed with the following commands:

    git clone git://github.com/ktechlab/ktechlab.git
    cd ktechlab


Building and Running
--------------------

Required dependencies

- Qt5 -dev
- cmake
- glib -dev
- Extra Cmake Modules (ECM)
- KDE Frameworks 5 libraries -dev  (at the time of writing:
   Config CoreAddons DocTools IconThemes I18n KDELibs4Support
    KIO Parts TextEditor TextWidgets WidgetsAddons
    WindowSystem XmlGui)

Optional dependencies

- GPSim -dev, for microcontroller support



In KDE Frameworks 5, all Makefiles in KDE projects are generated using CMake.
After checking out the source (see Getting the source),
here is how to compile and install KTechLab.
CMkake is not a run-time dependency of KTechLab.


1. By using a terminal, navigate to the top-level source-directory

2. Run the shell script `build-simple.sh`.
    This command compiles KTechLab into the directory `build-simple`,
    installs it into the directory `inst-simple` and
    sets up the user-specific settings (syscoca, mime database) for the
    current user.

         sh build-simple.sh

3. Launch KTechLab by running the script `run-simple.sh`

         sh run-simple.sh

KTechLab should start running at this point.
If this simple method of launching KTechLab does not work,
please contact the developers, because you have found a bug.

## Running a build when the source/build/install directory has been moved

It the source directory has been moved, then the setup procedure has
to be rerun. This is due to the fact that in the launch scripts of
KTechLab, absolute directory paths are generated.
The compilation step should be a lot faster than previously, because
the source files have not been changed, so the existing build is
just verified, not recreated.

         sh build-simple.sh
         sh run-simple.sh

Developing with KDevelop 4 / 5
--------------------------

Just open/import the CMakeLists.txt project file.


Write access to KTechLab repository at KDE
------------------------------------------

Please read the KDE Git manual:

    https://community.kde.org/Sysadmin/GitKdeOrgManual

You need:

1. KDE developer account; should be usable at `identity.kde.org` ; see
    https://community.kde.org/Infrastructure
    https://community.kde.org/Infrastructure/Get_a_Developer_Account

    An SSH key should be registered at KDE and usable on your computer.

2. Set up git for usage with KDE Git Infrastructure
    https://community.kde.org/Infrastructure/Git
    https://techbase.kde.org/Development/Git/Configuration
        git config --global user.name <Your Real Name>
        git config --global user.email <Your identity.kde.org email>

        cat ~/.gitconfig

        [url "git://anongit.kde.org/"]
            insteadOf = kde:
        [url "git@git.kde.org:"]
            pushInsteadOf = kde:

3. Clone KTechLab repository; current location is temporary:
        git clone kde:scratch/padrah/ktechlab

Pushing should work with the URL rewrite rules from step 2.


Porting to KF5 / Qt5
--------------------

See
1. https://techbase.kde.org/ECM5/IncompatibleChangesKDELibs4ToECM
2. https://community.kde.org/Frameworks/Porting_Notes
3. https://api.kde.org/frameworks/index.html

Have fun with KTechLab!
