git pull
cmake --build build --config release
read -p "Build version: " bld
mkdir Mad64_V$fname
bp=Mad64_V$fname
cp build/Mad64 $bp
cp build/font.ttf $bp
cp build/font.otf $bp
cp build/readme.txt $bp
cp -r build/samples $bp/samples
tar -czvf Mad64_V${fname}.tar.gz $bp


