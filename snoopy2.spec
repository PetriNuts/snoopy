%define version 3.12.1
%define release 1
%define prefix /usr/local
%define src snoopy-%{version}.tar.gz

Summary: A hierarchical Petri net editor
Name: snoopy
Version: %{version}
Release: %{release}
Group: Petri Net editor
Source: %{src}
BuildRoot: %{_tmppath}/%{name}-root
License: BTU Cottbus
URL: http://www-dssz.informatik.tu-cottbus.de/software/snoopy.html
Vendor: BTU Cottbus
Packager: Snoopy Team <snoopy@informatik.tu-cottbus.de>

%description
Snoopy is a software tool to design and animate hierarchical graphs, among
others Petri nets. The tool has been developed - and is still under
Development - at the University of Technology in Cottbus, Dep. of 
Computer Science, "Data Structures and Software Dependability".
The tool is in use for the verification of technical systems, especially
software-based systems, as well as for the validation of natural systems,
i.e. biochemcial networks as metabolic, signal transduction,
gene regulatory networks.


#any preparations before the building starts
%prep
rm -rf $RPM_BUILD_ROOT

#unpack source code quietly 
%setup -q

#begin the build process
%build

#generate Sourcecode list
sh mk_src_lst.sh

autoreconf -fvi
./configure --prefix=%{prefix}
make

%install
make DESTDIR=$RPM_BUILD_ROOT install
mkdir -p $RPM_BUILD_ROOT%{prefix}/lib/%{name}

for snoopylib in `ldd $RPM_BUILD_ROOT%{prefix}/bin/snoopy2 | awk '/local/{print $1}'` ;
do
	cp /usr/local/lib/$snoopylib \
	   $RPM_BUILD_ROOT%{prefix}/lib/%{name}/
done

mkdir -p $RPM_BUILD_ROOT/etc/ld.so.conf.d
touch $RPM_BUILD_ROOT/etc/ld.so.conf.d/snoopy2.conf
echo "%{prefix}/lib/%{name}/" >> $RPM_BUILD_ROOT/etc/ld.so.conf.d/snoopy2.conf

mkdir -p $RPM_BUILD_ROOT%{prefix}/share/%{name}
cp help.zip $RPM_BUILD_ROOT%{prefix}/share/%{name}/

%clean
rm -rf $RPM_BUILD_ROOT

%files
%{prefix}/bin/*
%{prefix}/lib/%{name}/*
%{prefix}/share/%{name}/*
/etc/ld.so.conf.d/snoopy2.conf

%post
ldconfig

%postun
ldconfig

