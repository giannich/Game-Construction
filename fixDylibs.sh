#!/bin/bash

#DYLIBS=`find lib/*.dylib -type f`
DYLIBS=`find lib/osgPlugins-3.4.0/*.so -type f`

for lib in $DYLIBS; do
	echo "Fixing $lib:"
	INSTALLLIBS=`otool -L $lib`
	for iLib in $INSTALLLIBS; do
		if [[ $iLib == *"Gianni"* ]]
		then
			echo "Installed lib: $iLib";
			FIXEDPATH=`echo $iLib | sed 's/^.*\/lib/@rpath\/lib\/lib/g'`
			echo "Corrected Path: $FIXEDPATH"
			install_name_tool -change $iLib $FIXEDPATH $lib
		fi
	done
done
