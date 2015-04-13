#!/bin/bash

BuildArtifactFileName=$1
Release=$2
BUILDDATE=$(date +%Y-%m-%d)
#### create .app ####
rm -r ${BuildArtifactFileName}.app
mkdir -p ${BuildArtifactFileName}.app/Contents/MacOS
cp ${BuildArtifactFileName} ${BuildArtifactFileName}.app/Contents/MacOS/
cp ../Info.plist ${BuildArtifactFileName}.app/Contents/

mkdir -p ${BuildArtifactFileName}.app/Contents/Resources
cp ../${BuildArtifactFileName}.icns ${BuildArtifactFileName}.app/Contents/Resources

mkdir -p ${BuildArtifactFileName}.app/Contents/SharedSupport
cp ../help.zip ${BuildArtifactFileName}.app/Contents/SharedSupport

mkdir -p ${BuildArtifactFileName}.app/Contents/Frameworks
cd ${BuildArtifactFileName}.app/Contents/Frameworks

echo "### check ${BuildArtifactFileName} dependencies ###"

while read -d $'\n' f ; do
  if [[ "${f}" == "/usr/local/"*".dylib"* || "${f}" == "${HOME}"*".dylib"* ]]
  then
    fullfile="${f% (*}"
    fullfilename="${fullfile%.*}"
    filebase=$(basename "$fullfile")
    extension="${filebase##*.}"
    filename="${filebase%.*}"
    if [[ -e "${fullfile}" ]]
    then
      if [[ -h "${fullfile}" && ( `dirname "$(readlink ${fullfile})"` != . ) ]]
      then
        MYPWD="$PWD"
        cd "$(dirname ""$fullfile"")"
        symfile=$(readlink "$fullfile")
        cd "$(dirname ""$symfile"")"
        cp -a "${filename}"* "${MYPWD}"/
        cd "$MYPWD"
        chmod 755 "${filename}"*
      else
        cp -a "${fullfilename}"* .
      fi
    fi
    install_name_tool -id @executable_path/../Frameworks/"${filebase}" "${filebase}"
    install_name_tool -change "${fullfile}" @executable_path/../Frameworks/"${filebase}" ../MacOS/"${BuildArtifactFileName}"
    if [[ -e "${filebase}" ]]
    then
      echo "copied ${filebase}"
    else
      echo "warning: ${filebase} not found"
    fi
  fi
done< <(otool -L ../MacOS/"${BuildArtifactFileName}")

echo "### check other dependencies ###"

checkMore=true
until [[ "$checkMore" == false ]] ;
do
	checkMore=false
	for val in * ; do
		if [[ -e "${val}" && ! -h "${val}" ]]
		then
      while read -d $'\n' f ; do
        if [[ "$f" == "/usr/local/"*".dylib"* || "$f" == "${HOME}"*".dylib"* ]]
        then
          checkMore=true
          fullfile="${f% (*}"
          fullfilename="${fullfile%.*}"
          filebase=$(basename "$fullfile")
          extension="${filebase##*.}"
          filename="${filebase%.*}"
          if [[ -e "${filebase}" ]]
          then
            install_name_tool -change "${fullfile}" @executable_path/../Frameworks/"${filebase}" "${val}"
          else
            if [[ -h "${fullfile}" && ( `dirname "$(readlink ${fullfile})"` != . ) ]]
            then
              MYPWD="$PWD"
              cd "$(dirname ""$fullfile"")"
              symfile=$(readlink "$fullfile")
              cd "$(dirname ""$symfile"")"
              cp -a "${filename}"* "${MYPWD}"/
              cd "$MYPWD"
              chmod 755 "${filename}"*
            else
              cp -a "${fullfilename}"* .
            fi
            install_name_tool -id @executable_path/../Frameworks/"${filebase}" "${filebase}"
            install_name_tool -change "${fullfile}" @executable_path/../Frameworks/"${filebase}" "$val"
            echo "copied ${filebase}"
          fi
        fi
			done< <(otool -L "$val")
		fi
	done
done

cd ../../..

echo "#### create .dmg ####"

cp ../snoopy.dmg .
hdiutil convert snoopy.dmg -format UDSP -o snoopy
rm snoopy.dmg
hdiutil resize -size 100m snoopy.sparseimage
hdiutil mount snoopy.sparseimage
cp -R "${BuildArtifactFileName}".app /Volumes/Snoopy/
hdiutil eject /Volumes/Snoopy
hdiutil convert snoopy.sparseimage -format UDBZ -o snoopy.dmg
rm snoopy.sparseimage
mv snoopy.dmg snoopy-"${Release}"-macosx-intel-"${BUILDDATE}".dmg
