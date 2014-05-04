#!/bin/sh

# we like to debug
set -x

# settings for the script

BUILD_DIRECTORY="./simple-build/"

INSTALL_DIRECTORY="./simple-install/"

BUILD_PARALLEL_JOBS_NUMBER=4

# internal variables

INSTALL_DIR_ABSOLUTE="$PWD/$INSTALL_DIRECTORY"

echo $INSTALL_DIR_ABSOLUTE

PATH_SETUP_SCRIPT="$INSTALL_DIR_ABSOLUTE/bin/ktechlab-paths.sh"

LAUNCHER_SCRIPT="$INSTALL_DIR_ABSOLUTE/bin/ktechlab-launch.sh"

USER_SETUP_SCRIPT="$INSTALL_DIR_ABSOLUTE/bin/ktechlab-user-setup.sh"
#

echo "Script for building KTechLab in a simple way"


# check for CMakeLists.txt, for verifying if we are in the correct directory
if [ ! -e CMakeLists.txt ]
then
    echo "Can't find CMakeLists.txt. Is the current directory "\
            "KTechLab's source directory?" ;
    echo "Exiting" ;
    exit 1 ;
fi

# create the build directory
mkdir -p "$BUILD_DIRECTORY"

if [ "$?" -ne "0" ]
then
    echo "Creation of build directory failed, exiting"
    exit 5;
fi


# create installation directory
mkdir -p "$INSTALL_DIRECTORY"

if [ "$?" -ne "0" ]
then
    echo "Creation of install directory failed, exiting"
    exit 6;
fi


# go to the newly created build directory
# "./dir" avoids CDPATH value
cd ./"$BUILD_DIRECTORY"

if [ "$?" -ne "0" ]
then
    echo "Directory change to build directory failed"
    exit 7;
fi


# configure the build
cmake .. -DCMAKE_BUILD_TYPE=debugfull \
  -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR_ABSOLUTE"

  # check for errors
if [ "$?" -eq "0" ]
then
    echo "Buildsystem generation done."
else
    echo "Buildsystem generation failed. Exiting."
    exit 2;
fi

# build the stuff
make -j$BUILD_PARALLEL_JOBS_NUMBER

  # check for errors
if [ "$?" -eq "0" ]
then
    echo "Build done."
else
    echo "Build failed. Exiting."
    exit 3;
fi

# install
make install

  # check for errors
if [ "$?" -eq "0" ]
then
    echo "Install done."
else
    echo "Install failed. Exiting."
    exit 4;
fi

# create a setup script

echo "#!/bin/sh" > $PATH_SETUP_SCRIPT

echo "export PATH=$INSTALL_DIR_ABSOLUTE/bin:$PATH" >> $PATH_SETUP_SCRIPT

echo "export LD_LIBRARY_PATH=$INSTALL_DIR_ABSOLUTE/lib/kde4:$INSTALL_DIR_ABSOLUTE/lib:$INSTALL_DIR_ABSOLUTE/lib64/kde4:$INSTALL_DIR_ABSOLUTE/lib64:$LD_LIBRARY_PATH" >> $PATH_SETUP_SCRIPT

echo "export XDG_DATA_DIRS=$INSTALL_DIR_ABSOLUTE/share:$XDG_DATA_DIRS" >> $PATH_SETUP_SCRIPT

echo "export QT_PLUGIN_PATH=$INSTALL_DIR_ABSOLUTE/lib/kde4:$QT_PLUGIN_PATH" >> "$PATH_SETUP_SCRIPT"

echo "export KDEDIRS=$INSTALL_DIR_ABSOLUTE:$KDEDIRS" >> $PATH_SETUP_SCRIPT

# create launcher script

echo "#!/bin/sh" > $LAUNCHER_SCRIPT

echo ". $PATH_SETUP_SCRIPT" >> $LAUNCHER_SCRIPT

echo "ktechlab" >> $LAUNCHER_SCRIPT

# create user setup script

echo "#!/bin/sh" > $USER_SETUP_SCRIPT

echo ". $PATH_SETUP_SCRIPT" >> $USER_SETUP_SCRIPT

echo "kbuildsycoca4" >> $USER_SETUP_SCRIPT

echo "update-mime-database $INSTALL_DIR_ABSOLUTE/share/mime" >> $USER_SETUP_SCRIPT

# run initial user setup

sh $USER_SETUP_SCRIPT


# done
