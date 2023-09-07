file=$(find *.c)
filename=${file%.*}
gcc $file -E -o $filename.i
# 词法分析
clang -E -Xclang -dump-tokens $file > tokens.log 2>&1
# 语法分析
clang -E -Xclang -ast-dump $file > ast.log 2>&1
clang -S -emit-llvm $file
llc -print-before-all -print-after-all $filename.ll > $filename.log 2>&1
llvm-as $filename".ll" -o $filename"_llvm.bc"
gcc $filename.i -S -o $filename"_gcc.S" 
arm-linux-gnueabihf-gcc $filename.i -S -o $filename"_arm.S"
llc $filename".ll" -o $filename"_llvm.S"
gcc $filename"_gcc.S" -c -o $filename"_gcc.o"
arm-linux-gnueabihf-gcc $filename"_arm.S" -o $filename"_arm.o"
llc $filename"_llvm.bc" -filetype=obj -o $filename"_llvm.o"
gcc $filename"_gcc.o" -o $filename
# 生成多个阶段的文件(.dot)
#gcc -O0 -fdump-tree-all-graph $file
