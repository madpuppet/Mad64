git pull
cmake --build build --config release
read -p "Build version: " bld
bp=Mad64_DEB_V$bld
mkdir $bp
cp build/Mad64 $bp
cp font.ttf $bp
cp font.otf $bp
cp readme.txt $bp
cp -r samples $bp/samples
tar -czvf ${bp}.tar.gz $bp
rm -r $bp

