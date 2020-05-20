cd "$(dirname "$0")"
cd ..

files=$(find ./src ./tests -name "*.hh" -o -name "*.cc")

for file in $files
do
  clang-format -i $file
done
