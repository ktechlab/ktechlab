Development of KTechLab with KDevelop 4
=======================================

This document presents instructions for setting up development environment
in KDevelop 4 for developming KTechLab.

The figures referred in this document are located in the
`figures-kdevelop4-guide`
directory.

Prerequisites
-------------

Please refer to the
Building and Running
section of the README.md file for the dependencies of KTechab.

Besides the requirements listed in the README.md file,
KDevelop 4 should be installed.

Initial build
-------------

First run the `simple-setup.sh` script, in order to perform the initial
configuration of CMake build system.

            simple-setup.sh

See figure `01-run-simple-setup.png`.

Creating KDevelop 4 project
---------------------------

Open KDevelop 4: `02-start-new-session-in-kdevelop4.png`.

Select `Project` -> `Open / Import Project...`: `03-open-import-project.png`.

Navigate to the downloaded source tree of KTechLab and
select the `CMakeLists.txt` file: `04-import-project-from-cmakelists.png`.

Click `Next`: `05-import-create-project.png`.

Click `Finish`.
On the next dialog box, with the settings of CMake,
For the build directory select as build directory the `simple-build`
subdirectory of the source tree:
`06-select-build-dir.png` and
`07-import-configure-cmake.png`.
Click `OK`.

Now the Kdevelop 4 project should be created: `08-project-created.png`.


Testing the build and install processes
---------------------------------------

At this stage building and "installing" the project should work.
The "installation" sets up a runnable KTechLab installation in the
`simple-install` subdirectory of the source tree.
Without installation, KTechLab will not find its plugins, so it won't be
able to display or edit circuits.

For building select the KTechLab project in KDevelop 4 and click on the
toolbar `Build Selection`: `09-build-should-work.png`.
The build should finish fast, as the source tree has been already built.
For installing, the `Install selection` button can be clicked from the
toolbar of the `Projects` tool view: `10-install-should-work.png`.


Running and  KTechLab from KDevelop 4
------------------------------------------------

For running KTechLab from KDevelop 4, Launches have to be created in
KDevelop 4.
In the menu select `Run` -> `Configure Launches...`:
`11-configure-launches-menu.png`.
In the newly appeared dialog click `Add new launch configuration`:
`12-add-new-launch.png`.
As Executable select `Executable`, click the browse button, and
from the source tree select
`simple-install/bin/ktechlab`: `13-select-executable.png`.

For proper launch of KTechLab, several environment variables have to be
correctly set up. These are:
- `PATH`
- `LD_LIBRARY_PATH`
- `XDG_DATA_DIRS`
- `QT_PLUGIN_PATH`
- `KDEDIRS`
For easy management, KDevelop 4 can set up the environment of a program that
is being launched.
Click the `Configure Environment Variables` button:
`14-create-environment-button.png`,
`15-create-environment-dialog.png`.
In the environment configuration dialog, write a name for the new
environment settings group, and click the `Add group` button:
`16-name-of-new-environment.png`.
Next, the environment variable names and values have to be set up.
In order to do so, open the `simple-install/bin/ktechlab-paths.sh`
file from the source tree and copy the names and values of the
environment variables to the environment setup dialog in KDevelop 4.
An example using the terminal is presented on figures
`17-ktechlab-environment.png` and
`18-copy-paste-environment-variables.png`.
The end result is depicted on
`19-environment-set-up.png`.
Click `OK` on the environment setup dialog.

After the environment has been set up, select it as the used environment
for launching KTechLab:
`20-select-environment.png`.
Finally click `OK` to finish setting up the launch.

Now by clicking `Execute Current Launch`, KTechLab can be started:
`21-execute-launch.png`,
`22-ktechlab-started.png`.


Verifying the loaded plugins in KTechLab
----------------------------------------

In order to ensure that KTechLab is launched properly, it is recommended to
verify the list of loaded KTechLab plugins.
For doing so, in KTechLab, select `Help` -> `Loaded Plugins` from the menu:
`23-loaded-plugins.png`.
A list similar to `24-loaded-plugins-list.png` should be shown.
Notice the plugins that have in their name KTechLab.


Debugging KTechLab with KDevelop 4
----------------------------------

Because launching KTechLab has been already set up,
the `Debug Current Launch` button from the toolbar can be readily used:
`25-debug-launch.png`.

KDevelop can be used from now on to work on KTechLab:
`26-work-on-application.png`.

Questions?
----------
Feel free to contact the developers
- by email on the `ktechlab-devel` list on SourceForge:
    http://sourceforge.net/mailarchive/forum.php?forum_name=ktechlab-devel
- chat on IRC: `#ktechlab` on `freenode.net`
- any other means listed on KTechLab's project page on Github
    https://github.com/ktechlab/ktechlab#resources

Possible improvements for this document
---------------------------------------
- add inline images
    ![Alt text](/path/to/img.jpg)
    ![Alt text](/path/to/img.jpg "Optional title")
- document the usage of test cases
