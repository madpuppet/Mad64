git pull
cmake --build build --config release
read -p "Build version: " bld
bp=Mad64_V$bld
mkdir $bp
cp build/Mad64 $bp
cp build/font.ttf $bp
cp build/font.otf $bp
cp build/readme.txt $bp
cp -r build/samples $bp/samples
tar -czvf ${bp}.tar.gz $bp
rm -r $bp
