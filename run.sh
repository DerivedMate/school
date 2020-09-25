rm ./gif-src/*;

FILE="./exe.o";
g++ -o $FILE $1 && chmod +x $FILE && $FILE;

cd gif-src;
convert -delay 8 -loop 0 `ls -v` ../the-gif.gif 