FILE="./exe.o";
g++ -o $FILE $1;
chmod +x $FILE;
$FILE;