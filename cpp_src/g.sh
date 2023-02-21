g++ main.cpp
if [ $? -gt 0 ];
then
	exit
else
	./a.out -f pure_dump
fi
