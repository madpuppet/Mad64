git pull
cmake --build build --config release
read -p "Build version: " bld
mkdir Mad64_V$fname
bp=Mad64_V$fname
copy build/Mad64 $bp
copy build/font.ttf $bp
copy build/font.otf $bp
copy build/readme.txt $bp
copy -r build/samples $bp/samples
tar -czvf Mad64_V${fname}.tar.gz $bp


