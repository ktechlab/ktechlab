Name:             ktechlab
#development snapshot
Version:          0.3
Release:          60%{?dist}
Summary:          Development and simulation of microcontrollers and electronic circuits

License:          GPL
Group:            Applications/Engineering

URL:              http://ktechlab.org/
Source:           http://ktechlab.org/download/ktechlab-dist.tar.bz2

Patch1:           ktechlab-0.3.desktop.patch
Patch2:           ktechlab-0.3-ppc-includes.patch

BuildRoot:        %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:    gpsim-devel, readline-devel, kdelibs-devel
BuildRequires:    desktop-file-utils

# Ktechlab requires gputils for PIC simulation.
Requires:         gputils

%description
KTechlab is a development and simulation environment for microcontrollers
and electronic circuits, distributed under the GNU General Public License.

KTechlab consists of several well-integrated components:
A circuit simulator, capable of simulating logic, linear devices and some
nonlinear devices.
* Integration with gpsim, allowing PICs to be simulated in circuit.
* A schematic editor, which provides a rich real-time feedback of the
simulation.
* A flowchart editor, allowing PIC programs to be constructed visually.
* MicroBASIC; a BASIC-like compiler for PICs, written as a companion program
to KTechlab.
* An embedded Kate part, which provides a powerful editor for PIC programs.
* Integrated assembler and disassembler via gpasm and gpdasm.

%prep
%setup -q

%patch1 -p0 -b .desktop

%ifarch ppc
%patch2 -p1 -b .ppc-includes
%endif

sed -i -e s'|SUBDIRS = core gui|SUBDIRS = math core gui|' src/Makefile.in

%build

%configure --disable-rpath

%{__make} %{?_smp_mflags}

%install
%{__rm} -rf %{buildroot}
%{__make} DESTDIR=%{buildroot} install

desktop-file-install --vendor "" \
    --add-category Enginneering \
    --delete-original \
    --dir %{buildroot}%{_datadir}/applications/ \
    %{buildroot}%{_datadir}/applnk/Development/%{name}.desktop

# Fix absolute symlink
%{__rm} -f %{buildroot}%{_docdir}/HTML/en/%{name}/common

%{__rm} -f %{buildroot}%{_libdir}/libtechmath.la

# No translation
#%find_lang %{name}

%clean
%{__rm} -rf %{buildroot}

%post
touch --no-create %{_datadir}/icons/hicolor || :
%{_bindir}/gtk-update-icon-cache --quiet %{_datadir}/icons/hicolor || :
%{_bindir}/update-desktop-database %{_datadir}/applications || :

%postun
touch --no-create %{_datadir}/icons/hicolor || :
%{_bindir}/gtk-update-icon-cache --quiet %{_datadir}/icons/hicolor || :
%{_bindir}/update-desktop-database %{_datadir}/applications || :

%files
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING TODO
%{_bindir}/%{name}
%{_bindir}/microbe
%{_datadir}/apps/%{name}
%{_datadir}/apps/katepart/syntax/microbe.xml
%dir %{_datadir}/config.kcfg/
%{_datadir}/config.kcfg/%{name}.kcfg
%{_datadir}/mimelnk/application/*.desktop
%{_datadir}/applications/%{name}.desktop
%{_datadir}/doc/HTML/en/%{name}
%{_datadir}/icons/hicolor/*/*/*.png
%{_libdir}/libtechmath
%{_libdir}/libtechmath.0*


%Changelog
* Mon Dec 25 2006 Chitlesh Goorah <chitlesh@fedoraproject.org> - 0.3-60 #development release
- New development snapshot
- dropped ktechlab-0.3-pic.patch
- package does not ship autom4te.cache anymore
- fixed missing make for src/math

* Mon Oct 25 2006 Chitlesh Goorah <chitlesh@fedoraproject.org> - 0.3-6
- Rebuilt due to new gpsim-devel release
- removed fedora vendor

* Fri Oct 13 2006 Mamoru Tasaka <mtasaka@ioa.s.u-tokyo.ac.jp> - 0.3-5
- Try to fix compilation on ppc.

* Mon Sep 25 2006 Chitlesh Goorah <chitlesh@fedoraproject.org> - 0.3-3
- Added gputils as requires

* Thu Sep 21 2006 Chitlesh Goorah <chitlesh@fedoraproject.org> - 0.3-2
- Removed gputils and gpsim as requires
- Removed autom4te.cache directory
- Owning %%{_datadir}/config.kcfg/
- Removed ldconfig from %%port and %%postun
- Removed gettext, libtool and autoconf as BR

* Fri Sep 15 2006 Chitlesh Goorah <chitlesh@fedoraproject.org> - 0.3-1
- Initial package for Fedora Extras.
