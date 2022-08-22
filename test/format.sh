#! /bin/sh

format="clang-format"

if ! command -v ${format} >/dev/null 2>&1
then
	format="clang-format-13"
fi

if ! command -v ${format} >/dev/null 2>&1
then
	echo "clang-format could not be found"
	exit
fi

echo "Using clang-format: $(${format} --version)"

cd ..
find . -type f -name "*.[ch]pp" \( -path "./test/*" -o -path "./include/*" -o -path "./src/*" \) -prune | xargs ${format} -i
