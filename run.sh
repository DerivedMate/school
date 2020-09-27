if [ ! -d gif-src ]; then 
  mkdir gif-src
  echo "Created gif-src"
fi;

len=`ls ./gif-src | wc -l`
if [ "${len}" -gt 0 ] ; then 
  rm gif-src/*
  echo "Removed outdated part files"
fi;

FILE="./exe.o";
g++ -o $FILE $1 && chmod +x $FILE && $FILE $2;

echo
cd gif-src;
convert -delay 8 -loop 0 `ls -v` ../the-gif.gif 